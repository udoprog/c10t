function c10t(json) {
  this.MapX = json.st.MapX;
  this.MapY = json.st.MapY;
  this.MapZ = json.st.MapZ;
  
  this.mx_x = json.world.mx_x;
  this.mn_x = json.world.mn_x;
  this.mx_z = json.world.mx_z;
  this.mn_z = json.world.mn_z;
  this.mx_y = this.MapY;
  this.mn_y = 0;
  this.cx = json.world.cx;
  this.cy = json.world.cy;
  this.dx = json.world.dx;
  this.dz = json.world.dz;
  this.dy = this.MapY;
  
  this.mode = this.modes[json.world.mode];
  this.projection = this.projections[this.mode];
  
  this.project = function (x, y, z)
  {
    cropCompensation = this.projection(0, 0, 0);
    coordinates = this.projection(x, y, z);
    
    coordinates[0] -= cropCompensation[0] - this.cx;
    coordinates[1] -= cropCompensation[1] - this.cy;
    
    return coordinates;
  }
}

c10t.prototype.modes = {
  0x0: "top",
  0x1: "oblique",
  0x2: "obliqueangle",
  0x3: "isometric"
};

c10t.prototype.projections = {
  'top': function(x, y, z) {
    x -= this.mn_x;
    z -= this.mn_z;
    return [(this.dz - z - 1), x];
  },
  'oblique': function(x, y, z) {
    x -= this.mn_x;
    z -= this.mn_z;
    return [(this.dz - z - 1) + x, x];
  },
  'obliqueangle': function(x, y, z) {
    x -= this.mn_x;
    z -= this.mn_z;
    return [(this.dz - z - 1) + x, (this.dy - y - 1) + z + x];
  },
  'isometric': function(x, y, z) {
    x -= this.mn_x;
    z -= this.mn_z;
    return [2 * ((this.dz - z - 1) + x), 2 * (this.dy - y - 1) + z + x];
  }
};
