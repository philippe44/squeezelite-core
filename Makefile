# Cross compile support - create a Makefile which defines these three variables and then includes this Makefile...
CFLAGS  ?= -Wall -fPIC -O2 -DLINKALL -DLOOPBACK -DDACAUDIO -DNO_FAAD -DTREMOR_ONLY -DBYTES_PER_FRAME=4 -fstack-usage
LDADD   ?= -lpthread -lm -lrt -lstdc++
EXECUTABLE ?= squeezelite-core

BASE = /media/Main\ Share/Development

INCLUDE = -I. \
	-I$(BASE)/libmad \
	-I$(BASE)/helix-aac \
	-I$(BASE)/flac/include \
	-I$(BASE)/tremor -I$(BASE)/libogg/include \
	-I$(BASE)/alac
#	-I$(BASE)/faad2/include 	

SOURCES = \
	main.c slimproto.c buffer.c stream.c utils.c \
	output.c output_pack.c output_dac.c decode.c \
	pcm.c mad.c helix-aac.c flac.c vorbis.c alac.c mpg.c

LIBRARIES = libmad.a libhelix-aac.a libflac.a libvorbisidec.a libogg.a libalac.a 

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
