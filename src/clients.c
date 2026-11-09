#include "src/clients.h"
#include <pthread.h>

void clients_init(ClientRegistry *registry) {
  registry->n_connections = 0;
  pthread_mutex_init(&registry->clients_lock, NULL);
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    registry->clients[i].socket.fd = -1;
  }
}

int next_free_client_slot(ClientRegistry *registry) {
  pthread_mutex_lock(&registry->clients_lock);
  int slot = -1;
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (registry->clients[i].socket.fd == -1) {
      slot = (int)i;
      break;
    }
  }
  if (slot == -1 && registry->n_connections < MAX_CLIENTS) {
    slot = (int)registry->n_connections;
    registry->n_connections++;
  }
  pthread_mutex_unlock(&registry->clients_lock);
  return slot;
}

void disconnect_client(ClientRegistry *registry, size_t slot) {
  pthread_mutex_lock(&registry->clients_lock);
  registry->clients[slot].socket.fd = -1;
  pthread_mutex_unlock(&registry->clients_lock);
}

void clients_with_slot(ClientRegistry *registry, size_t slot, ClientFn fn,
                       void *ctx) {
  pthread_mutex_lock(&registry->clients_lock);
  fn(&registry->clients[slot], ctx);
  pthread_mutex_unlock(&registry->clients_lock);
}

void clients_for_each(ClientRegistry *registry, ClientVisitor fn, void *ctx) {
  pthread_mutex_lock(&registry->clients_lock);
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    fn(&registry->clients[i], i, ctx);
  }
  pthread_mutex_unlock(&registry->clients_lock);
}
