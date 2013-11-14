CFLAGS = -g -I. -Wall -pthread
CC = gcc
LDFLAGS = -losmocore 

objects = test.o thread.o msgb.o queue.o fp_entity.o threadpool.o
TARGETS = fp_test

all: $(TARGETS)

fp_test: $(objects)
	$(CC) $(CFLAGS) -o fp_test $(objects) $(LDFLAGS)


.PHONY: clean
clean:
	rm fp_test $(objects) -rf
