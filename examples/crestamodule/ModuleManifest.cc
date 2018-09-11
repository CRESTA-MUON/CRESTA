// Main CRESTA Includes
#include "analysis/VProcessor.hh"
#include "analysis/VDetector.hh"
#include "db/DBTable.hh"
#include "sd/TrueMuonTracker.hh"
#include "analysis/Analysis.hh"
#include "geo/GeoObject.hh"

// Dynamic Includes
#include "ProtonBeamGenerator.hh"
#include "TrueProtonTracker.hh"

// namespace CRESTA
using namespace CRESTA;

// -----------------------------------------------------
// CHANCE MANIFEST
// -----------------------------------------------------
extern "C" {

  // Manifest List
  // ---------------------------------------------------
  std::string CRESTA_PrintManifest(){
    std::string name = "ProtonManifest";

    std::cout << " MANIFEST : " << name << std::endl;
    std::cout << " --> trueproton (DET)"  << std::endl;
    std::cout << " --> protonbeam (FLUX)" << std::endl;

    return name;
  }

  // Detector Manifest
  // ---------------------------------------------------
  VDetector* CRESTA_ConstructDetector(void* table) {
   
    DBTable* tb = reinterpret_cast<DBTable*>(table);
    if (!tb) { return 0; }    
    std::string type = tb->GetS("type");

    if (type.compare("trueproton") == 0) return new TrueProtonTracker(*tb);

    return 0;
  }

  // Geometry Manifest
  // ---------------------------------------------------
  /*
  GeoObject* CRESTA_ConstructGeometry(void* table) {
    DBTable* tb =  reinterpret_cast<DBTable*>(table);
    if (!tb) { return 0; }
    std::string type = tb->GetS("type");
    return 0;
  }
  */

  // Processor Manifest
  // ---------------------------------------------------
  /*
  GeoObject* CRESTA_ConstructGeometry(void* table) {
    DBTable* tb =  reinterpret_cast<DBTable*>(table);
    if (!tb) { return 0; }
    std::string type = tb->GetS("type");
    return 0;
  }
  */

  // Generator manifest
  // ---------------------------------------------------
  G4VUserPrimaryGeneratorAction* CRESTA_LoadFluxGenerator(std::string type) {

    if (type.compare("protonbeam") == 0) return new ProtonBeamGenerator();

    return 0;
  }
  
}
// -----------------------------------------------------
// - END CHANCE MANIFEST
// -----------------------------------------------------
