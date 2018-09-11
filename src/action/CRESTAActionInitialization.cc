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
#include "action/CRESTAActionInitialization.hh"

// CRESTA Headers
#include "action/CRESTARunAction.hh"
#include "action/CRESTAStackingAction.hh"
#include "action/CRESTASteppingAction.hh"
#include "flux/FluxFactory.hh"
#include "physics/PhysicsFactory.hh"
#include "sd/DetectorConstruction.hh"

// namespace COSMIC
using namespace COSMIC;

CRESTAActionInitialization::CRESTAActionInitialization()
  : G4VUserActionInitialization()
{}

CRESTAActionInitialization::~CRESTAActionInitialization()
{}

void CRESTAActionInitialization::BuildForMaster() const
{
  // Set run action from factory
  G4UserRunAction* run = new CRESTARunAction();
  if (run) SetUserAction(run);
  else {
  	std::cout << "No run loaded in master!" << std::endl;
  	throw;
  }
}

void CRESTAActionInitialization::Build() const
{
  // Set flux action from factory
  G4VUserPrimaryGeneratorAction* gen = FluxFactory::LoadFluxGenerator();
  if (gen) { SetUserAction(gen); }
  else {
  	std::cout << "No generator loaded in master!" << std::endl;
  	throw;
  }

  // Set run action from factory
  G4UserRunAction* run = new CRESTARunAction();
  if (run) { SetUserAction(run); }
  else {
  	std::cout << "No run loaded in slave!" << std::endl;
  	throw;
  }

  // Set event action from factory
  G4UserEventAction* event = NULL;
  if (event) { SetUserAction(event); }

  // Set stacking action from factory
  G4UserStackingAction* stack = new CRESTAStackingAction();
  if (stack) { SetUserAction(stack); }

  // Set tracking action from factory
  //  G4UserTrackingAction* track = NULL;
  //  if (track) { SetUserAction(track); }

  // Set stepping action from factory
  //  G4UserSteppingAction* step = new CRESTASteppingAction(); 
  //  if (step) { SetUserAction(step); }

}

