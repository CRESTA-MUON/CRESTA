#ifndef __IMPSBlockArray_HH__
#define __IMPSBlockArray_HH__

#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

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

#include "db/ROOTHeaders.hh"
#include "sd/LongDriftSD.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "analysis/VProcessor.hh"
#include "analysis/Analysis.hh"
#include "sd/SimpleScintillatorSD.hh"



class G4LogicalVolume;
class G4VPhysicalVolume;

namespace CRESTA {

// double inheritance, super dodgy!
class IMPSBlockArray : public GeoObject {
public:
  inline IMPSBlockArray() {};
  inline IMPSBlockArray(DBTable table) {Construct(table);};

  void Construct(DBTable table);

protected:

  std::vector<GeoObject*> fSubObjects;

};

} // namespace CRESTA
#endif
