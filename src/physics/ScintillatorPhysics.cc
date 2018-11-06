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
//
// $Id: ScintillatorPhysics.cc 100259 2016-10-17 08:02:30Z gcosmo $
//
/// \file optical/LXe/src/ScintillatorPhysics.cc
/// \brief Implementation of the ScintillatorPhysics class
//
//
#include "ScintillatorPhysics.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>
#include "G4Decay.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintillatorPhysics::ScintillatorPhysics(const G4String& name)
  :  G4VPhysicsConstructor(name) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintillatorPhysics::~ScintillatorPhysics() {
  //fDecayProcess = NULL;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"


#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4PionZero.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"

#include "G4GenericIon.hh"

#include "G4Proton.hh"


#include "G4Gamma.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"

void ScintillatorPhysics::ConstructParticle()
{
    // gamma
  G4Gamma::GammaDefinition();
 
  // electron
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();

  // Mu
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
  //These are needed for the mu- capture
  G4Neutron::Neutron();
  G4Proton::Proton();
  G4PionMinus::PionMinus();
  G4PionZero::PionZero();
  G4PionPlus::PionPlus();

  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();

  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ScintillatorPhysics::ConstructProcess()
{

  G4Decay* fDecayProcess = new G4Decay();

  // Add Decay Process
  auto particleIterator = GetParticleIterator();
  particleIterator->reset();
  while ( (*particleIterator)() ) {
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (fDecayProcess->IsApplicable(*particle)) {
      pmanager ->AddProcess(fDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);
    }
  }

  G4MuIonisation* fMuPlusIonisation =
    new G4MuIonisation();
  G4MuMultipleScattering* fMuPlusMultipleScattering =
    new G4MuMultipleScattering();
  G4MuBremsstrahlung* fMuPlusBremsstrahlung=
    new G4MuBremsstrahlung();
  G4MuPairProduction* fMuPlusPairProduction=
    new G4MuPairProduction();

  G4MuIonisation* fMuMinusIonisation =
    new G4MuIonisation();
  G4MuMultipleScattering* fMuMinusMultipleScattering =
    new G4MuMultipleScattering();
  G4MuBremsstrahlung* fMuMinusBremsstrahlung =
    new G4MuBremsstrahlung();
  G4MuPairProduction* fMuMinusPairProduction =
    new G4MuPairProduction();

  G4MuonMinusCapture* fMuMinusCaptureAtRest =
    new G4MuonMinusCapture();

  G4ProcessManager * pManager = 0;

  // Muon Plus Physics
  pManager = G4MuonPlus::MuonPlus()->GetProcessManager();

  pManager->AddProcess(fMuPlusMultipleScattering,-1,  1, 1);
  pManager->AddProcess(fMuPlusIonisation,        -1,  2, 2);
  pManager->AddProcess(fMuPlusBremsstrahlung,    -1,  3, 3);
  pManager->AddProcess(fMuPlusPairProduction,    -1,  4, 4);

  // Muon Minus Physics
  pManager = G4MuonMinus::MuonMinus()->GetProcessManager();

  pManager->AddProcess(fMuMinusMultipleScattering,-1,  1, 1);
  pManager->AddProcess(fMuMinusIonisation,        -1,  2, 2);
  pManager->AddProcess(fMuMinusBremsstrahlung,    -1,  3, 3);
  pManager->AddProcess(fMuMinusPairProduction,    -1,  4, 4);

  pManager->AddRestProcess(fMuMinusCaptureAtRest);

  G4PhotoElectricEffect* fPhotoEffect =
    new G4PhotoElectricEffect();
  G4ComptonScattering* fComptonEffect =
    new G4ComptonScattering();
  G4GammaConversion* fPairProduction =
    new G4GammaConversion();
 
    // Electron physics
  G4eMultipleScattering* fElectronMultipleScattering =
    new G4eMultipleScattering();
  G4eIonisation* fElectronIonisation =
    new G4eIonisation();
  G4eBremsstrahlung* fElectronBremsStrahlung =
    new G4eBremsstrahlung();
 
    //Positron physics
  G4eMultipleScattering* fPositronMultipleScattering =
    new G4eMultipleScattering();
  G4eIonisation* fPositronIonisation =
    new G4eIonisation();
  G4eBremsstrahlung* fPositronBremsStrahlung =
    new G4eBremsstrahlung();
  G4eplusAnnihilation* fAnnihilation =
    new G4eplusAnnihilation();


  // Gamma Physics
  pManager = G4Gamma::Gamma()->GetProcessManager();
  pManager->AddDiscreteProcess(fPhotoEffect);
  pManager->AddDiscreteProcess(fComptonEffect);
  pManager->AddDiscreteProcess(fPairProduction);

  // Electron Physics
  pManager = G4Electron::Electron()->GetProcessManager();

  pManager->AddProcess(fElectronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fElectronIonisation,         -1, 2, 2);
  pManager->AddProcess(fElectronBremsStrahlung,     -1, 3, 3);  

  //Positron Physics
  pManager = G4Positron::Positron()->GetProcessManager();
 
  pManager->AddProcess(fPositronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(fPositronIonisation,         -1, 2, 2);
  pManager->AddProcess(fPositronBremsStrahlung,     -1, 3, 3);  
  pManager->AddProcess(fAnnihilation,                0,-1, 4); 
}
