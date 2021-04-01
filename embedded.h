#ifndef EMBEDDED_H
#define EMBEDDED_H

#include <inttypes.h>

/* 	must provide 
		- mutex_create_p
		- pthread_create_name
		- register_xxx (see below)
		- stack size
		- s16_t, s32_t, s64_t and u64_t
		- PLAYER_ID / custom_player_id
	can overload (use #define)
		- exit
		- gettime_ms
		- BASE_CAP
		- EXT_BSS 		
	recommended to add platform specific include(s) here
*/	


typedef u_int8_t  u8_t;
typedef u_int16_t u16_t;
typedef u_int32_t u32_t;
typedef u_int64_t u64_t;
typedef int16_t   s16_t;
typedef int32_t   s32_t;
typedef int64_t   s64_t;
	
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN	256
#endif

#define STREAM_THREAD_STACK_SIZE  64 * 1024
#define DECODE_THREAD_STACK_SIZE 128 * 1024
#define OUTPUT_THREAD_STACK_SIZE  64 * 1024
#define IR_THREAD_STACK_SIZE      64 * 1024

// number of 5s times search for a server will happen beforee slimproto exits (0 = no limit)
#define MAX_SERVER_RETRIES	5

// or can point to a variable that is set at runtime
#define PLAYER_ID 12

#if BYTES_PER_FRAME == 8
#define BASE_CAP "Model=squeezeesp32,AccuratePlayPoints=1,HasDigitalOut=1,HasPolarityInversion=1,Balance=1,Depth=32,Firmware=" VERSION 
#else
#define BASE_CAP "Model=squeezeesp32,AccuratePlayPoints=1,HasDigitalOut=1,HasPolarityInversion=1,Balance=1,Depth=16,Firmware=" VERSION 
#endif

// to force some special buffer attribute
#define EXT_BSS

#define exit(code) { int ret = code; pthread_exit(&ret); }

#define mutex_create_p(m) pthread_mutexattr_t attr; pthread_mutexattr_init(&attr); pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT); pthread_mutex_init(&m, &attr); pthread_mutexattr_destroy(&attr)
#define pthread_create_name(t,a,f,p,n) pthread_create(t,a,f,p)

// must provide	of #define as empty macros	
#define 	embedded_init()
#define 	register_external()
#define 	deregister_external()
#define		decode_restore(external)
// used when other client wants to use slimproto socket to send messages
#define LOCK_P
#define UNLOCK_P

// bitmap of plugs status
#define PLUG_LINE_IN 	0x01
#define PLUG_LINE_OUT	0x02
#define PLUG_HEADPHONE	0x04
#define get_RSSI()	0xffff
#define get_plugged()	0
#define get_battery()	0

// set name 
void set_name(char *name);		// can be defined as an empty macro

// to be defined to nothing if you don't want to support these
extern struct visu_export_s {
	pthread_mutex_t mutex;
	u32_t level, size, rate;
	s16_t *buffer;
	bool running;
} visu_export;


// optional, please chain if used 
bool		(*slimp_handler)(u8_t *data, int len);
void 		(*slimp_loop)(void);
void 		(*server_notify)(in_addr_t ip, u16_t hport, u16_t cport);
				   
#endif // EMBEDDED_H
