#include "IMPSNeutronBlock.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "db/DBTable.hh"

namespace COSMIC {

IMPSNeutronBlock::IMPSNeutronBlock(DBTable table) {
  Construct(table);
}

G4VSolid* IMPSNeutronBlock::ConstructSolidVolume(DBTable tbl) {

  std::string geo_id = tbl.GetIndexName();
  std::string geo_id_box   = geo_id + std::string("_box");
  std::string geo_id_pitch = geo_id + std::string("_pitch");

  // Now setup object
  std::vector<G4double> geo_size = tbl.GetVecG4D("size");  
  G4double worldX = geo_size[0];
  G4double worldY = geo_size[1];
  G4double worldZ = geo_size[2];
  G4Box* geo_box = new G4Box(geo_id_box, 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);

  // Subtract the fibre pitches
  G4double pitch_radius = 1*mm;
  G4Tubs* geo_pitch = new G4Tubs(geo_id_pitch, 0.0, pitch_radius, fmax(worldX, fmax(worldY, worldZ)), 0.0, CLHEP::twopi);
  
  G4RotationMatrix *rotation = new G4RotationMatrix();
  rotation->rotateX(0.0 * CLHEP::deg);
  rotation->rotateY(0.0 * CLHEP::deg);
  rotation->rotateZ(0.0 * CLHEP::deg);
  G4VSolid* geo_solid1  = new G4SubtractionSolid(geo_id, geo_box, geo_pitch, rotation, G4ThreeVector(0.5*worldX,0.0,0.0));

  rotation->rotateX(90.0 * CLHEP::deg);
  rotation->rotateY(90.0 * CLHEP::deg);
  rotation->rotateZ(0.0 * CLHEP::deg);
  G4VSolid* geo_solid2  = new G4SubtractionSolid(geo_id, geo_solid1, geo_pitch, rotation, G4ThreeVector(0.0,-0.5*worldY,0.0));

  rotation->rotateX(0.0 * CLHEP::deg);
  rotation->rotateY(90.0 * CLHEP::deg);
  rotation->rotateZ(0.0 * CLHEP::deg);
  G4VSolid* geo_solid3  = new G4SubtractionSolid(geo_id, geo_solid2, geo_pitch, rotation, G4ThreeVector(0.0,0.0,0.5*worldZ));


  return geo_solid3;
}

} // - namespace COSMIC
