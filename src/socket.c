#include "src/socket.h"
#include "src/log.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static bool new_socket(int *fd);

static struct sockaddr_in sock_addr(in_addr_t addr, int port);

bool server_socket(in_addr_t addr, int port, Socket *sock) {
  if (!new_socket(&sock->fd)) {
    return false;
  }

  // make the socket rebind the the port
  int opt = 1;
  if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0) {
    LOG_ERROR("server_socket setsockopt: %m");
    return false;
  }

  sock->sockaddr_in = sock_addr(addr, port);
  if (bind(sock->fd, (struct sockaddr *)&sock->sockaddr_in,
           sizeof(sock->sockaddr_in)) < 0) {
    LOG_ERROR("server_socket bind: %m");
    return false;
  }
  if (listen(sock->fd, 5) < 0) {
    LOG_ERROR("server_socket listen: %m");
    return false;
  }
  return true;
}

bool accept_connection(int fd, Socket *sock) {
  struct sockaddr_in client_addr = {0};
  socklen_t len = sizeof(client_addr);
  int conn_fd = accept(fd, (struct sockaddr *)&client_addr, &len);
  if (conn_fd < 0) {
    LOG_ERROR("accept_connection: %m");
    return false;
  }
  sock->fd = conn_fd;
  sock->sockaddr_in = client_addr;
  return true;
}

bool client_connect(in_addr_t server_addr, int port, Socket *sock) {
  if (!new_socket(&sock->fd)) {
    return false;
  }
  sock->sockaddr_in = sock_addr(server_addr, port);
  if (connect(sock->fd, (struct sockaddr *)&sock->sockaddr_in,
              sizeof(sock->sockaddr_in)) < 0) {
    LOG_ERROR("client_connect: %m");
    return false;
  };
  return true;
}

bool socket_send(int fd, char *msg, int msg_len) {
  size_t sent = 0;
  while (sent < (size_t)msg_len) {
    ssize_t n = send(fd, msg + sent, msg_len - sent, MSG_NOSIGNAL);
    if (n < 0) {
      LOG_ERROR("socket_send: %m");
      return false;
    }
    sent += (size_t)n;
  }
  return true;
}

bool socket_read_str(int fd, char *msg, int msg_len) {
  size_t len = 0;
  while (len < (size_t)msg_len - 1) {
    ssize_t n_read = read(fd, msg + len, msg_len - 1 - len);
    if (n_read < 0) {
      LOG_ERROR("socket_read: %m");
      return false;
    }
    if (n_read == 0) {
      LOG_WARN("Client disconnected");
      return false;
    }
    len += (size_t)n_read;
    char *end = memchr(msg, '\0', len);
    if (end != NULL) {
      return true; /* '\0' is the message terminator, stripped by read */
    }
  }
  /* Buffer full without a terminator: discard the rest of the message so the
     next read starts at a message boundary. */
  LOG_ERROR("Message overflow");
  char discard[64];
  while (true) {
    ssize_t n = read(fd, discard, sizeof discard);
    if (n <= 0) {
      return false; /* read error or disconnect */
    }
    if (memchr(discard, '\0', (size_t)n) != NULL) {
      break;
    }
  }
  return false;
}

bool socket_close(int fd) {
  if (close(fd) < 0) {
    LOG_ERROR("Cannot close socket %d", fd);
    return false;
  }
  return true;
}

static bool new_socket(int *fd) {
  *fd = socket(AF_INET, SOCK_STREAM, 0);
  if (*fd < 0) {
    LOG_ERROR("new_socket: %m");
    return false;
  }
  return true;
}

static struct sockaddr_in sock_addr(in_addr_t addr, int port) {
  return (struct sockaddr_in){
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr = {addr},
  };
}
void socket_to_str(const Socket *sock, char *buf, size_t len) {
  char addr_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &sock->sockaddr_in.sin_addr, addr_str, sizeof addr_str);
  snprintf(buf, len, "Socket{fd=%d, addr=%s, port=%d}", sock->fd, addr_str,
           ntohs(sock->sockaddr_in.sin_port));
}
