#include "DetectorFactory.hh"

#include "G4LogicalVolume.hh"

#include "db/DBTable.hh"
#include "db/DynamicObjectLoader.hh"

#include "TrueMuonTracker.hh"
#include "SimpleScintillatorSD.hh"
#include "GridScintillatorSD.hh"
#include "LongDriftSD.hh"
#include "TargetMuonInOutTracker.hh"
#include "NeutronSD.hh"
#include "TrueInTracker.hh"
#include "LineOfSightSD.hh"
#include "PhotonSD.hh"

namespace COSMIC {

VDetector* DetectorFactory::CreateSD(DBTable tbl) {

  // Retrieve the table that matches this sensitive
  std::string type = tbl.GetS("type");

  // Now Search for different types
  if (type.compare("truemuon") == 0) return new TrueMuonTracker(tbl);
  if (type.compare("scintillator") == 0) return new SimpleScintillatorSD(tbl);
  if (type.compare("gridscintillator") == 0) return new GridScintillatorSD(tbl);
  if (type.compare("neutron") == 0) return new NeutronSD(tbl);
  if (type.compare("longdrift") == 0) return new LongDriftSD(tbl);
  if (type.compare("muoninouttracker") == 0) return new TargetMuonInOutTracker(tbl);
  if (type.compare("trueinabsorber") == 0) return new TrueInAbsorber(tbl);
  if (type.compare("los") == 0) return new LineOfSightSD(tbl);
  if (type.compare("photonsd") == 0) return new PhotonSD(tbl);

  VDetector* proc = DynamicObjectLoader::Get()->ConstructDynamicDetector(tbl);
  if (proc) return proc;

  // Check we didn't get to here
  std::cout << "Failed to Create New SD : '" << type << "'" << std::endl;
  throw;  
  return 0;
}

} // - namespace COSMIC
