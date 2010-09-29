#!/bin/sh
make -f dist/Makefile.linux clean package
make -f dist/Makefile.linux32 clean package
make -f dist/Makefile.windows clean package
make -f dist/Makefile.windows32 clean package
[ -d $1 ] && cp build/*.tar.gz build/*.zip $1
