#!/bin/sh

for dist in $(ls -1 dist/targets); do
  if ! dist/make.sh $dist "$@"; then
    echo "TARGET FAILED: $dist"
    exit 1
  fi
done
