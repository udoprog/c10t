<?php
ini_set("memory_limit", "200M");
define("VERBOSE", false);
/**
 * google-api.php
 *
 * author - Ben Rice (dashiva@dashiva.com)
 *
 * The function of this script is to render several Minecraft maps using the
 * map generator "c10t". A googlemap is then created using the tiles, and output.
 *
 * Usage:
 * php google-api.php [world] [output] [c10t args in quotes]
 * e.x.: php google-api.php myworld gmap "-isometric -r 270"
 */


# Required args
if(empty($argv[1]) || empty($argv[2]) ) {
	echo("google-api.php (Minecraft map parsing script)\r\nScriped by Ben Rice (dashiva@dashiva.com)\r\n\r\nUsage:\r\nphp google-api.php [world path]* [output directory]* [c10t args in quotes]\r\n *Required");
	exit();
}

$outHTML = "map.html";
$inPath = $argv[1];		# World files directory
$outDir = $argv[2];		# Tile directory
if( isset($argv[3]) ) {	# c10t args
	$c10tArgs = $argv[3]; //"--isometric";
}

$scale = array( 4096 => .0625, 	# 6.25%
				2048 => .125,	# 12.5%
				1024 => .25,		# 25%
				512  => .5,		# 50%
				256  => 1,		# 100%
				128  => 2 );	# 200%

$zoom = array(	4096 => 0,
				2048 => 1,
				1024 => 2,
				512  => 3,
				256  => 4,
				128  => 5);
				
$tileSizes = array(0 => 4096, 1 => 2048, 2=> 1024, 3=> 512, 4 => 256, 5 => 128);

# Verify world directory is valid
if( !is_dir($inPath) || !file_exists($inPath."level.dat") ) {
	echo("Invalid world directory: ".$inPath."\r\n");
	exit();
}
else
	echo("World directory...OK\r\n");

# Verify the executable
exec("c10t", $output);
if( empty($output) ) {
	echo("Cannot find c10t.exe!\r\n");
	exit();
}
else
	echo("c10t.exe...OK\r\n");

if (extension_loaded('gd') && function_exists('gd_info')) {
    echo("PHP GD extension...OK\r\n");
}
else {
	echo("GD extension is not installed or loaded!\r\nSee php.ini and be sure it contains extension=php_gd2.dll\r\n");
	exit();
}

# check / create output dir for images
if( !is_dir($outDir) ) {
	mkdir($outDir);
	echo("Creating folder ".$outDir."\r\n");
}
else
	echo("Folder ".$outDir." already exists.\r\n");



/**
 * function resizeImage
 *
 * This will use GD to resize an image.
 */
function resizeImage($file, $scale) {
    list($width, $height) = getimagesize($file);
	
    $newheight = $height * $scale;
    $newwidth = $newheight;
	
	if(VERBOSE)
		echo("Resizing " . $file . "\told(".$height.")". "\tnew(".$newheight.")" . "\tscale(" . $scale . ")\r\n");
	echo(".");
    $src = imagecreatefrompng($file);
    $dst = imagecreatetruecolor($newwidth, $newheight);
    $img = imagecopyresampled($dst, $src, 0, 0, 0, 0, $newwidth, $newheight, $width, $height);

	imagealphablending( $dst, false );
	imagesavealpha( $dst, true );
	imagepng(  $dst, $file, 9 );
}


/**
 * function generate
 *
 * This will execute c10t with the proper parameters.
 */
function generate($arg1, $name, $tile, $zoom, $scale) {
	$start = time();
	global $inPath;
	global $outDir;
	global $c10tArgs;
	echo("Generating: ".$name."\t Tile Size:".$tile."\t Zoom:".$zoom."...");
	
	# generate a set of split files
	$run = "c10t ".$c10tArgs." ".$arg1." --split ".$tile." -w ".$inPath." -o ".$outDir.$name.".%d.%d.".$zoom.".png --write-json ".$outDir.$name.".json";
	
	# Uncomment for details
	if(VERBOSE)
		echo("\r\n".$run."\r\n");
	
	exec($run, $output);
	if(VERBOSE)
		var_dump($output);
	$end = time();
	$elapsed = $end - $start;
	echo("Done in ".getTimeStr($elapsed)."\r\n");
	
}

/**
 * function read
 *
 * This will open a file and return it's contents.
 */
function read($file) {
	if(file_exists($file)) {
		$in = fopen ($file, "r");
		if (!$in)
			return false;
		$raw = "";
		if(filesize($file) <= 0)
			return false;
		else 
			$raw = fread($in, filesize($file));
		fclose($in);
		
		return $raw;
	}
	return false; 
}

/**
 * function write
 *
 * This will write contents to a file.
 */
function write($file, $content) {
	if( empty($file) || empty($content) )
		return false;
	
	$fl = fopen($file, "w+"); 
	fputs($fl, $content);
	fclose($fl);
	return true;
}

/**
 * function getTimeStr
 *
 * Return text for how many seconds is passed in.
 */
