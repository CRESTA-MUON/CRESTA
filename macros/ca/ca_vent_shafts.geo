// Vent shaft
{
  name: "GEO",
  index: "vent_shaft0",
  material: "G4_WATER",
  mother: "world",
  size: ["target_tunnel_size", "target_tunnel_size", "det_tunnel_depth+sampling_target_box_size-8*m"],
  position: ["0.0","0.0","sampling_target_box_size+4.0*m"],
  type: "box",
  sensitive: "los"
}

/*
{
  index: "vent_shaft0",
  name: "GEO",
  material: "G4_WATER",
  mother: "world",
  position: ["0.0","0.0","sampling_target_box_size+4.0*m"],
  type: "cons",
  r_min1: "0.0",
  r_max1: "target_tunnel_size*0.5",
  r_min2: "0.0",
  r_max2: "target_tunnel_size*0.5",
  size_z: "det_tunnel_depth+sampling_target_box_size-8*m",
  color: [0.2,0.8,0.2,0.5],
  drawstyle: "solid",
  sensitive: "los"
}
*/
