#include "src/match.h"
#include "src/log.h"
#include <stdlib.h>
#include <string.h>

Match *match_new(const char *name_a, const char *name_b, size_t slot_a,
                 size_t slot_b) {
  Match *m = malloc(sizeof *m);
  m->game = new_game((char *)name_a, (char *)name_b);
  m->slot_a = slot_a;
  m->slot_b = slot_b;
  m->started = false;
  pthread_mutex_init(&m->lock, NULL);
  return m;
}

void match_destroy(Match *match) {
  pthread_mutex_destroy(&match->lock);
  free(match);
}

void with_match(Match *match, MatchFn fn, void *ctx) {
  pthread_mutex_lock(&match->lock);
  fn(match, ctx);
  pthread_mutex_unlock(&match->lock);
}

void match_record(ClientRegistry *registry, Match *match) {
  pthread_mutex_lock(&registry->clients_lock);
  for (int s = 0; s < 2; s++) {
    size_t slot = s == 0 ? match->slot_a : match->slot_b;
    size_t other = s == 0 ? match->slot_b : match->slot_a;
    GameClient *client = &registry->clients[slot];
    if (client->n_history >= MAX_HISTORY) {
      continue;
    }
    MatchRecord *rec = &client->history[client->n_history++];
    rec->game = match->game;
    strncpy(rec->opponent, registry->clients[other].name,
            PLAYER_NAME_MAX_LENGTH - 1);
    rec->opponent[PLAYER_NAME_MAX_LENGTH - 1] = '\0';
  }
  pthread_mutex_unlock(&registry->clients_lock);
  LOG_INFO("recorded match (slots %zu/%zu) to both players' history",
           match->slot_a, match->slot_b);
}
