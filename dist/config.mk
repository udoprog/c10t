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

EXTRA=README.md

OBJECTS=${SOURCES:.cpp=.o}
CXXFLAGS+=-I${USR}/include/freetype2 -Wall -fomit-frame-pointer -O3

PACKAGE=${DIST}-${VERSION}
BUILD=./build/

all: ${TARGET}

${TARGET}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} ${LDFLAGS} -o ${TARGET}

clean:
	${RM} ${OBJECTS}
	${RM} -R ${PACKAGE}

package: ${TARGET} ${PACKAGE} local-package
	${RM} -R ${PACKAGE}

${PACKAGE}:
	mkdir -p ${PACKAGE}
	cp ${TARGET} ${PACKAGE}/
	cp ${EXTRA} ${PACKAGE}/
	mkdir -p ${BUILD}
