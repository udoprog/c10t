CC=g++
XCFLAGS=
#CFLAGS=-g $(XCFLAGS)
CFLAGS=-Wall -O3 $(XCFLAGS)
LDFLAGS=-lpng -lz -lboost_thread -lpthread

OUT=src/c10t

DEPLOY=.

ARCH=x86_64
VERSION=SNAPSHOT
DIST_DYNAMIC=c10t-dynamic-$(ARCH)-$(VERSION)
DIST_STATIC=c10t-static-$(ARCH)-$(VERSION)

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
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT) $(LDFLAGS)

$(OUT)-static: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)-static -static $(LDFLAGS) -lz

nbt:
	cd src/nbt && make

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	$(RM) $(OBJS)
	$(RM) $(OUT)
	$(RM) $(OUT)-static
	$(RM) -R $(DIST_DYNAMIC)
	$(RM) -R $(DIST_STATIC)
	$(RM) *.tar.gz

dist: all
	mkdir -p $(DIST_DYNAMIC)
	mkdir -p $(DIST_STATIC)
	cp $(OUT) $(DIST_DYNAMIC)/c10t
	cp $(OUT)-static $(DIST_STATIC)/c10t
	tar -cvf $(DIST_STATIC).tar $(DIST_STATIC)
	tar -cvf $(DIST_DYNAMIC).tar $(DIST_DYNAMIC)
	gzip $(DIST_STATIC).tar
	gzip $(DIST_DYNAMIC).tar

deploy: dist
	scp *.tar.gz $(DEPLOY)
