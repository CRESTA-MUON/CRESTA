#include "AWEScintillatorPaddle.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4Square.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Box.hh"
#include "G4Polyline.hh"
#include "Math/Functor.h"

#include "db/ROOTHeaders.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "sd/DetectorManager.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"
#include "sd/LongDriftSD.hh"
#include "chance/AWEDriftSD.hh"
#include "chance/BristolRPCSD.hh"

namespace COSMIC {

// --------------------------------------------------------------------------
void AWEScintillatorPaddle::Construct(DBTable table) {

  // SetID(table.GetIndexName());
  // SetType("awescintillatorpaddle");
  // std::string fName = table.GetIndexName();
  // std::string fMotherName = table.GetS("mother"); 
  // fMotherLogical = GEO::GetMotherLogicalFromStore(table);

  // // Setup Inputs
  // G4ThreeVector box_size = G4ThreeVector( 2.0*m, 2.0*m, 3*cm );
  // G4string box_material = "G4_C";
  // G4ThreeVector box_spacing = G4ThreeVector(0.0,0.0,3*cm);
  // G4Material* box_mat = MaterialFactory::GetMaterial("G4_C");
    
  // // Just a big box of scintillator. Efficiency mapping is handled by the other SD class.
  // G4VSolid* box_single  = new G4Box("box_single", box_size[0]*0.5, box_size[1]*0.5, box_size[2]*0.5);

  // G4VSolid* box_pair    = new G4UnionSolid("box_pair",    box_single, box_single, 0, box_spacing);
  // G4VSolid* box_triplet = new G4UnionSolid("box_triplet", box_pair,   box_single, 0, 2*box_spacing);

  // // Make Logical
  // fLogical = new G4LogicalVolume(box_triplet, box_sol, "box_paddle");

  // // Place in World
  // fPhysical = GeoSolid::ConstructPhysicalPlacement(table, fMotherLogical, fLogical);

  // // Assign Sensitive Detector
  // GeoSolid::ConstructSensitiveDetector(table, fLogical, fPhysical);

}
// --------------------------------------------------------------------------
} // namespace COSMIC
