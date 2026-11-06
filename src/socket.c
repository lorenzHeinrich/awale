#include "src/socket.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>

static bool new_socket(int *fd);

static struct sockaddr_in sock_addr(in_addr_t addr, int port);

bool server_socket(in_addr_t addr, int port, Socket *sock) {
  if (!new_socket(&sock->fd)) {
    return false;
  }
  sock->sockaddr_in = sock_addr(addr, port);
  if (bind(sock->fd, (struct sockaddr *)&sock->sockaddr_in,
           sizeof(sock->sockaddr_in)) < 0) {
    perror("server_socket bind");
    return false;
  }
  if (listen(sock->fd, 5) < 0) {
    perror("server_socket listen");
    return false;
  }
  return true;
}

bool accept_connection(int fd, Socket *sock) {
  struct sockaddr_in client_addr = {0};
  socklen_t len = sizeof(client_addr);
  int conn_fd = accept(fd, (struct sockaddr *)&client_addr, &len);
  if (conn_fd < 0) {
    perror("accept_connection");
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
    return false;
  };
  return true;
}

static bool new_socket(int *fd) {
  *fd = socket(AF_INET, SOCK_STREAM, 0);
  if (*fd < 0) {
    perror("new_socket");
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
