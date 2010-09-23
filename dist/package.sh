#!/bin/sh
make -f dist/Makefile.linux clean package
make -f dist/Makefile.linux32 clean package
make -f dist/Makefile.windows32 clean package
[ -d $1 ] && cp target/*.tar.gz target/*.zip $1
