# squeezelite-core
just a core of squeezelite, but modified to be compatible with embedded systems

new EMBEDDED #define 
- requires embedded.h to provide some platform specific system items
- see output_embedded.that c explains what is expected from the platform for audio output

Working codecs: mp3, pcm, flac, aac (faad or helix), alac (native, not using ffmepg), vorbis (improved)

Vorbis has a TREMOR_ONLY define to force use of tremor version of vorbis

Can use soxr for resampling or a faster but less accurate 16 bits fixed point resampler (see resample16 repository)

Can use 16 or 32 bits interim sample size (BYTES_PER_FRAME) to save outputbuf space (works with all above codecs)

Can use loopback interface is PIPE and EVENTFD are not available

Demo/example version can be compiled under Linux with EMBEDDED defined, it will either discard samples or send them to stdout
