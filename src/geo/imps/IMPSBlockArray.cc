#include "IMPSBlockArray.hh"

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

#include "db/ROOTHeaders.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"
#include "sd/DetectorFactory.hh"
#include "sd/LongDriftSD.hh"
#include "IMPSNeutronBlock.hh"

namespace CRESTA {

// --------------------------------------------------------------------------
void IMPSBlockArray::Construct(DBTable table) {
  SetID(table.GetIndexName());
  SetType(table.GetS("type"));
  std::string fName = table.GetIndexName();
  std::string fMotherName = table.GetS("mother");


  // Get the block positioning and sizes, make the temporary box
  // Place multiple copies of the block in a for loop grid pattern
  // nx, ny, nz
  // spacingx, spacingy, spacingz

  table.Set("type", "impsneutronblock");
  std::vector<G4double> pos;
  pos.push_back(0.0);
  pos.push_back(0.0);
  pos.push_back(0.0);

  // Loop over -nx/2
  int nx = 4;
  int ny = 4;
  int nz = 8;
  G4double sx = 0.1*mm;
  G4double sy = 0.1*mm;
  G4double sz = 0.1*mm;
  G4double wx = 5.0*cm;
  G4double wy = 5.0*cm;
  G4double wz = 2.5*cm;

  IMPSNeutronBlock* block = NULL;

  G4double startx = -( (1.0 + 0.5*(nx%2)) * double(nx) / 2.0) * (wx + sx);
  G4double starty = -( (1.0 + 0.5*(ny%2)) * double(ny) / 2.0) * (wy + sy);
  G4double startz = -( (1.0 + 0.5*(nz%2)) * double(nz) / 2.0) * (wz + sz);

  for (int i = 0; i < nx; i++){
    for (int j = 0; j < ny; j++){
      for (int k = 0; k < nz; k++){



        pos[0] = startx + i*wx + i*sx;
        pos[1] = starty + j*wy + j*sy;
        pos[2] = startz + k*wz + k*sz;

        std::cout << i << " " << j << " " << k << " --> " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
        table.Set("position",pos);

        // if (!block){
          block = new IMPSNeutronBlock(table);
        // } else {
          // block->PlaceCopy( table );
        // }
      }
    }
  }

}
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
} // namespace CRESTA
