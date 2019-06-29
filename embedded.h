#ifndef EMBEDDED_H
#define EMBEDDED_H

#include <inttypes.h>

#define HAS_MUTEX_CREATE_P		1
#define HAS_PTHREAD_SETNAME_NP	0

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN	256
#endif

typedef u_int8_t  u8_t;
typedef u_int16_t u16_t;
typedef u_int32_t u32_t;
typedef u_int64_t u64_t;
typedef int16_t   s16_t;
typedef int32_t   s32_t;
typedef int64_t   s64_t;

#define exit(code) { int ret = code; pthread_exit(&ret); }

#endif // EMBEDDED_H
