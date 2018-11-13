#--------------------------------------------------- 
# Target file
#---------------------------------------------------
TEST = test

#---------------------------------------------------
# Compiler
#---------------------------------------------------
CC = gcc

#--------------------------------------------------- 
# Project paths
#---------------------------------------------------
LIB_PATH =  ../lib
PRV_PATH =  ../daemon/lib
CMP_PATH =  ../daemon/components

#---------------------------------------------------
# Options passed to the compiler
#---------------------------------------------------
CFLAGS = -Wall -std=gnu99 -I$(LIB_PATH)

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

UTILS_CONF = confutils.o
UTILS_MEM = memutils.o
UTILS_TIME = timeutils.o

UTILS_O = $(UTILS_CONF) $(UTILS_MEM) $(UTILS_TIME)
		
#--------------------------------------------------- 
# Compile and create objects
#---------------------------------------------------

all: $(TEST)

$(TEST): $(CMP_PATH)/usocket.o $(CMP_PATH)/shatomic.o $(PRV_PATH)/rts_channel.o $(LIB_PATH)/rts_lib.o $(UTILS_O) $(TEST).o  
	$(CC) -o $(TEST) $(CFLAGS) $(CMP_PATH)/usocket.o $(CMP_PATH)/shatomic.o $(PRV_PATH)/rts_channel.o $(LIB_PATH)/rts_lib.o $(UTILS_O) $(TEST).o  $(LDFLAGS)

$(LIB_PATH)/rts_lib.o:  $(LIB_PATH)/rts_lib.c
	$(CC) -c $(CFLAGS) $(LIB_PATH)/rts_lib.c -o $(LIB_PATH)/rts_lib.o

$(PRV_PATH)/rts_channel.o :
	$(CC) -c $(CFLAGS) $(PRV_PATH)/rts_channel.c -o $(PRV_PATH)/rts_channel.o
	
$(CMP_PATH)/shatomic.o :
	$(CC) -c $(CFLAGS) $(CMP_PATH)/shatomic.c -o $(CMP_PATH)/shatomic.o
	
$(CMP_PATH)/usocket.o :
	$(CC) -c $(CFLAGS) $(CMP_PATH)/usocket.c -o $(CMP_PATH)/usocket.o

confutils.o: confutils.c 
	$(CC) -c confutils.c
	
memutils.o: memutils.c
	$(CC) -c memutils.c
	
timeutils.o: timeutils.c
	$(CC) -lrt -c timeutils.c
		
$(TEST).o: $(TEST).c 
	$(CC) -c $(CFLAGS) $(TEST).c
	
clean:
	@rm -rf $(TEST).o $(UTILS_O) $(CMP_PATH)/usocket.o $(CMP_PATH)/shatomic.o $(PRV_PATH)/rts_channel.o $(LIB_PATH)/rts_lib.o 
	


