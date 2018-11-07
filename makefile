## -*- Makefile -*-
##
## User: gabrieleserra
## Time: Oct 31, 2018 3:19:39 PM
## Makefile created by Oracle Developer Studio.
##
## This file is generated automatically.
##


#### Compiler and tool definitions shared by all build targets #####
CC = gcc
BASICOPTS = -g
CFLAGS = $(BASICOPTS)


# Define the target directories.
TARGETDIR_rtsd=GNU-x86_64-Linux


all: $(TARGETDIR_rtsd)/rtsd

## Target: rtsd
CPPFLAGS_rtsd = \
	-Icomponents/pipe.c \
	-Icomponents/shm.c \
	-Icomponents/list_int.c \
	-Icomponents/fifo.c \
	-Icomponents/usocket.c \
	-Icomponents/list_ptr.c \
	-Ilib/rts_scheduler.c \
	-Ilib/rts_channel.c \
	-Ilib/rts_utils.c \
	-Ilib/rts_taskset.c \
	-Ilib/rts_task.c
OBJS_rtsd =  \
	$(TARGETDIR_rtsd)/rts_channel.o \
	$(TARGETDIR_rtsd)/rts_scheduler.o \
	$(TARGETDIR_rtsd)/rts_taskset.o \
	$(TARGETDIR_rtsd)/rts_utils.o \
	$(TARGETDIR_rtsd)/list_int.o \
	$(TARGETDIR_rtsd)/list_ptr.o \
	$(TARGETDIR_rtsd)/shm.o \
	$(TARGETDIR_rtsd)/usocket.o
USERLIBS_rtsd = $(SYSLIBS_rtsd) 
DEPLIBS_rtsd =  
LDLIBS_rtsd = $(USERLIBS_rtsd)


# Link or archive
$(TARGETDIR_rtsd)/rtsd: $(TARGETDIR_rtsd) $(OBJS_rtsd) $(DEPLIBS_rtsd)
	$(LINK.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ $(OBJS_rtsd) $(LDLIBS_rtsd)


# Compile source files into .o files
$(TARGETDIR_rtsd)/rts_channel.o: $(TARGETDIR_rtsd) lib/rts_channel.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ lib/rts_channel.c

$(TARGETDIR_rtsd)/rts_scheduler.o: $(TARGETDIR_rtsd) lib/rts_scheduler.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ lib/rts_scheduler.c

$(TARGETDIR_rtsd)/rts_taskset.o: $(TARGETDIR_rtsd) lib/rts_taskset.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ lib/rts_taskset.c

$(TARGETDIR_rtsd)/rts_utils.o: $(TARGETDIR_rtsd) lib/rts_utils.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ lib/rts_utils.c

$(TARGETDIR_rtsd)/list_int.o: $(TARGETDIR_rtsd) components/list_int.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ components/list_int.c

$(TARGETDIR_rtsd)/list_ptr.o: $(TARGETDIR_rtsd) components/list_ptr.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ components/list_ptr.c

$(TARGETDIR_rtsd)/shm.o: $(TARGETDIR_rtsd) components/shm.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ components/shm.c

$(TARGETDIR_rtsd)/usocket.o: $(TARGETDIR_rtsd) components/usocket.c
	$(COMPILE.c) $(CFLAGS_rtsd) $(CPPFLAGS_rtsd) -o $@ components/usocket.c



#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_rtsd)/rtsd \
		$(TARGETDIR_rtsd)/rts_channel.o \
		$(TARGETDIR_rtsd)/rts_scheduler.o \
		$(TARGETDIR_rtsd)/rts_taskset.o \
		$(TARGETDIR_rtsd)/rts_utils.o \
		$(TARGETDIR_rtsd)/list_int.o \
		$(TARGETDIR_rtsd)/list_ptr.o \
		$(TARGETDIR_rtsd)/shm.o \
		$(TARGETDIR_rtsd)/usocket.o
	rm -f -r $(TARGETDIR_rtsd)


# Create the target directory (if needed)
$(TARGETDIR_rtsd):
	mkdir -p $(TARGETDIR_rtsd)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-x86_64-Linux

