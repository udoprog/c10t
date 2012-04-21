#!/bin/bash

if [ $# != 1 ] ; then
	echo "usage : $0 path_to_minecraft_world"
	exit 0
fi

OPTIONS=""
WORLD="$1"
SPEC_FILES_PATH="."

PWD=../build/ make

for x in {1..7} ; do
	spec_file="${SPEC_FILES_PATH}/testcase_$x.txt"
	echo ">> generating iso renderer tests for file ... $spec_file"
	echo ""
	../build/c10t -w map196/ -J testcase_7.txt -o test_iso_7.png -z $OPTIONS; 
done

for x in {1..7} ; do
	spec_file="${SPEC_FILES_PATH}/testcase_$x.txt"
	echo ">> generating flat renderer tests for file ... $spec_file"
	echo "" 
	../build/c10t -w map196/ -J testcase_$x.txt -o test$x.png $OPTIONS; 
done

rm swap.bin
