#!/bin/sh
set -e

dists="x86-linux x86_64-linux x86-windows x86_64-windows"

for dist in $dists; do
  dist/make.sh $dist

  (cd build/$dist && {
    ls -1
  })
done
