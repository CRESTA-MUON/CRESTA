
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
  size: ["60.*m", "60.0*m", "60.*m"],
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
  material: "G4_C",
  size: ["20.0*m","20.0*m","60.0*m"],
  position: ["0.0*m","0.0","0.0*m"],

}


{
  name: "GEO",
  index: "detector2",
  type: "box",
  mother: "detector",
  material: "G4_AIR",
  size: ["1.0*m","1.0*m","1.0*m"],
  position: ["0.0*m","0.0","0.0*m"],
}

{
  name: "GEO",
  index: "detector3",
  type: "box",
  mother: "detector",
  material: "G4_Pb",
  size: ["3.0*m","3.0*m","3.0*m"],
  position: ["0.0*m","0.0","20.0*m"],
}




// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["3.0*m", "3.0*m", "0.5*m"],
  position: ["0.0","0.0", "9.75*m"],
}
