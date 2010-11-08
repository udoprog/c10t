SOURCES+=src/level.cpp
SOURCES+=src/color.cpp
SOURCES+=src/blocks.cpp
SOURCES+=src/world.cpp
SOURCES+=src/text.cpp
SOURCES+=src/players.cpp
SOURCES+=src/fileutils.cpp
SOURCES+=src/image.cpp
SOURCES+=src/common.cpp
SOURCES+=src/nbt/nbt.cpp
SOURCES+=src/main.cpp
SOURCES+=src/utf8.cpp
SOURCES+=src/warps.cpp

OBJECTS=${SOURCES:.cpp=.o}
CXXFLAGS+=-I${USR}/include/freetype2 -Wall -fomit-frame-pointer -O2

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
