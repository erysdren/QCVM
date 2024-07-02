
EXEC ?= test
RM ?= rm -f
CC := wcl386
QCC ?= fteqcc
PKGCONFIG ?= pkg-config

override CFLAGS = -we -wx
override LDFLAGS = -bcl=dos32x

SOURCES = test.c qcvm.c

all: clean $(EXEC)

clean:
	$(RM) *.obj *.o *.exe $(EXEC)

$(EXEC): $(SOURCES)
	$(CC) -fe $@ $^ $(LDFLAGS)
