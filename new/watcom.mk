
!ifndef TARGET
TARGET=sieve.exe
!endif

!ifndef MODEL
MODEL=s
!endif

!ifeq MODEL f
CC=wcc386
SYSTEM=dos4g
!else
CC=wcc
SYSTEM=dos
!endif

CFLAGS=&
	-omaxnet&
	-bt=$(SYSTEM)&
	-m$(MODEL)&

OBJECTS=&
	sieve.obj&
	qcvm.obj&

.c.obj:
	$(CC) $(CFLAGS) -fo=$*.obj $*.c

$(TARGET): $(OBJECTS) wlink.rsp
	wlink @wlink.rsp

wlink.rsp: watcom.mk
	echo name $(TARGET) >wlink.rsp
	echo system $(SYSTEM) >>wlink.rsp
	for %i in ($(OBJECTS)) do echo file %i >>wlink.rsp
