#ifndef STMOS_UTIL_CEXTS_H
#define STMOS_UTIL_CEXTS_H

#define LIKELY(expr) __builtin_expect((expr), 1)
#define UNLIKELY(expr) __builtin_expect((expr), 0)

#endif
