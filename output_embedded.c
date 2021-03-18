/* 
 *  Squeezelite - lightweight headless squeezebox emulator
 *
 *  (c) Adrian Smith 2012-2015, triode1@btinternet.com
 *      Ralph Irving 2015-2017, ralph_irving@hotmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "squeezelite.h"

static log_level loglevel;

static bool running = true;

extern struct outputstate output;
extern struct buffer *outputbuf;

#define LOCK   mutex_lock(outputbuf->mutex)
#define UNLOCK mutex_unlock(outputbuf->mutex)

#define FRAME_BLOCK MAX_SILENCE_FRAMES

extern u8_t *silencebuf;

static u8_t *obuf;
static int bytes_per_frame;
static thread_type thread;

static int _embedded_write_frames(frames_t out_frames, bool silence, s32_t gainL, s32_t gainR, u8_t flags,
								s32_t cross_gain_in, s32_t cross_gain_out, ISAMPLE_T **cross_ptr);
static void *output_thread();

void set_volume(unsigned left, unsigned right) {
	LOG_DEBUG("setting internal gain left: %u right: %u", left, right);
	LOCK;
	output.gainL = left;
	output.gainR = right;
	UNLOCK;
}

void output_init_embedded(log_level level, char *device, unsigned output_buf_size, char *params, unsigned rates[], unsigned rate_delay, unsigned idle) {
	loglevel = level;

	LOG_INFO("init output EMBEDDED");
	
	memset(&output, 0, sizeof(output));

#if BYTES_PER_FRAME == 4
	output.format = S16_LE;
#else 
	output.format = S32_LE;
#endif	
	output.start_frames = FRAME_BLOCK * 2;
	output.write_cb = &_embedded_write_frames;
	output.rate_delay = rate_delay;

	if (params) {
		if (!strcmp(params, "32"))	output.format = S32_LE;
		if (!strcmp(params, "24")) output.format = S24_3LE;
		if (!strcmp(params, "16")) output.format = S16_LE;
	}
	
	output_init_common(level, device, output_buf_size, rates, idle);

#if LINUX || OSX || FREEBSD || EMBEDDED
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#ifdef PTHREAD_STACK_MIN
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + OUTPUT_THREAD_STACK_SIZE);
#endif
	pthread_create(&thread, &attr, output_thread, NULL);
	pthread_attr_destroy(&attr);
#if HAS_PTHREAD_SETNAME_NP	
	pthread_setname_np(thread, "output");
#endif	
#endif
#if WIN
	thread = CreateThread(NULL, OUTPUT_THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)&output_thread, NULL, 0, NULL);
#endif
}

void output_close_embedded(void) {
	LOG_INFO("close output");

	LOCK;
	running = false;
	UNLOCK;
#if LINUX || OSX || FREEBSD || EMBEDDED
	pthread_join(thread, NULL);
#endif
	output_close_common();
}

static int _embedded_write_frames(frames_t out_frames, bool silence, s32_t gainL, s32_t gainR, u8_t flags,
								s32_t cross_gain_in, s32_t cross_gain_out, ISAMPLE_T **cross_ptr) {
#if BYTES_PER_FRAME == 8									
	s32_t *optr;
#endif	
	
	if (!silence) {
		if (output.fade == FADE_ACTIVE && output.fade_dir == FADE_CROSS && *cross_ptr) {
			_apply_cross(outputbuf, out_frames, cross_gain_in, cross_gain_out, cross_ptr);
		}
		
#if BYTES_PER_FRAME == 4
		if (gainL != FIXED_ONE || gainR!= FIXED_ONE) {
			_apply_gain(outputbuf, out_frames, gainL, gainR, flags);
		}
			
		memcpy(obuf, outputbuf->readp, out_frames * bytes_per_frame);
#else
		optr = (s32_t*) outputbuf->readp;	
#endif		
	} else {
#if BYTES_PER_FRAME == 4		
		memcpy(obuf, silencebuf, out_frames * bytes_per_frame);
#else		
		optr = (s32_t*) silencebuf;
#endif	
	}

#if BYTES_PER_FRAME == 8
	IF_DSD(
	if (output.outfmt == DOP) {
			update_dop((u32_t *) optr, out_frames, output.invert);
		} else if (output.outfmt != PCM && output.invert)
			dsd_invert((u32_t *) optr, out_frames);
	)

	_scale_and_pack_frames(obuf, optr, out_frames, gainL, gainR, output.format);
#endif	

	return out_frames;
}

static void *output_thread() {
	int frames = 0;

#if BYTES_PER_FRAME == 8
	LOCK;

	switch (output.format) {
	case S32_LE:
		bytes_per_frame = 4 * 2; break;
	case S24_3LE:
		bytes_per_frame = 3 * 2; break;
	case S16_LE:
		bytes_per_frame = 2 * 2; break;
	default:
		bytes_per_frame = 4 * 2; break;
		break;
	}

	UNLOCK;
#else	
	bytes_per_frame = BYTES_PER_FRAME;
#endif

	obuf = malloc(FRAME_BLOCK * bytes_per_frame);

	while (running) {

		LOCK;
		
		if (output.state == OUTPUT_OFF) {
			UNLOCK;
			usleep(500000);
			continue;
		}		
			
		output.device_frames = 0;
		output.updated = gettime_ms();
		output.frames_played_dmp = output.frames_played;

		frames = _output_frames(FRAME_BLOCK);
		
		UNLOCK;

		if (frames) {
			if (output.device[0] == '-' && memcmp(obuf, silencebuf, frames * bytes_per_frame)) {
				fwrite(obuf, bytes_per_frame, frames, stdout);
				LOG_SDEBUG("writing frames %d", frames);
			} else {	
				// do something with some of these frames...
				usleep((frames * 1000 * 1000) / output.current_sample_rate);			
			}	
			frames = 0;
		} else {
			usleep((FRAME_BLOCK * 1000 * 1000) / output.current_sample_rate);
		}	
		
	}

	return 0;
}

bool test_open(const char *device, unsigned rates[], bool userdef_rates) {
	unsigned _rates[] = { 96000, 88200, 48000, 44100, 32000, 0 };	
	memcpy(rates, _rates, sizeof(_rates));
	return true;
}


