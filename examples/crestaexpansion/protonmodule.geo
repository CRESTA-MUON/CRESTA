// ---------------------------------------------------
// Global Config
{
  name: "GLOBAL",
  index: "config",
  flux: "protonbeam",
  physics: "QGSP_BERT",
  batchcommands: "",
  action_initialization: "cresta"
}

{
  name: "protonbeam"
  index: "config"
  energy: "2*GeV"
  position: ["0.0","0.0","0.9*m"]
  direction: ["0.0","0.0","-1.0"]
  rate: "1/s"  
}

// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["3.*m", "3.0*m", "2.*m"],
  type: "box",
  color: [1.0,1.0,1.0],
}

// --------------------------------------------------
// Detector
{
  name: "DETECTOR",
  index: "protontracker",
  type: "trueproton",
}

//--------------------------------------------
// Main mother detector volume (for easy placement)
{
  name: "GEO",
  index: "tracker",
  type: "box",
  mother: "world",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["40*cm","80*cm","40*cm"],
  position: ["0.0","0.0","0.0"],
  sensitive: "protontracker",
}
