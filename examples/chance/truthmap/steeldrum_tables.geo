{
  name: "GEO",
  index: "drum"
  type: "largesteeldrum"
  mother: "world"
  rotation: [90.0, 0.0, 0.0]
}
{
  name: "GEO",
  index: "leadblock"
  type: "box",
  size: ["10*cm","10*cm","10*cm"]
  position: ["0.0*m","0.0*m","0.0*m"]
  rotation: [0.0, 0.0, 0.0]
  material: "G4_Pb"
  mother: "drum"
}

{
  name: "DISCRIMINATOR"
  index: "config",
  grid_size: 10.0,
  grid_minx: -1000.0
  grid_maxx: 1000.0
  grid_miny: -1000.0
  grid_maxy: 1000.0
  grid_minz: -500.0
  grid_maxz: 500.0
  grid_offsetx: -500.0
  grid_offsety: 250.0
  grid_offsetz: 0.0
  smearing: 0.0
  force_momentum: 0.0
}

