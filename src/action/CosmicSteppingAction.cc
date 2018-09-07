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
// $Id: CosmicSteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file CosmicSteppingAction.cc
/// \brief Implementation of the CosmicSteppingAction class

#include "CosmicSteppingAction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "analysis/VDetector.hh"

using namespace COSMIC;

CosmicSteppingAction::CosmicSteppingAction()
  : G4UserSteppingAction()
{}

CosmicSteppingAction::~CosmicSteppingAction()
{}

void CosmicSteppingAction::UserSteppingAction(const G4Step* step)
{
  // If there is a step for the current volume, check if it
  G4StepPoint* posstep = step->GetPostStepPoint();
  G4VSensitiveDetector* postsd = posstep->GetSensitiveDetector();
  VDetector* det = NULL;
  if (postsd) {
    det = static_cast<VDetector*>(postsd);
  }
  if (det) {
      G4Track* track = step->GetTrack();
    G4TrackStatus trackstatus = det->ManuallyProcessHits(step, NULL);
    if (trackstatus != track->GetTrackStatus()) {
      track->SetTrackStatus(trackstatus);
    }
  }

  return;
}

