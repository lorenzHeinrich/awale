#ifndef CLIENTS_H
#define CLIENTS_H

#include "src/socket.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_CLIENTS 128
#define PLAYER_NAME_MAX_LENGTH 31

struct Match;

typedef struct GameClient {
  char name[PLAYER_NAME_MAX_LENGTH];
  Socket socket;
  struct Match *current_match; /* NULL = no match */
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

/* Two-slot transaction (same lock, so atomic): e.g. invite touches both
   players at once. */
typedef void (*ClientPairFn)(GameClient *a, GameClient *b, void *ctx);
void clients_with_two_slots(ClientRegistry *registry, size_t slot_a,
                            size_t slot_b, ClientPairFn fn, void *ctx);

/* Clears and frees the match of the client in slot (and the opponent's
   pointer). Call on disconnect. */
void clients_clear_match(ClientRegistry *registry, size_t slot);

#endif // !CLIENTS_H
