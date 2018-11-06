// ---------------------------------------------------
// Global Config
{
  name: "GLOBAL",
  index: "config",
  flux: "cry",
  physics: "scintillator",
  batchcommands: "",
  action_initialization: "optical"
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
// Example material properties. TAKE WITH PINC OF SALT
{
  name: "MATERIAL",
  index: "LABLOGIC_SCINTILLATOR_GD"
  color: [0.1,0.1,1.0,0.6],
  drawstyle: "solid"
  density: "1.032*g/cm3"
  element_names: ["C","H","Gd"]
  element_counts: [0.4736842105,0.5263157895,0.02]

  // Variable Properties
  // Can specify a generic PROPERTIES_X
  // Or can do e.g. FASTCOMPONENT_X and FASTCOMPONENT_Y

  PROPERTIES_X: ["2*eV","7.0*eV","10*eV"]
  FASTCOMPONENT_Y: ["0.1", "1.0", "1.0"]
  SLOWCOMPONENT_Y: ["0.1", "1.0", "1.0"]
  RINDEX_Y: ["1.59","1.59","1.59"]
  ABSLENGTH_Y: ["35*cm","35*cm","35*cm"]

  // Constants
  SCINTILLATIONYIELD: "1200./MeV"
  RESOLUTIONSCALE: 1.0
  FASTTIMECONSTANT: "10*ns"
  SLOWTIMECONSTANT: "30*ns"
  YIELDRATIO: "1.0"
  BIRKSCONSTANT: "0.126*mm/MeV"
}


//--------------------------------------------
// Main mother detector volume (for easy placement)
{
  name: "GEO",
  index: "tracker",
  type: "box",
  mother: "world",
  material: "LABLOGIC_SCINTILLATOR"
  size: ["40*cm","80*cm","40*cm"],
  position: ["0.0","0.0","0.0"],
}

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["3.0*cm", "3.0*cm", "0.05*cm"],
  position: ["0.0","0.0", "0.30*m"],
}
