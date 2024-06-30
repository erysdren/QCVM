
EXEC ?= test
RM ?= rm -f
CC ?= gcc
QCC ?= fteqcc
PKGCONFIG ?= pkg-config

override CFLAGS += -g3 -std=c89 -pedantic -Wextra -Wall -Werror -fsanitize=address -fanalyzer
override LDFLAGS += -g3 -fsanitize=address -fanalyzer

OBJECTS = test.o qcvm.o

all: clean $(EXEC)

clean:
	$(RM) $(OBJECTS) $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
