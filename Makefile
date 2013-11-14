CFLAGS = -g -I. -Wall -pthread
CC = gcc
LDFLAGS = -losmocore 

objects = test.o thread.o msgb.o queue.o fp_entity.o threadpool.o
TARGETS = fp_test


fp_test: $(objects)
	$(CC) $(CFLAGS) -o fp_test $(objects) $(LDFLAGS)

# test.o: test.h
# thread.o: thread.h
# msgb.o: msgb.h
# queue.o: queue.h
# fp_entity.o: fp_entity.h
# threadpool.o: threadpool.h

.PHONY: clean
clean:
	rm fp_test $(objects) -rf
