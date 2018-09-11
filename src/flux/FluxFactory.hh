#ifndef __COSMIC_PrimaryGeneratorFactory_HH__
#define __COSMIC_PrimaryGeneratorFactory_HH__

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Types.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "db/DBTable.hh"

namespace COSMIC{

/// Flux Factory function to load generators from table
namespace FluxFactory {
  G4VUserPrimaryGeneratorAction* LoadFluxGenerator();
  G4VUserPrimaryGeneratorAction* LoadFluxGenerator(DBTable table);
}

} // - namespace COSMIC
#endif


