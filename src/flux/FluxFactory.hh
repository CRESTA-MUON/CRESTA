#ifndef __CRESTA_PrimaryGeneratorFactory_HH__
#define __CRESTA_PrimaryGeneratorFactory_HH__

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Types.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "db/DBTable.hh"

namespace CRESTA{

/// Flux Factory function to load generators from table
namespace FluxFactory {
  G4VUserPrimaryGeneratorAction* LoadFluxGenerator();
  G4VUserPrimaryGeneratorAction* LoadFluxGenerator(DBTable table);
}

} // - namespace CRESTA
#endif


