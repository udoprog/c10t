VERSION=1.0
target=c10t-${VERSION}

dist:
	mkdir -p ${target}
	g++ src/*.cpp src/nbt/*.cpp -static -lpng -lz -lboost_thread -lpthread -o ${target}/c10t
	cp README.md ${target}/README
	tar -cvf ${target}.tar ${target}
	gzip ${target}.tar
