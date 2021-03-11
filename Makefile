# Cross compile support - create a Makefile which defines these three variables and then includes this Makefile...

EMBEDDED = 1

CFLAGS  = -Wall -fPIC -O2 -DLINKALL -DLOOPBACK -DNO_FAAD -DTREMOR_ONLY -DBYTES_PER_FRAME=4 -fstack-usage
LDADD   ?= -lpthread -lm -lrt -lstdc++ -lgomp
EXECUTABLE ?= squeezelite-core

BASE = /media/Main\ Share/Development

INCLUDE = -I. \
	-I$(BASE)/libmad \
	-I$(BASE)/helix-aac \
	-I$(BASE)/flac/include \
	-I$(BASE)/tremor -I$(BASE)/libogg/include \
	-I$(BASE)/libopus/include -I$(BASE)/libopusfile/include\
	-I$(BASE)/soxr/src \
	-I$(BASE)/alac \
	-I$(BASE)/libresample16
#	-I$(BASE)/faad2/include 	

SOURCES = \
	main.c slimproto.c buffer.c stream.c utils.c \
	output.c output_pack.c decode.c \
	process.c \
	pcm.c mad.c helix-aac.c flac.c vorbis.c alac.c mpg.c opus.c

ifeq ($(EMBEDDED),1)
SOURCES += output_embedded.c embedded.c resample16.c
CFLAGS += -DEMBEDDED -DRESAMPLE16
else
SOURCES += output_stdout.c resample.c output_pa.c output_alsa.c
CFLAGS += -DRESAMPLE
endif

LIBRARIES = libmad.a libhelix-aac.a libflac.a libvorbisidec.a libogg.a libalac.a libsoxr.a libresample16.a libopusfile.a libopus.a

LINK_LINUX       = -ldl

DEPS             = squeezelite.h slimproto.h

OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARIES) $(LDFLAGS) $(LDADD) -o $@

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPTS) $(INCLUDE) $< -c -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
