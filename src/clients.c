#include "src/clients.h"
#include "src/log.h"
#include "src/match.h"
#include <pthread.h>

void clients_init(ClientRegistry *registry) {
  registry->n_connections = 0;
  pthread_mutex_init(&registry->clients_lock, NULL);
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    registry->clients[i].socket.fd = -1;
    registry->clients[i].current_match = NULL;
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
  if (slot < 0) {
    LOG_WARN("registry full (%d slots), no free slot", MAX_CLIENTS);
  }
  return slot;
}

void disconnect_client(ClientRegistry *registry, size_t slot) {
  pthread_mutex_lock(&registry->clients_lock);
  registry->clients[slot].socket.fd = -1;
  pthread_mutex_unlock(&registry->clients_lock);
  LOG_DEBUG("freed client slot %zu", slot);
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

void clients_with_two_slots(ClientRegistry *registry, size_t slot_a,
                            size_t slot_b, ClientPairFn fn, void *ctx) {
  pthread_mutex_lock(&registry->clients_lock);
  fn(&registry->clients[slot_a], &registry->clients[slot_b], ctx);
  pthread_mutex_unlock(&registry->clients_lock);
}

void clients_clear_match(ClientRegistry *registry, size_t slot) {
  pthread_mutex_lock(&registry->clients_lock);
  Match *m = registry->clients[slot].current_match;
  if (m != NULL) {
    registry->clients[m->slot_a].current_match = NULL;
    registry->clients[m->slot_b].current_match = NULL;
  }
  pthread_mutex_unlock(&registry->clients_lock);
  if (m != NULL) {
    LOG_INFO("ending match of slot %zu (slots %zu/%zu)", slot, m->slot_a,
             m->slot_b);
    match_destroy(m);
  }
}
