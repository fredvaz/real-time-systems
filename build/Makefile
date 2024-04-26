

# Configuration

# GCC for c and g++ for G++
CC = gcc
# Compilation flags
CFLAGS =  -g -Wall -O0 -D_REENTRANT
# Linker flags
LFLAGS = -lpthread -pthread -lrt
# Lib files directories
LIBS = ../lib
# Include files directory
INCLUDES = ../include
# Source files directory 
SOURCES = ../src


lib:
	$(CC) -c $(SOURCES)/func.c -o $(LIBS)/func.o

execu_time:
	$(CC) $(CFLAGS) $(SOURCES)/execu_time.c -I$(INCLUDES) $(LFLAGS) $(LIBS)/func.o -o execu_time.o

example2:
	$(CC) $(CFLAGS) $(SOURCES)/example2.c -I$(INCLUDES) $(LFLAGS) $(LIBS)/func.o -o example2.o

# Clean up all executable files
clean:
	rm -f *.o
