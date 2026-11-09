#include "src/match.h"
#include <stdlib.h>

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
