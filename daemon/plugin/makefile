CC = gcc
CFLAGS= -Wall -O -fPIC -Wno-unused-result -std=gnu99
DEBUG = -g -O0

LIB_PATH = ../lib
CMP_PATH = ../components

.PHONY: all clean

all: sched_SSRM.so sched_RR.so sched_FP.so sched_EDF.so

sched_SSRM.so: $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_int.o $(LIB_PATH)/rts_utils.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_SSRM.o
	$(CC) $(DEBUG) $(CFLAGS) -shared $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_int.o $(LIB_PATH)/rts_utils.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_SSRM.o -o $@

sched_RR.so: $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_utils.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_RR.o
	$(CC) $(DEBUG) $(CFLAGS) -shared $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_utils.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_RR.o -o $@

sched_FP.so: $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_utils.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_FP.o
	$(CC) $(DEBUG) $(CFLAGS) -shared $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_utils.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_FP.o -o $@
	
sched_EDF.so: $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_utils.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_EDF.o
	$(CC) $(DEBUG) $(CFLAGS) -shared $(CMP_PATH)/shatomic.o $(CMP_PATH)/list_ptr.o $(LIB_PATH)/rts_utils.o $(LIB_PATH)/rts_taskset.o $(LIB_PATH)/rts_task.o $(LIB_PATH)/rts_plugin.o sched_EDF.o -o $@

sched_SSRM.o : sched_SSRM.c
	$(CC) -c sched_SSRM.c $(DEBUG) $(CFLAGS) -lm -o sched_SSRM.o

sched_RR.o : sched_RR.c
	$(CC) -c sched_RR.c $(DEBUG) $(CFLAGS) -o sched_RR.o

sched_FP.o : sched_FP.c
	$(CC) -c sched_FP.c $(DEBUG) $(CFLAGS) -o sched_FP.o
	
sched_EDF.o : sched_EDF.c
	$(CC) -c sched_EDF.c $(DEBUG) $(CFLAGS) -o sched_EDF.o

$(LIB_PATH)/rts_taskset.o: $(LIB_PATH)/rts_taskset.c
	$(CC) -c $(LIB_PATH)/rts_taskset.c $(DEBUG) $(CFLAGS) -o $(LIB_PATH)/rts_taskset.o
	
$(LIB_PATH)/rts_task.o: $(LIB_PATH)/rts_task.c
	$(CC) -c $(LIB_PATH)/rts_task.c $(DEBUG) $(CFLAGS) -o $(LIB_PATH)/rts_task.o

$(LIB_PATH)/rts_plugin.o: $(LIB_PATH)/rts_plugin.c
	$(CC) -c $(LIB_PATH)/rts_plugin.c $(DEBUG) $(CFLAGS) -o $(LIB_PATH)/rts_plugin.o
	
$(LIB_PATH)/rts_utils.o: $(LIB_PATH)/rts_utils.c
	$(CC) -c $(LIB_PATH)/rts_utils.c $(DEBUG) $(CFLAGS) -o $(LIB_PATH)/rts_utils.o

$(CMP_PATH)/list_ptr.o: $(CMP_PATH)/list_ptr.c
	$(CC) -c $(CMP_PATH)/list_ptr.c $(DEBUG) $(CFLAGS) -o $(CMP_PATH)/list_ptr.o
	
$(CMP_PATH)/list_int.o: $(CMP_PATH)/list_int.c
	$(CC) -c $(CMP_PATH)/list_int.c $(DEBUG) $(CFLAGS) -o $(CMP_PATH)/list_int.o
	
$(CMP_PATH)/shatomic.o: $(CMP_PATH)/shatomic.c
	$(CC) -c $(CMP_PATH)/shatomic.c $(DEBUG) $(CFLAGS) -o $(CMP_PATH)/shatomic.o

clean:
	@rm -rf $(LIB_PATH)/rts_task.o \
		$(LIB_PATH)/rts_taskset.o \
		$(LIB_PATH)/rts_plugin.o \
		$(LIB_PATH)/rts_utils.o \
		$(CMP_PATH)/list_ptr.o \
		$(CMP_PATH)/list_int.o \
		$(CMP_PATH)/shatomic.o \
		sched_SSRM.o \
		sched_RR.o \
		sched_FP.o \
		sched_EDF.o \
		sched_SSRM.so \
		sched_RR.so \
		sched_FP.so \
		sched_EDF.so