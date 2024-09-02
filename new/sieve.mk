
EXEC?=sieve
RM?=rm -f
CC?=gcc
QCC?=fteqcc
PKGCONFIG?=pkg-config

override CFLAGS+=-O2 -std=c89 -pedantic -Wextra -Wall -Werror
override LDFLAGS+=-O2

OBJECTS=sieve.o qcvm.o
PROGS=sieve.dat
PROGSRC=sieve.qc

all: clean $(EXEC) $(PROGS)

clean:
	$(RM) $(OBJECTS) $(EXEC) $(PROGS)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(PROGS): $(PROGSRC)
	$(QCC) $^
