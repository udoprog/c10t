#!/bin/bash

C10T=./c10t
C10T_OPTS=""
C10T_OUT=c10t.out.txt

set -e

if [[ $# != 2 ]]; then
  echo "Usage: $0 <world> <target-path>"
  exit 1
fi

current=$(dirname $0)
world=$1
target=$2

C10T_OPTS="$C10T_OPTS -w $world --split 16"

if [[ -z $world ]] || [[ ! -d $world ]]; then
  echo "Directory does not exist: $world";
  exit 1;
fi

if [[ ! -x $C10T ]] ; then
  echo "Not an executable: $C10T"
  exit 1
fi

[ ! -d $target ] && mkdir -p $target
[ ! -d $target/parts ] && mkdir -p $target/parts

cat > $target/index.html << ENDL
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
    <script type="text/javascript" src="http://maps.google.com/maps/api/js?sensor=false"></script>
    <script type="text/javascript">
      function extend(t , o) {
        for (k in o) { if (o[k] != null) { t[k] = o[k]; } }
        return t;
      }
      
      function keys(o) {
        var a = [];
        for (m in modes) { a[a.length] = m; };
        return a;
      }
      
      // The maximum width/height of the grid in regions (must be a power of two)
      var GRID_WIDTH_IN_REGIONS = 4096;
      // Map from a GRID_WIDTH_IN_REGIONS x GRID_WIDTH_IN_REGIONS square to Lat/Long (0, 0),(-90, 90)
      var SCALE_FACTOR = 90.0 / GRID_WIDTH_IN_REGIONS;

      // Override the default Mercator projection with Euclidean projection
      // (insert oblig. Flatland reference here)
      function Proj() {};
      
      Proj.prototype.fromLatLngToPoint = function(latLng, opt_point) {
        var point = opt_point || new google.maps.Point(0, 0);
        point.x = latLng.lng() / SCALE_FACTOR;
        point.y = latLng.lat() / SCALE_FACTOR;
        return point;
      };
      
      Proj.prototype.fromPointToLatLng = function(point) {
        var lng = point.x * SCALE_FACTOR;
        var lat = point.y * SCALE_FACTOR;
        return new google.maps.LatLng(lat, lng, true);
      };
      
      function new_map_type(o, ob) {
        return extend(
          {
            base: "base",
            getTileUrl: function(c, z) {
                return o.host + this.base + "." + c.y + "." + -c.x + ".png";
            },
            isPng: true,
            name : "none",
            alt : "none",
            minZoom: 0, maxZoom: 0,
            projection: new Proj()
          },
          ob
        );
      }
      
      function initialize(id, opt, modes) {
        var element = document.getElementById(id);
        
        opt = extend(opt, {
          mapTypeControlOptions: {
            mapTypeIds: keys(modes),
            style: google.maps.MapTypeControlStyle.DROPDOWN_MENU}});
        
        var map = new google.maps.Map(element, opt);
        
        for (m in modes) {
          var imt  = new google.maps.ImageMapType(new_map_type(opt, modes[m]));
          
          // Now attach the grid map type to the map's registry
          map.mapTypes.set(m, imt);
          map.setMapTypeId(m);
        }
        
        // This starting lat/long will center us over the region at 25,23(Spawn)
        map.setCenter(new google.maps.LatLng(0, 0));
        map.setZoom(0);
        
        // We can now set the map to use the 'grid' map type
        
        if (window.attachEvent) {
          window.attachEvent("onresize", function() {this.map.onResize()} );
        } else {
          window.addEventListener("resize", function() {this.map.onResize()} , false);
        }
      }
    </script>
    
    <!-- Make the document body take up the full screen -->
    <style type="text/css">
        v\:* {behavior:url(#default#VML);}
        html, body {width: 100%; height: 100%}
        body {margin-top: 0px; margin-right: 0px; margin-left: 0px; margin-bottom: 0px}
    </style>
    
    <script type="text/javascript" src="options.js"></script>
  </head>
  <body onload="initialize('map_canvas', options, modes)">
    <div id="map_canvas" style="width: 100%; height: 100%;"></div>
  </body>
</html>
ENDL

cat > $target/options.js << ENDL
var options = {
  host: "parts/",
  scaleControl: false,
  navigationControl: false,
  streetViewControl: false,
  noClear: false,
  backgroundColor: "#000000",
  isPng: true,
}

var modes = {
  'height': { base: 'height', name: "Heightmap", alt: "Heightmap in Top-Down view", tileSize: new google.maps.Size(256, 256)},
  'caves': { base: 'caves', name: "Cavemode", alt: "Cavemode in Top-Down view", tileSize: new google.maps.Size(256, 256)},
  'night': { base: 'night', name: "Night", alt: "Night in Top-Down view", tileSize: new google.maps.Size(256, 256)},
  'day': { base: 'day', name: "Day", alt: "Day in Top-Down view", tileSize: new google.maps.Size(256, 256)}
}
ENDL

echo "NOTE: if something goes wrong, check out $C10T_OUT"

echo -n "Generating Day... "
$C10T $C10T_OPTS -o $target/parts/day.%d.%d.png &> $C10T_OUT
echo "done!"

echo -n "Generating Night... "
$C10T $C10T_OPTS -n -o $target/parts/night.%d.%d.png &> $C10T_OUT
echo "done!"

echo -n "Generating Caves... "
$C10T $C10T_OPTS -c -o $target/parts/caves.%d.%d.png &> $C10T_OUT
echo "done!"

echo -n "Generating Heightmap... "
$C10T $C10T_OPTS --heightmap -o $target/parts/height.%d.%d.png &> $C10T_OUT
echo "done!"
