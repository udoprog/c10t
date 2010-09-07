CC=g++
CFLAGS=-g -Wall
LDFLAGS=-lpng

OUT=cart5

SOURCES=src/main.cpp
SOURCES+=src/Level.cpp
SOURCES+=src/Image.cpp
OBJS=$(SOURCES:.cpp=.o)

.SUFFIXES: .cpp .o

cart5: $(OBJS) 
	$(CC) $(OBJS) -o $(OUT) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	$(RM) $(OBJS)
	$(RM) $(OUT)
