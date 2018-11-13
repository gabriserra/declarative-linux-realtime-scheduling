#--------------------------------------------------- 
# Target file
#---------------------------------------------------
DAEMON = daemon

#---------------------------------------------------
# Compiler
#---------------------------------------------------
CC = gcc

#--------------------------------------------------- 
# Project paths
#---------------------------------------------------
LIB_PATH =  lib
CMP_PATH =  components
BIN_PATH =  bin

#---------------------------------------------------
# Options passed to the compiler
#---------------------------------------------------
CFLAGS = -Wall -lpthread -std=c99 #-lrt 

#---------------------------------------------------
# Load library path
#---------------------------------------------------
CPPFLAGS = -I$(LIB_PATH) -I$(CMP_PATH)

#---------------------------------------------------
# Modules loaded
#---------------------------------------------------
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -pthread

#--------------------------------------------------- 
# DEBUG behavior 
#---------------------------------------------------
ifdef DEBUG
    CFLAGS += -g -O0
else
    CFLAGS += -O3
endif

#--------------------------------------------------- 
# Dependencies
#---------------------------------------------------

CMP_ATO = $(CMP_PATH)/atomic
CMP_LSI = $(CMP_PATH)/list_int
CMP_LSP = $(CMP_PATH)/list_ptr
CMP_SHM = $(CMP_PATH)/shatomic
CMP_USK = $(CMP_PATH)/usocket

CMPS =	$(CMP_LSI) $(CMP_LSP) \
	$(CMP_SHM) $(CMP_USK)

CMPS_C = $(foreach CMP, $(CMPS), $(CMP).c)
CMPS_O = ${CMPS_C:.c=.o}

LIB_CHN = $(LIB_PATH)/rts_channel
LIB_DAE = $(LIB_PATH)/rts_daemon
LIB_PLG = $(LIB_PATH)/rts_plugin
LIB_SCH = $(LIB_PATH)/rts_scheduler
LIB_TSK = $(LIB_PATH)/rts_task
LIB_TSS = $(LIB_PATH)/rts_taskset
LIB_TYP = $(LIB_PATH)/rts_types
LIB_UTS = $(LIB_PATH)/rts_utils

LIBS =	$(LIB_CHN) $(LIB_DAE) $(LIB_PLG) $(LIB_SCH) \
	$(LIB_TSK) $(LIB_TSS) $(LIB_UTS)
	
LIBS_C = $(foreach LIB, $(LIBS), $(LIB).c)
LIBS_O = ${LIBS_C:.c=.o}
		
#--------------------------------------------------- 
# Compile and create objects
#---------------------------------------------------
print-%  : ; @echo $* = $($*)

all: $(DAEMON)

$(DAEMON): $(DAEMON).o $(CMPS_O) $(LIBS_O) 
	$(CC) -o $(DAEMON) $(DAEMON).o $(CMPS_O) $(LIBS_O) $(LDFLAGS)
	
$(DAEMON).o: $(DAEMON).c 
	$(CC) -c $(DAEMON).c
	
$(CMPS_O): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(LIBS_O): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	@rm -rf $(DAEMON).o $(CMPS_O) $(LIBS_O)
	

