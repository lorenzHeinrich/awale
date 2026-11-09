#ifndef CLIENTS_H
#define CLIENTS_H

#include "src/socket.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_CLIENTS 128
#define PLAYER_NAME_MAX_LENGTH 31

typedef struct GameClient {
  char name[PLAYER_NAME_MAX_LENGTH];
  Socket socket;
} GameClient;

typedef struct ClientRegistry {
  GameClient clients[MAX_CLIENTS];
  size_t n_connections;
  pthread_mutex_t clients_lock;
} ClientRegistry;

void clients_init(ClientRegistry *registry);

int next_free_client_slot(ClientRegistry *registry);

void disconnect_client(ClientRegistry *registry, size_t slot);

typedef void (*ClientFn)(GameClient *client, void *ctx);
void clients_with_slot(ClientRegistry *registry, size_t slot, ClientFn fn,
                       void *ctx);

typedef void (*ClientVisitor)(const GameClient *client, size_t slot, void *ctx);
void clients_for_each(ClientRegistry *registry, ClientVisitor fn, void *ctx);

#endif // !CLIENTS_H
