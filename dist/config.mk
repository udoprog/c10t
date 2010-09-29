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

EXTRA=README.md

OBJECTS=${SOURCES:.cpp=.o}
CXXFLAGS+=-I${USR}/include/freetype2 -Wall

PACKAGE=${DIST}-${VERSION}
BUILD=./build/

all: ${TARGET} ${TARGET_DEBUG}

${TARGET}: ${OBJECTS}
	${CXX} ${CXXFLAGS} -O3 ${OBJECTS} ${LDFLAGS} -o ${TARGET}

${TARGET_DEBUG}: ${OBJECTS_DEBUG}
	${CXX} ${CXXFLAGS} -g ${OBJECTS_DEBUG} ${LDFLAGS} -o ${TARGET}

clean:
	${RM} ${OBJECTS}
	${RM} -R ${PACKAGE}

package: ${TARGET} ${PACKAGE} local-package
	${RM} -R ${PACKAGE}

${PACKAGE}:
	mkdir -p ${PACKAGE}
	cp ${TARGET} ${PACKAGE}/
	cp ${TARGET_DEBUG} ${PACKAGE}/
	cp ${EXTRA} ${PACKAGE}/
	mkdir -p ${BUILD}
