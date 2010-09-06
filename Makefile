CC=g++
CFLAGS=-g -Wall
LFLAGS=-lcorona

SOURCES=src/main.cpp
SOURCES+=src/Level.cpp
OBJS=$(SOURCES:.cpp=.o)

.SUFFIXES: .c .o

cart5: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o cart5

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	$(RM) $(OBJS)
