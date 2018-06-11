
{
  name: "GLOBAL",
  index: "config",
  flux: "pumasback",
  physics: "QGSP_BERT_HP",
  batchcommands: "",
}

// ---------------------------------------------------
// World Geometry : 20 x 20 x 30 AIR
// Then air and carbon base
{
  name: "GEO",
  index: "world",
  material: "G4_AIR",
  size: ["30.*m", "30.0*m", "20.*m"],
  type: "box",
  color: [1.0,1.0,1.0],
}

// --------------------------------------------------
// Detector

{
  name: "GEO",
  index: "detector",
  type: "box",
  mother: "world",
  material: "G4_Pb",
  size: ["2.0*m","2.0*m","2.0*m"],
  position: ["0.0*m","0.0","0.0"],

}

// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["3.0*m", "3.0*m", "0.5*m"],
  position: ["0.0","0.0", "9.75*m"],
}
