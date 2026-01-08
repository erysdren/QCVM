
EXEC?=sieve
RM?=rm -f
CC?=gcc
QCC?=fteqcc
PKGCONFIG?=pkg-config

override CFLAGS+=-O3 -flto -Wextra -Wall -Werror
override LDFLAGS+=-O3

OBJECTS=sieve.o qcvm_reckless.o
PROGS=sieve.dat
PROGSRC=sieve.qc

all: clean $(EXEC) $(PROGS)

clean:
	$(RM) $(OBJECTS) $(EXEC) $(PROGS)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(PROGS): $(PROGSRC)
	$(QCC) $^
