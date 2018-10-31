#--------------------------------------------------- 
# Target file to be compiled by default
#---------------------------------------------------
# gcc -Wall -o exe src/rts_daemon.c lib/rts_access.c lib/rts_scheduler.c
# gcc -Wall -o exe src/rts_daemon.c lib/rts_channel.c lib/rts_scheduler.c components/usocket.c lib/rts_utils.c lib/rts_taskset.c lib/rts_task.c components/list_ptr.c
# gcc -Wall -o exe main.c lib/rts_channel.c lib/rts_scheduler.c components/usocket.c lib/rts_utils.c lib/rts_taskset.c lib/rts_task.c components/list_ptr.c

DAEMON = rts_daemon
#---------------------------------------------------
# CC will be the compiler to use
#---------------------------------------------------
CC = gcc
#---------------------------------------------------
# CFLAGS will be the options passed to the compiler
#---------------------------------------------------
CFLAGS = -Wall -lpthread -lrt -pedantic -std=c99
#---------------------------------------------------
# LDFLAGS will be the modules loaded
#---------------------------------------------------
LDFLAGS = -pthread
#---------------------------------------------------
# PATH_xxx contains the pathname of xxx files
#---------------------------------------------------
PATH_COMPONENTS = components
PATH_LIB = lib 
PATH_SRC = src
#--------------------------------------------------- 
# Dependencies 
#---------------------------------------------------
$(DAEMON): $(DAEMON).o ptask.o physics.o userpanel.o udp.o
	$(CC) $(CFLAGS) -o $(DAEMON) $(DAEMON).o ptask.o physics.o userpanel.o udp.o $(LDFLAGS)
	
$(DAEMON).o: $(DAEMON).c 
	$(CC) -c $(DAEMON).c

ptask.o: ptask.c
	$(CC) -c ptask.c
	
physics.o: physics.c
	$(CC) -c physics.c

userpanel.o: userpanel.c
	$(CC) -c userpanel.c
	
udp.o: udp.c
	$(CC) -c udp.c