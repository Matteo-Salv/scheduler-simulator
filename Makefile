PROJECT := simulator
CC = gcc
CFLAGS = -Wall -pedantic
DEBUG = -ggdb3 -D DEBUG
LDFLAGS = -pthread

HEADERS := $(wildcard *.h)
CFILES := $(wildcard *.c)
OBJECTS := $(wildcard *.o)


all: libsched libfunct $(PROJECT)

libsched: scheduler.c Header.h
	$(CC) -c $(CFLAGS) $<

libfunct: Functions.c Header.h
	$(CC) -c $(CFLAGS) $<

$(PROJECT): main.c
	$(CC) -c $(CFLAGS) $<
	$(CC) $(CFLAGS) $(wildcard *.o) -o $(PROJECT) $(LDFLAGS)
	
clean:
	@rm $(PROJECT)
	@rm $(OBJECTS)
