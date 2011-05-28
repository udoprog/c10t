function extend(t , o) {
  for (k in o) { if (o[k] != null) { t[k] = o[k]; } }
  return t;
}

function keys(o) {
  var a = [];
  for (m in modes) { a[a.length] = m; };
  return a;
}

// Direct Link Feature
var _globalDefaults = {
  lat : 0,
  lng : 0,
  zoom : 1,
  type : "day",
}

function get_parameters() {
  var lat = _globalDefaults.lat;
  var lng = _globalDefaults.lng;
  var zoom = _globalDefaults.zoom;
  var type = _globalDefaults.type;

  var query = location.search.substring(1);

  var pairs = query.split("&");
  for (var i=0; i<pairs.length; i++) {
    var pos = pairs[i].indexOf("=");
    var argname = pairs[i].substring(0,pos).toLowerCase();
    var value = pairs[i].substring(pos+1).toLowerCase();

    if (argname == "lat") {lat = parseFloat(value);}
    if (argname == "lng") {lng = parseFloat(value);}
    if (argname == "zoom") {zoom = parseInt(value);}
    if (argname == "type") {type = value;}
  }

  return {
    lat: lat,
    lng: lng,
    zoom: zoom,
    type: type,
  };
}

function retrieveLink(map) {

  var latlng = EuclideanProjection.prototype.fromPointToLatLng(_globalCenter);
  
  var lat = map.getCenter().lat() - latlng.lat();
  var lng = map.getCenter().lng() - latlng.lng();

  var pos = window.location.href.indexOf("?");
  var base = pos > -1 ? window.location.href.substring(0,pos) : window.location.href;
  var ret = base
    + "?lat=" + lat.toFixed(6)
    + "&lng=" + lng.toFixed(6);
  if (map.getZoom() != _globalDefaults.zoom)
    ret += "&zoom=" + map.getZoom();
  if (map.getMapTypeId() != _globalDefaults.type)
    ret += "&type=" + map.getMapTypeId();
    
  return ret;
}

function refreshLink(map, control) {
  var link = retrieveLink(map);
  
  if (!(control.innerHTML.replace(/&amp;/gi,"&") == link)) {
    control.innerHTML = link;
  }
}

function createLinkControl(map) {
  
  var controlDiv = document.createElement('DIV');
  controlDiv.style.padding = '5px';

  var controlUI = document.createElement('DIV');
  controlUI.style.backgroundColor = 'white';
  controlUI.style.borderStyle = 'solid';
  controlUI.style.borderWidth = '1px';
  controlUI.style.textAlign = 'center';
  controlUI.title = 'The link to the current position';
  controlDiv.appendChild(controlUI);

  var controlText = document.createElement('DIV');
  controlText.name = "linkControl";
  controlText.style.fontFamily = 'Arial,sans-serif';
  controlText.style.fontSize = '12px';
  controlText.style.paddingLeft = '4px';
  controlText.style.paddingRight = '4px';
  controlUI.appendChild(controlText);
  
  if (controlDiv.attachEvent) {
    controlDiv.attachEvent("onclick", function() {window.location.href = retrieveLink(map)});
  } else {
    controlDiv.addEventListener("click", function() {window.location.href = retrieveLink(map)} , false);
  }
  
  map.controls[google.maps.ControlPosition.TOP_RIGHT].push(controlDiv);
  
  refreshLink(map, controlText);
  
  return controlText;
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
    var parameters = get_parameters();
    var latlng;
    _globalCenter = new google.maps.Point(world.cx / factor, world.cy / factor);
    latlng = EuclideanProjection.prototype.fromPointToLatLng(_globalCenter)
    parameters.lat += latlng.lat();
    parameters.lng += latlng.lng();
	  latlng = new google.maps.LatLng(parameters.lat, parameters.lng, true);
    map.setZoom(parameters.zoom);
    map.setMapTypeId(parameters.type);

    map.setCenter(latlng);
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
  
  var linkControl = createLinkControl(map);
  
  if (window.attachEvent) {
    window.attachEvent("onresize", function() {this.map.onResize()} );
    window.attachEvent("onmousemove", function() {refreshLink(map, linkControl)} );
  } else {
    window.addEventListener("resize", function() {this.map.onResize()} , false);
    window.addEventListener("mousemove", function() {refreshLink(map, linkControl)} , false);
  }
}