function getTimeStr($durationInSeconds)
{
  $week = floor($durationInSeconds / 86400 / 7);
  $day = $durationInSeconds / 86400 % 7;
  $hour = $durationInSeconds / 3600 % 24;
  $min = $durationInSeconds / 60 % 60;
  $sec = $durationInSeconds % 60;

  if($week != 0)
  {
    $time  = $week . isPlural($week, " week");
    if($day != 0)
      $time .= ", " . $day  . isPlural($day, " day");
  }
  else if($day != 0)
  {
    $time  = $day  . isPlural($day, " day");
    if($hour != 0)
      $time .= ", " . $hour . isPlural($hour, " hour");
  }
  else if($hour != 0)
  {
    $time  = $hour . isPlural($hour, " hour");
    if($min != 0)
      $time .= ", " . $min  . isPlural($min, " minute");
  }
  else if($min != 0)
  {
    $time  = $min  . isPlural($min, " minute");
    if($sec != 0)
      $time .= ", " . $sec  . isPlural($sec, " second");
  }
  else if($sec != 0)
  {
    $time  = $sec  . isPlural($sec, " second");
  }
  
  return $time;
}

/**
 * function isPlural
 *
 * Return pluralized text.
 */
function isPlural($num, $word){
  if($num > 1)
    return $word."s";
  else
    return $word;
}


$_start = time();

$fl = fopen($outHTML, "w+"); 
fputs($fl, $output);
fclose($fl);


# Loop and call generate for day/night and each zoom level
foreach($tileSizes as $t) {
	$z = $zoom[$t];
	$s = $scale[$t];
	
	generate("", "day", $t, $z, $s);
	generate("-n", "ngt", $t, $z, $s);
}


# convert the files to the appropriate sizes using GD module
$validExtensions = array("png");
$files = array();
if(is_dir($outDir)) {
	if($handle = opendir($outDir)) {
		while(($file = readdir($handle)) !== false) {
			## List
			$extension = end(explode(".", $file));
			
			if( in_array($extension, $validExtensions) )
				array_push($files, $file);

		}
		closedir($handle);
	}
	
	echo("Resizing images");
	foreach($files as $file) {
		$zoomLevel = substr($file, -5, 1);
		resizeImage($outDir.$file, $scale[$tileSizes[$zoomLevel]]);
	}
	echo(" Done.\r\n");
}
else {
	echo($outDir." is not a valid directory... Somehow. Quitting.");
	exit();
}






$output = '
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
      function EuclideanProjection() {};
      
      EuclideanProjection.prototype.fromLatLngToPoint = function(latLng, opt_point) {
        var point = opt_point || new google.maps.Point(0, 0);
        point.x = latLng.lng() / SCALE_FACTOR;
        point.y = latLng.lat() / SCALE_FACTOR;
        return point;
      };
      
      EuclideanProjection.prototype.fromPointToLatLng = function(point) {
        var lng = point.x * SCALE_FACTOR;
        var lat = point.y * SCALE_FACTOR;
        return new google.maps.LatLng(lat, lng, true);
      };
      
      function new_map_type(m, o, ob) {
        return extend(
          {
            getTileUrl: function(c, z) {
                return o.host + m + "." + c.x + "." + c.y + "." + z + ".png";
            },
            isPng: true,
            name : "none",
            alt : "none",
            minZoom: 0, maxZoom: 5,
            tileSize: new google.maps.Size(256, 256)
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

        var firstMode = null;
        
        for (m in modes) {
          var imt  = new google.maps.ImageMapType(new_map_type(m, opt, modes[m]));
          imt.projection = new EuclideanProjection();
          
          // Now attach the grid map type to the maps registry
          map.mapTypes.set(m, imt);
          if (firstMode == null) firstMode = m;
        }
        
        map.setMapTypeId(firstMode);

        var globaldata = modes[firstMode].data;
        
        {
          var world = globaldata.world;
          var center = new google.maps.Point(world["cx"] / 16, world["cy"] / 16);
          var latlng = EuclideanProjection.prototype.fromPointToLatLng(center)
          map.setCenter(latlng);
          map.setZoom(0);
        }
        
        for (var i = 0; i < globaldata.markers.length; i++)
        {
          var m = globaldata.markers[i];
          var point = new google.maps.Point(m.x / 16, m.y / 16);
          var latlng = EuclideanProjection.prototype.fromPointToLatLng(point)
          
          new google.maps.Marker({
              position: latlng, 
              map: map, 
              title: m.text
          });
        }
        
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
  <body onload="initialize(\'map_canvas\', options, modes)">
    <div id="map_canvas" style="width: 100%; height: 100%;"></div>
  </body>
</html>';

echo("Writing ".$outHTML."...");
if( !write($outHTML, $output) ) {
	echo("Error writing ".$outHTML);
	exit();
}
echo("Done.\r\n");


# Googlemap options JS
if( !$jsonDay = read($outDir."day.json") ) {
	echo("Error opening day.json");
	exit();
}

if( !$jsonNgt = read($outDir."ngt.json") ) {
	echo("Error opening ngt.json");
	exit();
}

# '.$outDir.'\ This extra slash below is intended for escaping
$js = 'var options = {
  host: "'.$outDir.'\",
  scaleControl: false,
  navigationControl: true,
  streetViewControl: false,
  noClear: false,
  backgroundColor: "#000000",
  isPng: true,
}

var modes = {
  \'day\': { name: "Day", alt: "Day Mode", data: '.$jsonDay.'},
  \'ngt\': { name: "Night", alt: "Night Mode", data: '.$jsonNgt.'},
}';

echo("Writing options.js...");
if( !write("options.js", $js) ) {
	echo("Error writing options.js");
	exit();
}
echo("Done.\r\n");

$_end = time();

echo("Complete in ".getTimeStr($_end - $_start) );

?>
