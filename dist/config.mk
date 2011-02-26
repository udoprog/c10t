#SOURCES+=src/algorithm.cpp
#SOURCES+=src/text.cpp
#SOURCES+=src/players.cpp
#SOURCES+=src/fileutils.cpp
#SOURCES+=src/nbt/nbt.cpp
#SOURCES+=src/main.cpp
#SOURCES+=src/main_utils.cpp
#SOURCES+=src/utf8.cpp
#SOURCES+=src/warps.cpp
#SOURCES+=src/mc/region.cpp
#SOURCES+=src/mc/utils.cpp
#SOURCES+=src/mc/level.cpp
#SOURCES+=src/mc/blocks.cpp
#SOURCES+=src/mc/world.cpp
#SOURCES+=src/engine/isometric_engine.cpp
#SOURCES+=src/engine/oblique_engine.cpp
#SOURCES+=src/engine/obliqueangle_engine.cpp
#SOURCES+=src/engine/topdown_engine.cpp
#SOURCES+=src/image/cached_image.cpp
#SOURCES+=src/image/color.cpp
#SOURCES+=src/image/image_base.cpp
#SOURCES+=src/image/image_operations.cpp
#SOURCES+=src/image/memory_image.cpp
SOURCES+=src/mc/region.cpp
SOURCES+=src/mc/region_inspect.cpp

OBJECTS=${SOURCES:.cpp=.o}
CXXFLAGS+=-Isrc -I${USR}/include/freetype2 -Wall -fomit-frame-pointer -O2

PACKAGE=${DIST}-${VERSION}
BUILD=./build

all: ${TARGET}

${TARGET}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} ${LDFLAGS} -o ${TARGET}

clean:
	${RM} ${OBJECTS}
	${RM} -rf ${PACKAGE}

package: ${TARGET} ${PACKAGE} local-package
	${RM} -rf ${PACKAGE}

${PACKAGE}:
	mkdir ${PACKAGE}
	cp ${TARGET} ${PACKAGE}/${TARGET}
	mkdir -p ${BUILD}
