#ifdef __USE_PUMAS__
//******************************************************************************
// PumasBackwardsGenerator.cc
//
// 1.00 JMV, LLNL, Jan-2007:  First version.
//******************************************************************************
//
#include "PumasBackwardsGenerator.hh"
#include "db/DB.hh"
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "pumas/PUMASInterface.hh"
/* Standard library includes */
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

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
#include "db/ROOTHeaders.hh"
#include "G4Polyline.hh"

#include "pumas/pumas.h"
using namespace PUMASInterface;

namespace COSMIC {

//----------------------------------------------------------------------------//
PumasBackwardsGenerator::PumasBackwardsGenerator()
{

  fSetupState = false;
  fKineticThreshold = 5E6;

  G4int nofParticles = 1;
  fParticleGun  = new G4ParticleGun(nofParticles);

  // Set exposure and nthrows
  fExposureTime = 0.0;
  fNThrows = 0;
  // Make a new processor
  // Analysis::Get()->SetFluxProcessor(new PumasBackwardFluxProcessor(this));
  Analysis::Get()->SetFluxProcessor(new PumasBackwardsProcessor(this));
}

//----------------------------------------------------------------------------//
PumasBackwardsGenerator::~PumasBackwardsGenerator()
{
}

void PumasBackwardsGenerator::Setup() {

  // Get a histogram of densities
  TH2D* histXY = NULL;
  TH2D* histXZ = NULL;
  TH2D* histYZ = NULL;

  // Build the PUMAS Context
  context = PUMASInterface::BuildPUMAS(histXY, histXZ, histYZ);
  context->scheme = PUMAS_SCHEME_HYBRID;
  context->longitudinal = 0;
  context->kinetic_limit = fKineticThreshold;  
  context->distance_max = 500.0;

  std::cout << "Built context : " << context << std::endl;
  fSetupState = true;
}


void PumasBackwardsGenerator::GeneratePrimaries(G4Event* anEvent)
{
  if (!fSetupState) Setup();

  // Throw an angle and an energy;
  double theta = +0.25 * CLHEP::twopi + 0.5 * CLHEP::twopi * PUMASInterface::uniform01(context);
  double phi = CLHEP::twopi * PUMASInterface::uniform01(context);

  double cos_theta = cos(theta);
  double sin_theta = sin(theta);
  double cos_phi   = cos(phi);
  double sin_phi   = sin(phi);
  double kf = 210;
  double wf = 1;

    // G4double cos_theta = fZenithPDF->GetRandom();
    // G4double phi = CLHEP::twopi * G4UniformRand(); //phi uniform in [-pi, pi];
    // G4double sin_theta = std::sqrt(1 - cos_theta * cos_theta);
    // G4double x = sin_theta * cos(phi);
    // G4double y = sin_theta * sin(phi);
    // G4double z = cos_theta;
    // return G4ThreeVector(x, y, -z);

  // Update the PUMAS_STATE
  struct pumas_state state = {
    .charge  = -1.,
    .kinetic = kf,
    .distance = 0.0,
    .grammage = 0.0,
    .time = 0.0,
    .weight  = wf,
    .position = {0.0, 0.0, 0.0},
    .direction = { sin_theta * cos_phi, sin_theta * sin_phi, cos_theta }
  };

  // Update vectors
  fMuonPos    = G4ThreeVector(0, 0, 0);
  fMuonDir    = G4ThreeVector(-sin_theta, 0., -cos_theta);
  fMuonEnergy = kf * MeV;
  fMuonTime   = 1 * ns;


  std::vector<double> posX;
  std::vector<double> posY;
  std::vector<double> posZ;


  // While the muon is less than some threshold, project backwards
  // while (state.kinetic < fKineticThreshold - FLT_EPSILON) {

  // std::cout << "While " << state.kinetic << " " << state.weight << " " << state.direction[0] << " " << state.direction[1] << " " << state.direction[2] << std::endl;

  posX.push_back( state.position[0]*m );
  posY.push_back( state.position[1]*m );
  posZ.push_back( state.position[2]*m );

  // Transport a step
  pumas_transport(context, &state);

  // Save position of the state
  posX.push_back( state.position[0]*m );
  posY.push_back( state.position[1]*m );
  posZ.push_back( state.position[2]*m );

  double val = medium2(context, &state, NULL);
  // std::cout << "Checking position : " << val << std::endl;
  // if (val <= 0) {
  // std::cout << "Breaking" << std::endl;
  // break;
  // }
  // }
  const double wi = state.weight *
                    spectrum_gaisser(-state.direction[2], state.kinetic);

  // G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  // if (pVVisManager) {
  //   G4Polyline polyline;
  //   G4Colour colour(1., 1., 0.);
  //   G4VisAttributes attribs(colour);

  //   polyline.SetVisAttributes(attribs);
  //   for (int i = 0; i < posX.size(); i++) {

  //     polyline.push_back( G4ThreeVector(posX[i], posY[i], posZ[i]));
  //   }
  //   pVVisManager->Draw(polyline);
  // }



  // Get a weight for the muon depending on its angle and momentum.

  // std::cout << "Particle Weight : " << wi << std::endl;
  // std::cout << "Final particle pos : " << state.position[0] << " "  << state.position[1] << " " << state.position[2] << std::endl;
  // std::cout << "Final particle dir : " << state.direction[0] << " "  << state.direction[1] << " " << state.direction[2] << std::endl;

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));
  fParticleGun->SetParticleEnergy(fMuonEnergy);
  fParticleGun->SetParticleTime(fMuonTime);
  fParticleGun->SetParticleMomentumDirection(fMuonDir);
  fParticleGun->SetParticlePosition(fMuonPos);
  fParticleGun->GeneratePrimaryVertex(anEvent);


}
//------------------------------------------------------------------



//------------------------------------------------------------------
PumasBackwardsProcessor::PumasBackwardsProcessor(PumasBackwardsGenerator * gen, bool autosave) :
  VFluxProcessor("pumas")
{
  fGenerator = gen;
}

bool PumasBackwardsProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  return true;
}

bool PumasBackwardsProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Register Trigger State
  fHasInfo = true;
  fTime    = 1.0;

  return true;
}

} // - namespace COSMIC
#endif // __USE_PUMAS__
