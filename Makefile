all: dist-linux-x86_64 dist-linux-x86
	echo "All done"

dist-linux-x86:
	make -Bf dist/Makefile.linux CC="i686-pc-linux-gnu-g++" ARCH="x86" dist

dist-linux-x86_64:
	make -Bf dist/Makefile.linux ARCH="x86_64" dist 
