
#include "src/utils.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

bool parse_int(const char *s, int *i, int min, int max) {
  char *end;
  errno = 0;
  long v = strtol(s, &end, 10);
  if (end == s || *end != '\0')
    return false;
  if (errno == ERANGE || v < min || v > max)
    return false;
  *i = (int)v;
  return true;
}

bool parse_str(const char *s, size_t min, size_t max) {
  size_t len = strnlen(s, max + 1);
  if (len > max || len < min) {
    return false;
  }
  return true;
}
