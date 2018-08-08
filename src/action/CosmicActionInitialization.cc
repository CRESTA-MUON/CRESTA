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
#include "action/CosmicActionInitialization.hh"

// Cosmic Headers
#include "action/CosmicRunAction.hh"
#include "action/CosmicStackingAction.hh"
#include "flux/PrimaryGeneratorFactory.hh"
#include "physics/PhysicsFactory.hh"
#include "sd/DetectorConstruction.hh"

// namespace COSMIC
using namespace COSMIC;

CosmicActionInitialization::CosmicActionInitialization()
  : G4VUserActionInitialization()
{}

CosmicActionInitialization::~CosmicActionInitialization()
{}

void CosmicActionInitialization::BuildForMaster() const
{
  // Define our standard cosmic actions
  SetUserAction(new CosmicRunAction);
}

void CosmicActionInitialization::Build() const
{
  // Build extra with generator and stacker for slave nodes
  SetUserAction(PrimaryGeneratorFactory::LoadGenerator());
  SetUserAction(new CosmicRunAction);
  SetUserAction(new CosmicStackingAction);
}

