CC=g++
CFLAGS=-g -Wall
LDFLAGS=-lpng

OUT=src/c10t

SOURCES=src/main.cpp
SOURCES+=src/Level.cpp
SOURCES+=src/Image.cpp
SOURCES+=src/Color.cpp
SOURCES+=src/blocks.cpp
SOURCES+=src/nbt/nbt.cpp
OBJS=$(SOURCES:.cpp=.o)

.SUFFIXES: .cpp .o

all: $(OUT) $(OUT)-static

$(OUT): $(OBJS) 
	$(CC) $(OBJS) -o $(OUT) $(LDFLAGS)

$(OUT)-static: $(OBJS) 
	$(CC) $(OBJS) -o $(OUT)-static -static $(LDFLAGS) -lz

nbt:
	cd src/nbt && make

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	$(RM) $(OBJS)
	$(RM) $(OUT)
	$(RM) $(OUT)-static
