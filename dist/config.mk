USR?=/usr/${TARGET}/usr
LIB?=${USR}/lib

SOURCES+=src/algorithm.cpp
SOURCES+=src/altitude_graph.cpp
SOURCES+=src/dirlist.cpp
SOURCES+=src/dlopen.cpp
SOURCES+=src/engine/block_rotation.cpp
SOURCES+=src/engine/fatiso_engine.cpp
SOURCES+=src/engine/functions.cpp
SOURCES+=src/engine/isometric_engine.cpp
SOURCES+=src/engine/oblique_engine.cpp
SOURCES+=src/engine/obliqueangle_engine.cpp
SOURCES+=src/engine/topdown_engine.cpp
SOURCES+=src/fileutils.cpp
SOURCES+=src/image/algorithms.cpp
SOURCES+=src/image/cached_image.cpp
SOURCES+=src/image/color.cpp
SOURCES+=src/image/image_base.cpp
SOURCES+=src/image/image_operations.cpp
SOURCES+=src/image/memory_image.cpp
SOURCES+=src/json.cpp
SOURCES+=src/main.cpp
SOURCES+=src/main_utils.cpp
SOURCES+=src/mc/blocks.cpp
SOURCES+=src/mc/dynamic_buffer.cpp
SOURCES+=src/mc/level.cpp
SOURCES+=src/mc/level_info.cpp
SOURCES+=src/mc/marker.cpp
SOURCES+=src/mc/region.cpp
SOURCES+=src/mc/region_iterator.cpp
SOURCES+=src/mc/rotated_level_info.cpp
SOURCES+=src/mc/utils.cpp
SOURCES+=src/mc/world.cpp
SOURCES+=src/nbt/nbt.cpp
SOURCES+=src/players.cpp
SOURCES+=src/text.cpp
SOURCES+=src/warps.cpp

LDFLAGS+=${LIB}/libpng.a
LDFLAGS+=${LIB}/libboost_thread.a
LDFLAGS+=${LIB}/libboost_system.a
LDFLAGS+=${LIB}/libboost_filesystem.a
LDFLAGS+=${LIB}/libfreetype.a
LDFLAGS+=${LIB}/libz.a
LDFLAGS+=${LIB}/libbz2.a
LDFLAGS+=${LIB}/libdl.a
LDFLAGS+=${LIB}/libunc.a
LDFLAGS+=${LIB}/libuncdata.a

OBJECTS=${SOURCES:.cpp=.o}
CXXFLAGS+=-Isrc -I${USR}/include/freetype2 -Wall -fomit-frame-pointer -O2

CXX=${TARGET}-g++
STRIP=${TARGET}-strip

VERSION?=SNAPSHOT
PACKAGE=c10t-${VERSION}-${OS}-${ARCH}

BUILD=./build

all: package

.SUFFIXES: .cpp .o

.cpp.o:
	${CXX} ${CXXFLAGS} -c $< -o $@

${BIN}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} ${LDFLAGS} -o ${BIN}
	${STRIP} ${BIN}

clean:
	${RM} ${OBJECTS}
	${RM} -rf ${PACKAGE}

pre-package: ${BIN}
	echo "pre-package: ${PACKAGE}"
	mkdir -p ${PACKAGE}
	cp ${BIN} ${PACKAGE}/${BIN}
	rm -rf ${BUILD}
	mkdir -p ${BUILD}

post-package:
	echo "post-package: ${PACKAGE}"
	${RM} -rf ${PACKAGE}

package: pre-package local-package post-package

%.sha1:
	sha1sum $* > $*.sha1
	cp $@ ${BUILD}/$@

%.tar.gz:
	tar -cvf $*.tar $*
	gzip -f $*.tar
	cp $@ ${BUILD}/$@

%.zip:
	zip -r $*.zip $*
	cp $@ ${BUILD}/$@
