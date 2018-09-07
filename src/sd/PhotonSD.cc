#include "PhotonSD.hh"
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
#include "tomography/PoCAUtils.hh"
#include "G4OpticalPhoton.hh"
#include "PhotonHit.hh"

namespace COSMIC {

//------------------------------------------------------------------
PhotonSD::PhotonSD(DBTable tbl):
  VDetector(tbl.GetIndexName(), "photonsd"), fHCID(-1)
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
    Analysis::Get()->RegisterProcessor(new PhotonProcessor(this));
  }

  collectionName.push_back(GetID());

}


void PhotonSD::ResetState() {
  VDetector::ResetState();
}

G4bool PhotonSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {
  return false;
}

G4TrackStatus PhotonSD::ManuallyProcessHits(const G4Step* step, G4TouchableHistory* touch) {

  // Require a photon
  if (step->GetTrack()->GetDefinition()
      != G4OpticalPhoton::OpticalPhotonDefinition()) return step->GetTrack()->GetTrackStatus();

  PhotonHit* hit = new PhotonHit();
  hit->SetTime(step->GetPostStepPoint()->GetGlobalTime() / ns);
  hit->SetEnergy(step->GetPostStepPoint()->GetKineticEnergy());
  hit->SetPosition(step->GetPostStepPoint()->GetPosition());
  fHitsCollection->insert(hit);

  return fStopAndKill;
};

void PhotonSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection = new PhotonHitsCollection(SensitiveDetectorName, collectionName[0]);
  if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
  hce->AddHitsCollection(fHCID, fHitsCollection);
}


//------------------------------------------------------------------
PhotonProcessor::PhotonProcessor(PhotonSD* trkr, bool autosave) :
  VProcessor(trkr->GetID())
{
  fDetector = trkr;
}

bool PhotonProcessor::BeginOfRunAction(const G4Run* /*run*/) {

// Each of these should be vectors
  std::string tableindex = GetID();
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->CreateNtupleDColumn(tableindex + "_E", fPhotonE);
  man->CreateNtupleDColumn(tableindex + "_t", fPhotonT);
  Reset();

  return true;
}

bool PhotonProcessor::ProcessEvent(const G4Event* event) {

  if (fHCID < 0) fHCID = fDetector->GetHCID();
  PhotonHitsCollection* hc = static_cast<PhotonHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));
  if (!hc) return false;
  int nhits = (int) (hc)->GetSize();
  if (nhits < 1) {
    return false;
  }

  fPhotonE.clear();
  fPhotonT.clear();
  for (int i = 0; i < nhits; i++) {
    PhotonHit* hit = ( *(hc) )[i];
    // if (i % nhits/50 == 0) hit->Draw();
    fPhotonE.push_back(hit->GetEnergy());
    fPhotonT.push_back(hit->GetTime());
  }

  fHasInfo = true;
  return fHasInfo;
}

} // - namespace COSMIC
