CFLAGS = -g -I. -Wall
CC = gcc


objects = test.o thread.o msgb.o queue.o fp_entity.o

fp_test: $(objects)
	$(CC) $(CFLAGS) -o fp_test $(objects) -losmocore -lpthread

test.o: test.h
thread.o: thread.h
msgb.o: msgb.h
queue.o: queue.h
fp_entity.o: fp_entity.h

.PHONY: clean
clean:
	rm fp_test $(objects)
