#include "geo/GeoManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"

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

#include "db/DB.hh"
#include "db/DBTable.hh"

#include "simple/GeoBox.hh"
#include "simple/GeoTubs.hh"
#include "simple/GeoCone.hh"
#include "simple/GeoEllipticalTube.hh"
#include "simple/GeoEllipticalTunnel.hh"

#include "nuclearwaste/DryStorageCask_VSC24.hh"
#include "nuclearwaste/SmallSteelDrum.hh"
#include "nuclearwaste/LargeSteelDrum.hh"


#include "chance/HybridMuonTomographyDetector.hh"
#include "chance/AWEDriftChamber.hh"

#include "cadmesh/GeoCADMesh.hh"

#include "iso/ISOSpacingTest.hh"
#include "imps/IMPSNeutronBlock.hh"
#include "imps/IMPSBlockArray.hh"


#include <map>

namespace COSMIC{


GeoObject* GeoObjectFactory::Construct(DBTable table){

  std::string type = table.GetS("type");

  std::cout << "GEO: Constructing " << type << " : " << table.GetIndexName() <<  std::endl;
  if (type.compare("box")==0) return new GeoBox(table);
  else if (type.compare("tubs")==0) return new GeoTubs(table);
  else if (type.compare("cons")==0) return new GeoCone(table);
  else if (type.compare("eliptube")==0) return new GeoEllipticalTube(table);
  else if (type.compare("eliptunnel")==0) return new GeoEllipticalTunnel(table);
  else if (type.compare("DSC_VSC24") == 0) return new DryStorageCask_VSC24(table);
  else if (type.compare("hybrid_muontom") == 0) return new HybridMuonTomographyDetector(table);
  else if (type.compare("awe_drift") == 0) return new AWEDriftChamber(table);
  else if (type.compare("smallsteeldrum") == 0) return new SmallSteelDrum(table);
  else if (type.compare("largesteeldrum") == 0) return new LargeSteelDrum(table);
  else if (type.compare("isospacingtest") == 0) return new ISOSpacingTest(table);
  else if (type.compare("impsneutronblock") == 0) return new IMPSNeutronBlock(table);
  else if (type.compare("impsblockarray") == 0) return new IMPSBlockArray(table);
  #ifdef __USE_CADMESH__
  else if (type.compare("cadmesh") == 0) return new GeoCADMesh(table);
  #endif
  
  //  else if (type.compare("bristol_rpc") == 0) return new BristolRPC(table);

  std::cout << "Failed to Construct Geometry" << std::endl;
  throw;
  return 0;
}





//----------------------------------------------------------
GeoManager *GeoManager::fPrimary(0);

GeoManager::GeoManager(){
}

G4VPhysicalVolume* GeoManager::ConstructAll(){

  std::cout << "===============================" << std::endl;
  std::cout << "GEO: Building Geometry " << std::endl;
  std::vector<DBTable> tables_clone = DB::Get()->GetTableGroup("GEO");
  std::vector<DBTable>::iterator geo_iter = tables_clone.begin();


  int count = 0;
  while (tables_clone.size() > 0){

    for (geo_iter = tables_clone.begin(); geo_iter != tables_clone.end();){

      DBTable geo_tab = (*geo_iter);
      std::string geo_id = geo_tab.GetIndexName();

      // Check doesn't already exist
      if (HasGeoObject(geo_id)){
        std::cout << "Trying to add duplicate GEOM! "
                  << "INDEX : " << geo_id << std::endl;
        geo_tab.Print();
        throw;
      }

      // Check if mother already created
      int mother_status = MotherStatus(geo_tab);
      if (mother_status == 2){
        count++;
        ++geo_iter;
        continue; // If it hasn't skip for now
      }

      GeoObject* geo_obj = GeoObjectFactory::Construct(geo_tab);
      fGeoObjects[geo_id] = geo_obj;
      fGeoIDs.push_back(geo_id);

      // Remove tabs
      geo_iter = tables_clone.erase(geo_iter);

    }
    if (count > 10000){
      std::cout << "Stuck in infinite detector loop" << std::endl;
      throw;
    }
  }

  fWorldVolume = fGeoObjects["world"]->GetPhysical();
  return fWorldVolume;
}


int GeoManager::MotherStatus(DBTable geo_tab){
  if (geo_tab.Has("mother")){
    std::string mother = (geo_tab).GetS("mother");
    if (this->HasGeoObject(mother)){
      return 1;
    } else{
      return 2;
    }
  }
  return 0; // World has no mother
}

bool GeoManager::HasGeoObject(std::string name){
  return (fGeoObjects.find(name) != fGeoObjects.end());
}

}
