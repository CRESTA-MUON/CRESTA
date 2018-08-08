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
#include "action/CosmicRunAction.hh"

// G4 Headers
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"
#include "G4UnitsTable.hh"

// Cosmic Headers
#include "analysis/Analysis.hh"
#include "action/CosmicRun.hh"
#include "sd/DetectorManager.hh"
#include "sd/DetectorManager.hh"

// namespace COSMIC
using namespace COSMIC;

CosmicRunAction::CosmicRunAction() : G4UserRunAction(), fCurrentRun(0)
{
}

CosmicRunAction::~CosmicRunAction()
{
  delete G4AnalysisManager::Instance();
}

G4Run* CosmicRunAction::GenerateRun()
{
  return new CosmicRun;
}

void CosmicRunAction::BeginOfRunAction(const G4Run* run)
{
  // Logging Info
  std::cout << "=========================================" << std::endl;
  std::cout << "ACT: Beginning Run : " << fCurrentRun << std::endl;

  // Do start analysis processing
  Analysis::Get()->BeginOfRunAction(run);
}

void CosmicRunAction::EndOfRunAction(const G4Run* run)
{
  // Do any run processing
  Analysis::Get()->EndOfRunAction(run);

  // Add to the current counters
  fCurrentRun++;
  Analysis::Get()->IncrementSubRun();

  // Logging Info
  std::cout << "ACT: Finished Run. "
            << "Events : " << Analysis::Get()->GetNEvents()
            << ", Triggered : " << Analysis::Get()->GetNSavedEvents()
            << ", Exposure : " << Analysis::Get()->GetExposureTime() << " s" << std::endl;
  std::cout << "=========================================" << std::endl;

  // Check exposure/trigger limits
  Analysis::Get()->CheckAbortState();
}
