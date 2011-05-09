#!/bin/bash

target=$1

shift

dist_target=dist/targets/$target
build_target=build/$target
dist_config=dist/config.mk
dist_src=src
dist_scripts=scripts
dist_res=res

exit_usage() {
  echo "Usage: dist/dist.sh <target> [make-opts]"
  echo "<target> is one of:"
  ls -1 dist/targets
  exit 1
}

do_cp() {
  if test $1 -nt $2; then
    printf "cp %-37s -> %s\n" "$1" "$2"
    cp $1 $2
  fi

  return 0
}

do_sync() {
  src=$1
  dir=$2

  shift 2

  echo "Syncing $src to $dir/$src"

  while read file; do
    target="$dir/$file"
    mkdir -p $(dirname "$target")

    if test $file -nt $target; then
      printf "%-40s -> %s\n" "$file" "$target"
      cp -p "$file" "$target"
    fi
  done < <(find $src -type f $@)
}

[[ ! -d dist/targets ]] && echo "Not in build directory" && exit 1
[[ -z $target ]] && exit_usage
[[ ! -f $dist_target ]] && exit_usage

if ! cmake .; then
  echo "cmake: failed"
  exit 1
fi

mkdir -p $build_target
do_cp $dist_target $build_target/Makefile
do_cp $dist_config $build_target/config.mk
do_cp LICENSE.txt $build_target/LICENSE.txt
do_cp README.md $build_target/README.md
do_sync $dist_src $build_target -name "*.cpp" -o -name "*.hpp"
do_sync $dist_scripts $build_target
do_sync $dist_res $build_target

echo "cd $build_target && make $@"
cd $build_target && make "$@"
exit $?
