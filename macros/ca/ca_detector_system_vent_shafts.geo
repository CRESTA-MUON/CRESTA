
// --------------------------------------------------
// Detector
{
  name: "DETECTOR",
  index: "scint",
  type: "scintillator"
}
{
  name: "DETECTOR",
  index: "truemuon",
  type: "truemuon",
}

{
  name: "DETECTOR",
  index: "los",
  type: "los",
}

//--------------------------------------------
// Main mother tunnel and detector volume (for easy placement)
//
{
  name: "GEO",
  index: "tunnel",
  type: "eliptunnel",
  mother: "world",
  material: "G4_AIR",
  size: ["11.630*m", "7.740*m", "world_box_width"],
  position: ["0.0","0.0","-0.5*det_tunnel_depth"],
  rotation: [90.0,0.0,90.0],
  sensitive: "los"
  color: [0.6,0.0.6,0.6,0.4],
  drawstyle: "solid"
}

{
  name: "GEO",
  index: "detector",
  type: "box",
  mother: "tunnel",
  material: "G4_AIR",
  size: ["sampling_target_box_size", "sampling_target_box_size", "sampling_target_box_size"],
  position: ["det_shift","0.5*sampling_target_box_size","0.0"],
  rotation_mother: [0.0,-90.0,90.0],
  sensitive: "los"
}

{
  name: "GEO",
  index: "bar0",
  type: "box",
  mother: "detector",
  material: "G4_PLASTIC_SC_VINYLTOLUENE",
  size: ["det_sx","det_sy","det_sz"],
  position: ["0.5*det_focal_length","0.0","-1*det_system_spacing"],
  rotation: [0.0,0.0,0.0],
  rotation_mother: [0.0,90.0,0.0],
  sensitive: "los",
  color: [0.0,0.0,1.0,0.5],
  drawstyle: "solid"
}

{  name: "GEO", index: "bar1", clone: "bar0",
   position: ["0.5*det_focal_length","0.0","0*det_system_spacing"]
}
{  name: "GEO", index: "bar2", clone: "bar0",
   position: ["0.5*det_focal_length","0.0","1*det_system_spacing"]
}



{  name: "GEO", index: "bar3", clone: "bar0",
   position: ["-det_sx+0.5*det_focal_length","-1*det_system_spacing","0.0"]
   rotation: [0.0,0.0,-90.0],
}
{  name: "GEO", index: "bar4", clone: "bar0",
   position: ["-det_sx+0.5*det_focal_length","0*det_system_spacing","0.0"]
   rotation: [0.0,0.0,-90.0],
}
{  name: "GEO", index: "bar5", clone: "bar0",
   position: ["-det_sx+0.5*det_focal_length","1*det_system_spacing","0.0"]
   rotation: [0.0,0.0,-90.0],
}





{  name: "GEO", index: "target_det0", clone: "bar0",
   size: ["30.0*cm","30.0*cm","4.0*cm"],
   position: ["-15.0*cm","0.0","-0.5*det_focal_length"]
   color: [1.0,0.0,0.0,0.5],
   rotation: [0.0,0.0,45.0],
   rotation_mother: [0.0,0.0,0.0]
}


{  name: "GEO", index: "target_det1", clone: "bar0",
   size: ["30.0*cm","30.0*cm","4.0*cm"],
   position: ["15.0*cm","0.0","-0.5*det_focal_length-4.0*cm"]
   color: [1.0,0.0,0.0,0.5],
   rotation: [0.0,0.0,45.0],
   rotation_mother: [0.0,0.0,0.0]
}

// Coincidence triggers
{
  name:  "TRIGGER",
  index: "trigger00",
  type: "coincidence",
  energy_threshold: "0",
  require_n: "2",
  processors: ["bar0_los","target_det0_los"],
  efficiency: "0.9*0.9"
}

{  name:  "TRIGGER",  index: "trigger01",  clone: "trigger00",  processors: ["bar0_los","target_det1_los"] }

{  name:  "TRIGGER",  index: "trigger10",  clone: "trigger00",  processors: ["bar1_los","target_det0_los"] }
{  name:  "TRIGGER",  index: "trigger11",  clone: "trigger00",  processors: ["bar1_los","target_det1_los"] }

{  name:  "TRIGGER",  index: "trigger20",  clone: "trigger00",  processors: ["bar2_los","target_det0_los"] }
{  name:  "TRIGGER",  index: "trigger21",  clone: "trigger00",  processors: ["bar2_los","target_det1_los"] }

{  name:  "TRIGGER",  index: "trigger30",  clone: "trigger00",  processors: ["bar3_los","target_det0_los"] }
{  name:  "TRIGGER",  index: "trigger31",  clone: "trigger00",  processors: ["bar3_los","target_det1_los"] }

