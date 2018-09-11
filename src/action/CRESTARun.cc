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
#include "action/CRESTARun.hh"

// System Headers
#include <ctime>
#include <math.h>
#include <stdio.h>

// G4 Headers
#include "globals.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4THitsMap.hh"

// CRESTA Headers
#include "analysis/Analysis.hh"
#include "geo/GeoManager.hh"
#include "sd/DetectorFactory.hh"
#include "trigger/TriggerFactory.hh"

// namespace COSMIC
using namespace COSMIC;

CRESTARun::CRESTARun()
  : G4Run()
{
  // Start a counter for this run
  Analysis::Get()->StartTheClock();
}

CRESTARun::~CRESTARun()
{
}

void CRESTARun::RecordEvent(const G4Event* event)
{

  // Print progress
  int eventid = event->GetEventID();
  Analysis::Get()->PrintProgress(eventid, numberOfEventToBeProcessed);

  // Do all processing loops
  Analysis::Get()->ProcessEvent(event);

  // Reset analysis state for next event.
  Analysis::Get()->BeginOfEventAction();

  // Check Abort State
  Analysis::Get()->CheckAbortState();

}

void CRESTARun::Merge(const G4Run* aRun)
{
  // Uncomment this if local run processing needed at a later date
  // const CRESTARun* localRun = static_cast<const CRESTARun*>(aRun);

  // Merge the normal runs
  G4Run::Merge(aRun);
}
