#ifndef SOCKET_H

#define SOCKET_H

#include <netinet/in.h>
#include <stdbool.h>

typedef struct Socket {
  int fd;
  struct sockaddr_in sockaddr_in;
} Socket;

bool server_socket(in_addr_t addr, int port, Socket *sock);

bool accept_connection(int fd, Socket *sock);

bool client_connect(in_addr_t server_addr, int port, Socket *sock);

#endif // !SOCKET_H
