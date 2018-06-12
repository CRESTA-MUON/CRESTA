
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
  size: ["300.*m", "300.0*m", "100.*m"],
  type: "box",
  color: [1.0,1.0,1.0],
}

// --------------------------------------------------
// Detector
{
  name: "GEO",
  index: "detector2",
  type: "box",
  mother: "world",
  material: "G4_AIR",
  size: ["1.0*m","1.0*m","1.0*m"],
  position: ["0.0*m","0.0","0.0*m"],
  sensitive: "truemuon"
  drawstyle: "solid"
}




// ---------------------------------------------------
// Flux Generator Source Location : Default is Shukla
{
  name: "FLUX",
  index: "source_box",
  size: ["300.0*m", "300.0*m", "0.05*m"],
  position: ["0.0","0.0", "99.95*m"],
}
