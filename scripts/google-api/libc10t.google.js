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
  var world = ob.data.world;

  return extend(
    {
      getTileUrl: function(c, z) {
        var img = o.host + m + "." + (world.split - z) + "." + c.x + "." + c.y + ".png";
        return img
      },
      isPng: true,
      name : "none",
      alt : "none",
      minZoom: 1, maxZoom: world.split,
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
    
    // Now attach the grid map type to the map's registry
    map.mapTypes.set(m, imt);
    if (firstMode == null) firstMode = m;
  }
  
  map.setMapTypeId(firstMode);

  var globaldata = modes[firstMode].data;

  var world = globaldata.world;
  var factor = Math.pow(2, opt.factor);
  
  {
    var center = new google.maps.Point(world.cx / factor, world.cy / factor);
    var latlng = EuclideanProjection.prototype.fromPointToLatLng(center)
    map.setCenter(latlng);
    map.setZoom(1);
  }
  
  for (var i = 0; i < globaldata.markers.length; i++)
  {
    var m = globaldata.markers[i];
    var point = new google.maps.Point(m.x / factor, m.y / factor);
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
