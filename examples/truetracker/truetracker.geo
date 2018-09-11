// ---------------------------------------------------
// Global Config
{
  name: "GLOBAL",
  index: "config",
  flux: "cry",
  physics: "QGSP_BERT",
  batchcommands: "",
  action_initialization: "cresta"
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
  index: "truemuon",
  type: "truemuon",
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
  sensitive: "truemuon",
}

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["3.0*m", "3.0*m", "0.05*m"],
  position: ["0.0","0.0", "0.975*m"],
}
