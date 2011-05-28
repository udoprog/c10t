#!/bin/bash

C10T=c10t
[[ -x ./$C10T ]] && C10T=./$C10T

scriptdir=$(dirname $(readlink -f $0))

world=""
output=""
host=""
zoom=5
factor=4
base=256
res=$scriptdir
opts=""

exit_usage() {
  echo "Usage: google-api -w <world> -o <output> [options]"
  echo "-H <host>     - Host prefix to use for all scripts"
  echo "-z <levels>   - Number of zoom levels to generate (default: 5)"
  echo "-b <res>      - Resolution for the base tile (default: 256px)"
  echo "-r <dir>      - Directory to look for reasources (defaults to script directory)"
  echo "-O <opts>     - Extra options to pass directly to c10t"
  exit 1
}

while getopts "w:o:H:z:b:O:h" opt; do
  case $opt in
    w) world=$OPTARG ;;
    o) output=$OPTARG ;;
    H) host=$OPTARG ;;
    O) opts=$OPTARG ;;
    h) exit_usage ;;
    z)
      zoom=$OPTARG
      factor=$[$zoom - 1]
      ;;
    b) base=$OPTARG ;;
    \?) exit 1 ;;
  esac
done

shift $OPTIND

[ -z $world ] && exit_usage
[ -z $output ] && exit_usage

[ ! -d $world ] && {
  echo "$0: -w: directory does not exist: $world";
  exit_usage
}

[ ! -d $output ] && {
  echo "$0: -o: directory does not exist: $output";
  exit_usage
}

[ $zoom -lt 2 ] && {
  echo "$0: -z: must be a numberic value greater than 2, but was '$zoom'";
  exit_usage
}

[ $base -lt 32 ] && {
  echo "$0: -b: must be a numberic value greater than 32, but was '$base'";
  exit_usage
}

if ! $C10T -h &> /dev/null; then
  echo "Could not find executable: $C10T"
  exit_usage
fi

google_js=$res/libc10t.google.js
index_html=$res/index.html

[ ! -f $google_js ] && {
  echo "could not find: $C10T_GOOGLE_JS"
  exit 1;
}

[ ! -f $index_html ] && {
  echo "could not find: $C10T_GOOGLE_JS"
  exit 1;
}

host_google_js="${host}$(basename $google_js)"
host_options_js="${host}options.js"

create_tile_sizes() {
  b=$base
  i=$[$zoom - 1]

  echo -n $[$b / 2]

  while [[ $i -gt 0 ]]; do
    echo -n " $b"
    b=$[$b*2]
    i=$[$i - 1]
  done
}

splits=$(create_tile_sizes)
# this is what each tile will be resized to

tiles=$output/tiles
c10t_opts="$opts -w $world"

if [[ -z $world ]] || [[ ! -d $world ]]; then
  echo "Directory does not exist: $world";
  exit 1;
fi

[ ! -d $target ] && mkdir -p $target
[ ! -d $tiles ] && mkdir -p $tiles

generate() {
  x_opts=$1
  name=$2
  pixelsplit="$3"

  # generate a set of split files
  tile=$tiles/$name.%d.%d.%d.png

  echo "$C10T $c10t_opts $x_opts --split=\"$pixelsplit\" --split-base=$base -o $tile --write-json=$output/$name.json"
  if ! $C10T $c10t_opts $x_opts --split="$pixelsplit" --split-base=$base -o $tile --write-json="$output/$name.json"; then
    exit 1
  fi

  echo "done!"
}

echo "Copying $index_html"
cat $index_html | \
  sed -r "s/\bC10T_GOOGLE_JS\b/$host_google_js/" | \
  sed "s/\bC10T_OPTIONS_JS\b/$host_options_js/" > $output/index.html

echo "Copying $google_js"
cp $google_js $output

generate "" "day" "$splits"
generate "-n" "night" "$splits"

cat > $output/options.js << ENDL
var options = {
  factor: $factor,
  host: "${host}tiles/",
  scaleControl: false,
  navigationControl: true,
  streetViewControl: false,
  noClear: false,
  backgroundColor: "#000000",
  isPng: true,
}

var modes = {
  'day': { name: "Day", alt: "Day Mode", data: $(cat $output/day.json)},
  'night': { name: "Night", alt: "Night Mode", data: $(cat $output/night.json)},
}
ENDL
