#include "FluxFactory.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "db/DynamicObjectLoader.hh"
#include "flux/ShuklaPrimaryGenerator.hh"
#include "flux/CRYPrimaryGenerator.hh"
#include "flux/PumasBackwardsGenerator.hh"
#include "flux/LOSPrimaryGenerator.hh"
#include "flux/PrimaryGeneratorROOT.hh"
#include "flux/IsotropicSphereSource.hh"

namespace COSMIC {

G4VUserPrimaryGeneratorAction* FluxFactory::LoadFluxGenerator() {
  // Get default config if requested
  DBTable tbl = DB::Get()->GetTable("GLOBAL", "config");
  return LoadFluxGenerator(tbl);
}

G4VUserPrimaryGeneratorAction* FluxFactory::LoadFluxGenerator(DBTable table) {

  // Get the flux specification from the global config
  std::string type = table.GetS("flux");
  std::cout << "FLX: Loading Primary Flux Generator : " << type << std::endl;

  // Read Flux Table using ugly string comparisons!
  if (type.compare("shukla") == 0) return new ShuklaPrimaryGenerator();
  if (type.compare("cry") == 0)    return new CRYPrimaryGenerator();
  if (type.compare("los") == 0)    return new PrimaryGeneratorROOT();
  if (type.compare("root") == 0)   return new LOSPrimaryGenerator();
  if (type.compare("isotropicsphere") == 0) return new IsotropicSphereGenerator();

#ifdef __USE_PUMAS__
  if (type.compare("pumasback") == 0) return new PumasBackwardsGenerator();
#endif

  // If none of the previous results worked, lets check the dynamic loader
  G4VUserPrimaryGeneratorAction* gen = NULL;
  gen = DynamicObjectLoader::Get()->LoadDynamicFluxGenerator(type);
  if (gen) return gen;

  // Check if only made
  std::cout << "Generator Creation Failed!" << std::endl;
  throw;

  // Return NULL generator
  return 0;
}
} // - namespace COSMIC

