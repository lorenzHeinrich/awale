#ifndef UTILS_H

#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

bool parse_int(const char *s, int *i, int min, int max);

bool parse_str(const char *s, size_t min, size_t max);

#endif // !UTILS_H
