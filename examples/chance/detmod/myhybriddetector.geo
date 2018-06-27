// --------------------------------
// Define all constants
{
   name: "VARIABLE",
   index: "myconstants"
   rpc_x: "2.0*m",
   rpc_y: "2.0*m",
   rpc_efficiency: "1.0"
   rpc_resolution: "0.4*mm",
   rpc_thickness: "6*mm",
   rpc_spacing: "25*mm",
   rpc_offset: "0.45*m",
}

{
   name: "INFO"
   index: "detectors",
   rpc_0: "x",
   rpc_1: "y",
   rpc_2: "x",
   rpc_3: "y",
   rpc_4: "x",
   rpc_5: "y",
   
}



// --------------------------------
// Materials
{
  name: "MATERIAL",
  index: "AWE_GASMIX1",
  element_names: ["Ar","C","O","H"],
  element_counts: [0.925,0.75,0.100,0.100],
  density: 2.4,  
  density_units: "g/cm3"
}
{
  name: "MATERIAL",
  index: "AWE_SCINTMIX",
  element_names: ["C","H"]
  element_counts: [0.9,0.1],
  density: 2.4,
  density_units: "g/cm3"
}

{
  name:	"MATERIAL",
  index: "BRISTOL_GASMIX1",
  element_names: ["O","Na","Si","Ca"]
  element_counts: [0.45,0.10,0.35,0.10],
  density: 2.4,
  density_units: "g/cm3"
}

// --------------------------------
// Main Detector geometry for placement
// --> Main Box, just air
{
  name: "GEO",
  index: "volume",
  type: "box",
  size: [2.0,2.0,1.0],
  size_units: "m"
  position: [0.0,0.0,0.0],
  material: "G4_AIR",
}

// --------------------------------
// --> True Muon Detector
{
 name: "DETECTOR",
 index: "truemuon",
 type: "truemuon",
}
{
  name: "GEO",
  index: "trueentrance",
  type: "box",
  mother: "volume",
  size: ["2.0*m","2.0*m","0.001*m"],
  position: ["0.0","0.0","0.4995*m"],
  material: "G4_AIR",	
  sensitive: "truemuon"
}

{
  name: "GEO",
  index: "trueexit",
  type: "box",
  mother: "volume",
  size: ["2.0*m","2.0*m","0.001*m"],
  position: ["0.0","0.0","-0.4995*m"],
  material: "G4_AIR",
  sensitive: "truemuon"
}

// --------------------------------
// --> Make Bristol RPCs (turn this into a templated function)
{
  name: "GEO",
  index: "rpc_template",
  type: "box",
  size: ["rpc_x","rpc_y","rpc_thickness"]
  material: "BRISTOL_GASMIX1",
  mother: "volume", 
  sensitive: "bristol_rpc_rec"
  color: [0.0,0.5,0.0,0.5]
  drawstyle: "solid",
}

{
  name: "GEO",
  index: "rpc_positions",
  rpc_0: ["0.0","0.0","rpc_offset",              "0.0","0.0","90.0"],
  rpc_1: ["0.0","0.0","rpc_offset+rpc_spacing",  "0.0","0.0","0.0"],
  rpc_2: ["0.0","0.0","-rpc_offset",             "0.0","0.0","90.0"],
  rpc_3: ["0.0","0.0","-rpc_offset-rpc_spacing", "0.0","0.0","0.0"],
}

// --------------------------------
// Scintillator Panel
// --> Make simple scintillator
{
  name: "GEO",
  index: "scintillator_template",
  type: "box",
  mother: "volume",
  // 
  // Big ~2*2 slab 
  size: [1.9,1.9,0.05],
  size_units: "m"
  //
  // Made out of G4 plastic scintillator
  material: "AWE_SCINTMIX",
  //
  // Sensitive scintillator object
  sensitive: "scint_true"
  //
  // Drawing options
  color: [0.,0.,1.,0.2],
  drawstyle: "solid"
}

{
  name: "GEO",
  index: "scintillator_positions"
  scintillator_0: ["0.0","0.0"," 0.38*m","0.0","0.0","0.0"]
  scintillator_1: ["0.0","0.0","-0.38*m","0.0","0.0","0.0"]
}

// --------------------------------
// --> drift template
{
  name: "GEO",
  index: "drift_template",
  type: "box",
  mother: "volume"
  // Set placement units for later
  position_units: "m",
  material: "AWE_GASMIX1",
  size: ["1.8*m","0.6*m","5*cm"]
}

// --> drift placements
{
  name: "GEO",
  index: "drift_positions",
  //
  // pos and rot [x,y,z,thx,thy,thz] 
  // pos and rot [x,y,z,thx,thy,thz] in m
}







