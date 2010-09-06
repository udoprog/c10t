CC=g++
CFLAGS=-g -Wall -I./corona/usr/include/
LDFLAGS=-lpng

SOURCES=src/main.cpp
SOURCES+=src/Level.cpp
SOURCES+=src/IMG.cpp
OBJS=$(SOURCES:.cpp=.o)

.SUFFIXES: .cpp .o

cart5: $(OBJS) 
	$(CC) $(OBJS) -o cart5 $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	$(RM) $(OBJS)
