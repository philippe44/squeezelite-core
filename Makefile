# Cross compile support - create a Makefile which defines these three variables and then includes this Makefile...
CFLAGS  ?= -Wall -fPIC -O2 -DLINKALL -DLOOPBACK -DDACAUDIO -DTREMOR_ONLY -fstack-usage
LDADD   ?= -lpthread -lm -lrt -lstdc++
EXECUTABLE ?= squeezelite-core

BASE = /media/Main\ Share/Development

INCLUDE = -I. \
	-I$(BASE)/libmad \
	-I$(BASE)/faad2/include \
	-I$(BASE)/flac/include \
	-I$(BASE)/tremor -I$(BASE)/libogg/include \
	-I$(BASE)/alac

SOURCES = \
	main.c slimproto.c buffer.c stream.c utils.c \
	output.c output_pack.c output_dac.c decode.c \
	pcm.c mad.c faad.c flac.c vorbis.c alac.c mpg.c

LIBRARIES = libmad.a libfaad.a libflac.a libvorbisidec.a libogg.a libalac.a 

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
