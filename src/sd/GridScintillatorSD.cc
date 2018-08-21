#include "GridScintillatorSD.hh"
#include "analysis/Analysis.hh"
#include "db/DBTable.hh"
#include "db/DB.hh"
#include "ScintillatorHit.hh"

namespace COSMIC {

//------------------------------------------------------------------
GridScintillatorSD::GridScintillatorSD(DBTable tbl):
    VDetector(tbl.GetIndexName(), "scintillator"), fHCID(-1)
{
    std::cout << "Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // Get the efficiency
    fEfficiency = 1.0;
    if (tbl.Has("efficiency")) {
        fEfficiency = tbl.GetD("efficiency");
        std::cout << "TRG: Setting Efficiency : " << fEfficiency << std::endl;
    }

    // By default also include the auto processor
    if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
        Analysis::Get()->RegisterProcessor(new GridScintillatorProcessor(this));
    }

    collectionName.push_back(GetID());
}

GridScintillatorSD::GridScintillatorSD(std::string name, std::string id,
        bool autoprocess, bool autosave):
    VDetector(name, id)
{
    std::cout << "Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // By default also include the auto processor
    if (autoprocess) {
        Analysis::Get()->RegisterProcessor(new GridScintillatorProcessor(this, autosave));
    }

    collectionName.push_back(GetID());
}

void GridScintillatorSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new ScintillatorHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4bool GridScintillatorSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) {
        return false;
    }

    if (fEfficiency != 1.0) {
        G4double r = G4UniformRand();
        if (r > fEfficiency) return false;
    }


    G4Track* track = step->GetTrack();

    // Get the step inside the detector
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4TouchableHistory* touchable = (G4TouchableHistory*)(preStepPoint->GetTouchable());

    // Get the position of the volume associated with the step
    G4ThreeVector volume_position = touchable->GetVolume()->GetTranslation() / m;
    G4int this_id = touchable->GetVolume()->GetCopyNo();

    // std::cout << "Vol: " << volume_position << std::endl;

    // Get the hitTime
    G4double hitTime = preStepPoint->GetGlobalTime();

    ScintillatorHit* hit = new ScintillatorHit();
    hit->SetParticleType((int) track->GetParticleDefinition()->GetPDGEncoding());
    hit->SetEdep(edep);
    hit->SetTime(hitTime);
    hit->SetID(1);
    hit->SetPos(preStepPoint->GetPosition());
    hit->SetVolPos(volume_position);
    hit->SetAngles(track->GetMomentumDirection());
    fHitsCollection->insert(hit);

    return true;
}


//------------------------------------------------------------------
GridScintillatorProcessor::GridScintillatorProcessor(GridScintillatorSD* trkr, bool autosave) :
    VProcessor(trkr->GetID()), fSave(autosave)
{
    fDetector = trkr;
    fHCID = trkr->GetHCID();
}

bool GridScintillatorProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  // Each of these should be vectors
  std::string tableindex = GetID();
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->CreateNtupleFColumn(tableindex + "_E", fVoxelE);
  man->CreateNtupleFColumn(tableindex + "_x", fVoxelX);
  man->CreateNtupleFColumn(tableindex + "_y", fVoxelY);
  man->CreateNtupleFColumn(tableindex + "_z", fVoxelZ);
  man->CreateNtupleFColumn(tableindex + "_t", fVoxelT);
  man->CreateNtupleIColumn(tableindex + "_pdg", fVoxelP);
  Reset();
  return true;
}

bool GridScintillatorProcessor::ProcessEvent(const G4Event* event) {

    // Average over hits
    if (fHCID < 0) fHCID = fDetector->GetHCID();
    ScintillatorHitsCollection* hc = static_cast<ScintillatorHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));
    int nhits = (int)  (hc)->GetSize();
    if (nhits < 1) {
        return false;
    }
    
    // We don't average over hits, instead we take each point and convert it into a voxel
    G4double T = (event->GetPrimaryVertex())->GetT0();
    double Edep = 0.0;
    for (int ihit = 0; ihit < nhits; ihit++) {      
      Edep += (*hc)[ihit]->GetEdep();
      fVoxelE.push_back( (*hc)[ihit]->GetEdep() / MeV );
      fVoxelX.push_back( ( *(hc) )[ihit]->GetPos()[0] / mm );
      fVoxelY.push_back( ( *(hc) )[ihit]->GetPos()[1] / mm );
      fVoxelZ.push_back( ( *(hc) )[ihit]->GetPos()[2] / mm );
      fVoxelT.push_back(  (( ( *(hc) )[ihit]->GetTime() / ns) - T / ns) );
      fVoxelP.push_back( ( *(hc) )[ihit]->GetType() );
    }

    // Register Trigger State
    fHasInfo = Edep > 0.0;
    fEnergy  = Edep;
    if (!fSave) return false;
    return fHasInfo;
}

void GridScintillatorProcessor::Reset() {
  VProcessor::Reset();
  fVoxelE.clear();
  fVoxelX.clear();
  fVoxelY.clear();
  fVoxelZ.clear();
  fVoxelT.clear();
  fVoxelP.clear();
}


} // - namespace COSMIC
