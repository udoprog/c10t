#!/bin/bash

set -e

world=$1
dir=$2

C10T=./c10t
C10T_ARGS="-w $world -L -70,-60,5,20"

if [[ ! -d $world ]]; then
  echo "Not a directory: $world"
  exit 1;
fi

if [[ ! -d $dir ]]; then
  echo "Not a directory: $dir"
  exit 1;
fi

echo_html() {
  cat << END
<html>
  <head>
    <title>c10t - showroom</title>
    <style type="text/css">
.desc {
  font-size: 120%;
  padding: 20px;
}

.images {
  padding: 10px;
}
    </style>
  </head>
  <body>
    <h1>c10t Showroom</h1>
    <p class="desc">
      Normal map
    </p>
    <div class="images">
      <img src="n.png" />
      <img src="n-n.png" />
      <img src="n-H.png" />
      <img src="n-C.png" />
    </div>
    
    <p class="desc">
      Oblique map
    </p>
    <div class="images">
      <img src="q.png" />
      <img src="q-n.png" />
      <img src="q-H.png" />
      <img src="q-C.png" />
    </div>
    
    <p class="desc">
      Oblique angled map
    </p>
    <div class="images">
      <img src="y.png" />
      <img src="y-n.png" />
      <img src="y-H.png" />
      <img src="y-C.png" />
    </div>
    
    <p class="desc">
      Isometric
    </p>
    <div class="images">
      <img src="z.png" />
      <img src="z-n.png" />
      <img src="z-H.png" />
      <img src="z-C.png" />
    </div>
  </body>
</html>
END
}

echo_html > $dir/index.html

motion() {
  $C10T $C10T_ARGS $1 -o $dir/$2.png
  $C10T $C10T_ARGS $1 --night -o $dir/$2-n.png
  $C10T $C10T_ARGS $1 --heightmap -o $dir/$2-H.png
  $C10T $C10T_ARGS $1 --cave-mode -o $dir/$2-C.png
}

motion "" "n"
motion "-q" "q"
motion "-y" "y"
motion "-z" "z"
