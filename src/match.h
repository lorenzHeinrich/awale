#ifndef MATCH_H
#define MATCH_H

#include "src/awale/awale.h"
#include "src/clients.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct Match {
  Game game;
  size_t slot_a;
  size_t slot_b;
  bool started; /* false = pending invite */
  pthread_mutex_t lock;
} Match;

Match *match_new(const char *name_a, const char *name_b, size_t slot_a,
                 size_t slot_b);
void match_destroy(Match *match);

typedef void (*MatchFn)(Match *match, void *ctx);
void with_match(Match *match, MatchFn fn, void *ctx);

/* Appends the match's final state to both players' history (registry lock
   held internally). */
void match_record(ClientRegistry *registry, Match *match);

#endif // !MATCH_H