{  name:  "TRIGGER",  index: "trigger40",  clone: "trigger00",  processors: ["bar4_los","target_det0_los"] }
{  name:  "TRIGGER",  index: "trigger41",  clone: "trigger00",  processors: ["bar4_los","target_det1_los"] }

{  name:  "TRIGGER",  index: "trigger50",  clone: "trigger00",  processors: ["bar5_los","target_det0_los"] }
{  name:  "TRIGGER",  index: "trigger51",  clone: "trigger00",  processors: ["bar5_los","target_det1_los"] }

// Now pairs detectors in the upper planes
{  name:  "TRIGGER",   require_n: "3", index: "trigger231",  clone: "trigger00",  processors: ["bar2_los","bar3_los","target_det1_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger230",  clone: "trigger00",  processors: ["bar2_los","bar3_los","target_det0_los"] }

{  name:  "TRIGGER",     require_n: "3", index: "trigger240",  clone: "trigger00",  processors: ["bar2_los","bar4_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger241",  clone: "trigger00",  processors: ["bar2_los","bar4_los","target_det1_los"] }

{  name:  "TRIGGER",     require_n: "3", index: "trigger250",  clone: "trigger00",  processors: ["bar2_los","bar5_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger251",  clone: "trigger00",  processors: ["bar2_los","bar5_los","target_det1_los"] }

{  name:  "TRIGGER",     require_n: "3", index: "trigger130",  clone: "trigger00",  processors: ["bar1_los","bar3_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger131",  clone: "trigger00",  processors: ["bar1_los","bar3_los","target_det1_los"] }

{  name:  "TRIGGER",     require_n: "3", index: "trigger140",  clone: "trigger00",  processors: ["bar1_los","bar4_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger141",  clone: "trigger00",  processors: ["bar1_los","bar4_los","target_det1_los"] }

{  name:  "TRIGGER",     require_n: "3", index: "trigger150",  clone: "trigger00",  processors: ["bar1_los","bar5_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger151",  clone: "trigger00",  processors: ["bar1_los","bar5_los","target_det1_los"] }


{  name:  "TRIGGER",     require_n: "3", index: "trigger030",  clone: "trigger00",  processors: ["bar0_los","bar3_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger031",  clone: "trigger00",  processors: ["bar0_los","bar3_los","target_det1_los"] }

{  name:  "TRIGGER",     require_n: "3", index: "trigger040",  clone: "trigger00",  processors: ["bar0_los","bar4_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger041",  clone: "trigger00",  processors: ["bar0_los","bar4_los","target_det1_los"] }

{  name:  "TRIGGER",     require_n: "3", index: "trigger050",  clone: "trigger00",  processors: ["bar0_los","bar5_los","target_det0_los"] }
{  name:  "TRIGGER",     require_n: "3", index: "trigger051",  clone: "trigger00",  processors: ["bar0_los","bar5_los","target_det1_los"] }





// The coincidence trigger - set up to mimic the DAQ system
// These are all OR'd together
/*
{
  name:  "TRIGGER",
  index: "trigger56",
  type: "coincidence",
  energy_threshold: "0",
  require_n: "2",
  processors: ["bar5_los","bar6_los"],
  efficiency: "0.7*0.9"
}
{  name:  "TRIGGER",  index: "trigger46",  clone: "trigger56",  processors: ["bar4_los","bar6_los"] }
{  name:  "TRIGGER",  index: "trigger36",  clone: "trigger56",  processors: ["bar3_los","bar6_los"] }
{  name:  "TRIGGER",  index: "trigger26",  clone: "trigger56",  processors: ["bar2_los","bar6_los"] }
{  name:  "TRIGGER",  index: "trigger16",  clone: "trigger56",  processors: ["bar1_los","bar6_los"] }
{  name:  "TRIGGER",  index: "trigger06",  clone: "trigger56",  processors: ["bar0_los","bar6_los"] }

{  name:  "TRIGGER",  index: "trigger57",  clone: "trigger56",  processors: ["bar5_los","bar7_los"] }
{  name:  "TRIGGER",  index: "trigger47",  clone: "trigger56",  processors: ["bar4_los","bar7_los"] }
{  name:  "TRIGGER",  index: "trigger37",  clone: "trigger56",  processors: ["bar3_los","bar7_los"] }
{  name:  "TRIGGER",  index: "trigger27",  clone: "trigger56",  processors: ["bar2_los","bar7_los"] }
{  name:  "TRIGGER",  index: "trigger17",  clone: "trigger56",  processors: ["bar1_los","bar7_los"] }
{  name:  "TRIGGER",  index: "trigger07",  clone: "trigger56",  processors: ["bar0_los","bar7_los"] }
*/
