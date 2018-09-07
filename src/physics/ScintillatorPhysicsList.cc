//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#include "ScintillatorPhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "Shielding.hh"
#include "G4HadronPhysicsShielding.hh"

#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmProcessOptions.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonQMDPhysics.hh"
#include "G4IonElasticPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronElasticPhysicsLEND.hh"

#include "G4DataQuestionaire.hh"
#include "G4HadronPhysicsShielding.hh"




ScintillatorPhysicsList::ScintillatorPhysicsList()
  : G4VModularPhysicsList() {

  // default cut value  (1.0mm)
  G4int verbose = 0;

  // General Physics
  // RegisterPhysics( new ScintillatorPhysics("general+EM+muon") );
  // RegisterPhysics( new LXeEMPhysics("standard EM"));
  // RegisterPhysics( new LXeMuonPhysics("muon"));



  this->defaultCutValue = 0.7 * CLHEP::mm;


  this->RegisterPhysics( new G4EmStandardPhysics(verbose));
  //G4EmProcessOptions emOptions;
  //emOptions.SetFluo(true); // To activate deexcitation processes and fluorescence
  //emOptions.SetAuger(true); // To activate Auger effect if deexcitation is activated
  //emOptions.SetPIXE(true); // To activate Particle Induced X-Ray Emission (PIXE)

  // Synchroton Radiation & GN Physics
  this->RegisterPhysics( new G4EmExtraPhysics(verbose) );

  // Decays
  this->RegisterPhysics( new G4DecayPhysics(verbose) );
  //if ( rad == true ) this->RegisterPhysics( new G4RadioactiveDecayPhysics(verbose) );
  this->RegisterPhysics( new G4RadioactiveDecayPhysics(verbose) );

  // Hadron Elastic scattering
  this->RegisterPhysics( new G4HadronElasticPhysicsHP(verbose) );


  // Hadron Physics
  G4HadronPhysicsShielding* hps = new G4HadronPhysicsShielding(verbose);
  this->RegisterPhysics( hps );

  //Activate prodcuton of fission fragments in neutronHP
  char env_ff[] = "G4NEUTRONHP_PRODUCE_FISSION_FRAGMENTS=1";
  putenv(env_ff);

  // Stopping Physics
  this->RegisterPhysics( new G4StoppingPhysics(verbose) );

  // Ion Physics
  this->RegisterPhysics( new G4IonQMDPhysics(verbose));

  this->RegisterPhysics( new G4IonElasticPhysics(verbose));


  // Optical Physics
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  RegisterPhysics( opticalPhysics );

  opticalPhysics->SetWLSTimeProfile("delta");

  opticalPhysics->SetScintillationYieldFactor(1.0);
  opticalPhysics->SetScintillationExcitationRatio(1.0);

  opticalPhysics->SetMaxNumPhotonsPerStep(100);
  opticalPhysics->SetMaxBetaChangePerStep(10.0);

  opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
  opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);

}


ScintillatorPhysicsList::~ScintillatorPhysicsList()
{
}


void ScintillatorPhysicsList::SetCuts()
{
  // The method SetCuts() is mandatory in the interface. Here, one just use
  // the default SetCuts() provided by the base class.
  SetCutsWithDefault();
  this->SetCutValue(0, "proton");
}
