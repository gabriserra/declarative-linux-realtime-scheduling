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

#---------------------------------------------------
# Options passed to the compiler
#---------------------------------------------------
CFLAGS = -Wall -std=c99 

#---------------------------------------------------
# Load library path
#---------------------------------------------------
CPPFLAGS = -I$(LIB_PATH)

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

$(TEST): $(LIB_PATH)/rts_lib.o $(UTILS_O) $(TEST).o  
	$(CC) -o $(TEST) $(TEST).o $(UTILS_O) $(LDFLAGS)

$(LIB_PATH)/rts_lib.o: $(LIB_PATH)/rts_lib.c
	$(CC) -c $(LIB_PATH)/rts_lib.c

confutils.o: confutils.c 
	$(CC) -c confutils.c
	
memutils.o: memutils.c
	$(CC) -c memutils.c
	
timeutils.o: timeutils.c
	$(CC) -lrt -c timeutils.c
		
$(TEST).o: $(TEST).c 
	$(CC) -c $(TEST).c
	
clean:
	@rm -rf $(TEST).o $(UTILS_O)
	


