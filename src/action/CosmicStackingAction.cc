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
#include "action/CosmicStackingAction.hh"

// G4 Headers
#include "G4Track.hh"
#include "G4NeutrinoE.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

// namespace COSMIC
using namespace COSMIC;

CosmicStackingAction::CosmicStackingAction() {
}

CosmicStackingAction::~CosmicStackingAction() {
}

G4ClassificationOfNewTrack CosmicStackingAction::ClassifyNewTrack(const G4Track* track)
{

  // kill secondary neutrino
  if (track->GetParentID() > 0) {
    if (track->GetDefinition() == G4NeutrinoE::NeutrinoE() ||
        track->GetDefinition() == G4Electron::Electron() ||
        track->GetDefinition() == G4Positron::Positron() || 
        track->GetDefinition() == G4Gamma::Gamma()) {
      return fKill;
    }
  }
  if (track->GetDefinition() == G4Gamma::Gamma()) return fKill;

  // otherwise, return what Geant4 would have returned by itself
  return G4UserStackingAction::ClassifyNewTrack(track);

}
