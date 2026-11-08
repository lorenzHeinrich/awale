
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

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
