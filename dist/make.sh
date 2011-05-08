#!/bin/bash

target=$1

shift

dist_target=dist/targets/$target
build_target=build/$target
dist_config=dist/config.mk
dist_src=src
dist_scripts=src

exit_usage() {
  echo "Usage: dist/dist.sh <target> [make-opts]"
  echo "<target> is one of:"
  ls -1 dist/targets
  exit 1
}

do_cp() {
  echo "copying $1 -> $2"
  cp $1 $2
  return 0
}

do_sync() {
  echo "syncing $1 -> $2"
  rsync -rav $1 $2
  return $?
}

[[ ! -d dist/targets ]] && echo "Not in build directory" && exit 1
[[ -z $target ]] && exit_usage
[[ ! -f $dist_target ]] && exit_usage

mkdir -p $build_target
do_cp $dist_target $build_target/Makefile
do_cp $dist_config $build_target/config.mk
do_sync $dist_src $build_target/
do_sync $dist_scripts $build_target/

cd $build_target && make "$@"
