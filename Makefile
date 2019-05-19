# Cross compile support - create a Makefile which defines these three variables and then includes this Makefile...
CFLAGS  ?= -Wall -fPIC -O2 -DLINKALL -DLOOPBACK -DDACAUDIO
LDADD   ?= -lpthread -lm -lrt 
EXECUTABLE ?= squeezelite-core

SOURCES = \
	main.c slimproto.c buffer.c stream.c utils.c \
	output.c output_pack.c output_dac.c decode.c \
	pcm.c

LINK_LINUX       = -ldl

DEPS             = squeezelite.h slimproto.h

OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LDADD) -o $@

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPTS) $< -c -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
