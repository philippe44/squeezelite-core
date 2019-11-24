#ifndef EMBEDDED_H
#define EMBEDDED_H

#include <inttypes.h>

/* 	must provide 
		- mutex_create_p
		- pthread_create_name
		- register_other (an option to register different ways to fill the outputbuf)
		- stack size
		- s16_t, s32_t, s64_t and u64_t
	can overload (use #define)
		- exit
		- gettime_ms
		- BASE_CAP
		- EXT_BSS 		
	recommended to add platform specific include(s) here
*/	
	
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN	256
#endif

#define STREAM_THREAD_STACK_SIZE  64 * 1024
#define DECODE_THREAD_STACK_SIZE 128 * 1024
#define OUTPUT_THREAD_STACK_SIZE  64 * 1024
#define IR_THREAD_STACK_SIZE      64 * 1024


typedef u_int8_t  u8_t;
typedef u_int16_t u16_t;
typedef u_int32_t u32_t;
typedef u_int64_t u64_t;
typedef int16_t   s16_t;
typedef int32_t   s32_t;
typedef int64_t   s64_t;

#define exit(code) { int ret = code; pthread_exit(&ret); }

#define mutex_create_p(m) pthread_mutexattr_t attr; pthread_mutexattr_init(&attr); pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT); pthread_mutex_init(&m, &attr); pthread_mutexattr_destroy(&attr)
#define pthread_create_name(t,a,f,p,n) pthread_create(t,a,f,p)

// these are here as they can be #define to nothing			
void 		register_external(void);
void 		deregister_external(void);
				   
#endif // EMBEDDED_H
