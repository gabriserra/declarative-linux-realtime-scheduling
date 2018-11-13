CC = gcc
CFLAGS= -Wall -O -fPIC

.PHONY: all clean

all: SSRM.so

SSRM.so: list_ptr.o rts_taskset.o rts_task.o rts_plugin.o 
	$(CC) $(CFLAGS) -shared $< -o $@

rts_taskset.o: rts_taskset.c rts_taskset.h
	$(CC) -c rts_taskset.c rts_tasks.c list_ptr.c

rts_task.o: rts_task.c rts_task.h
	$(CC) -c rts_task.c

rts_plugin.o: rts_plugin.c rts_plugin.h
	$(CC) -c rts_plugin.c

list_ptr.o: list_ptr.c list_ptr.h
	$(CC) -c list_ptr.c

