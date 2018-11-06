#include "TrueProtonTracker.hh"
#include "analysis/Analysis.hh"
#include "db/DBTable.hh"


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
#include "analysis/Analysis.hh"


namespace CRESTA {

//------------------------------------------------------------------
TrueProtonTracker::TrueProtonTracker(DBTable tbl):
  VDetector(tbl.GetIndexName(), "trueproton")
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
    Analysis::Get()->RegisterProcessor(new TrueProtonProcessor(this));
  }
}

TrueProtonTracker::TrueProtonTracker(std::string name, std::string id, bool autoprocess, bool autosave):
  VDetector(name, id)
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (autoprocess) {
    Analysis::Get()->RegisterProcessor(new TrueProtonProcessor(this, autosave));
  }
}

void TrueProtonTracker::ResetState() {
  VDetector::ResetState();
  fProtonTime = 0.0;
  fProtonMom = G4ThreeVector();
  fProtonPos = G4ThreeVector();
  fProtonPDG = -999;
  fTotEDep = 0.0;
  fNProtonHits = 0;
}

G4bool TrueProtonTracker::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

  // // Don't save tracks if no energy left in the detector
  // G4double edep = step->GetTotalEnergyDeposit();
  // fTotEDep += edep;
  // if (edep <= 0.) return false;

  // Get only protons
  G4Track* track = step->GetTrack();
  int steppdg = track->GetParticleDefinition()->GetPDGEncoding();
  bool is_proton = (steppdg == 2212);
  if (!is_proton) return true;

  // Get the step+1 inside the detector
  G4StepPoint* steppoint = step->GetPostStepPoint();
  G4double steptime = steppoint->GetGlobalTime();
  G4ThreeVector steppos = steppoint->GetPosition();
  G4ThreeVector stepmom = track->GetMomentum();

  // Very inefficient lazy averaging
  fProtonPos  *= fNProtonHits;
  fProtonMom  *= fNProtonHits;
  fProtonTime *= fNProtonHits;
  fProtonPDG  *= fNProtonHits;

  fProtonPos  += steppos;
  fProtonMom  += stepmom;
  fProtonTime += steptime;
  fProtonPDG  += steppdg;

  // Get new average
  fNProtonHits++;
  fProtonPos  /= fNProtonHits;
  fProtonMom  /= fNProtonHits;
  fProtonTime /= fNProtonHits;
  fProtonPDG  /= fNProtonHits;

  return true;
}


//------------------------------------------------------------------
TrueProtonProcessor::TrueProtonProcessor(TrueProtonTracker* trkr, bool autosave) :
  VProcessor(trkr->GetID()), fSave(autosave)
{
  fTracker = trkr;
}

bool TrueProtonProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = GetID();
  std::cout << "DET: Registering TrueProtonProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fProtonTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
  fProtonMomXIndex = man ->CreateNtupleDColumn(tableindex + "_px");
  fProtonMomYIndex = man ->CreateNtupleDColumn(tableindex + "_py");
  fProtonMomZIndex = man ->CreateNtupleDColumn(tableindex + "_pz");
  fProtonPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
  fProtonPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
  fProtonPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");
  fProtonPDGIndex  = man ->CreateNtupleIColumn(tableindex + "_pdg");

  return true;
}

bool TrueProtonProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Register Trigger State
  fHasInfo = fTracker->GetProtonPDG() != -999;
  fTime    = fTracker->GetProtonTime();
  fEnergy  = fTracker->GetTotEDep();

  if (fHasInfo) {
    // Fill proton vectors
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fProtonTimeIndex, fTracker->GetProtonTime());
    man->FillNtupleDColumn(fProtonMomXIndex, fTracker->GetProtonMom().x() / MeV);
    man->FillNtupleDColumn(fProtonMomYIndex, fTracker->GetProtonMom().y() / MeV);
    man->FillNtupleDColumn(fProtonMomZIndex, fTracker->GetProtonMom().z() / MeV);
    man->FillNtupleDColumn(fProtonPosXIndex, fTracker->GetProtonPos().x() / m);
    man->FillNtupleDColumn(fProtonPosYIndex, fTracker->GetProtonPos().y() / m);
    man->FillNtupleDColumn(fProtonPosZIndex, fTracker->GetProtonPos().z() / m);
    man->FillNtupleIColumn(fProtonPDGIndex , fTracker->GetProtonPDG());

    //    TrueProtonProcessor::DrawEvent();

    return true;
  } else {
    // Set Ntuple to defaults
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fProtonTimeIndex, -999.);
    man->FillNtupleDColumn(fProtonMomXIndex, -999.);
    man->FillNtupleDColumn(fProtonMomYIndex, -999.);
    man->FillNtupleDColumn(fProtonMomZIndex, -999.);
    man->FillNtupleDColumn(fProtonPosXIndex, -999.);
    man->FillNtupleDColumn(fProtonPosYIndex, -999.);
    man->FillNtupleDColumn(fProtonPosZIndex, -999.);
    man->FillNtupleIColumn(fProtonPDGIndex,  fTracker->GetProtonPDG() );
    return false;
  }
}

void TrueProtonProcessor::DrawEvent(){
}

} // - namespace CRESTA
