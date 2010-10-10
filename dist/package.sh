#!/bin/sh

version=$2

set -e
make -f dist/Makefile.linux clean package VERSION="$version"
make -f dist/Makefile.linux32 clean package VERSION="$version"
make -f dist/Makefile.windows clean package VERSION="$version"
make -f dist/Makefile.windows32 clean package VERSION="$version"
[ -d $1 ] && cp build/*.tar.gz build/*.zip $1
