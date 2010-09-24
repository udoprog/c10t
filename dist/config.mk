SOURCES+=src/Level.cpp
SOURCES+=src/Color.cpp
SOURCES+=src/blocks.cpp
SOURCES+=src/fileutils.cpp
SOURCES+=src/Image.cpp
SOURCES+=src/nbt/nbt.cpp
SOURCES+=src/main.cpp

EXTRA=README.md

OBJECTS=${SOURCES:.cpp=.o}

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
