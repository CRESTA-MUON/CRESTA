#include "AWEScintillatorSD.hh"
#include "analysis/Analysis.hh"
#include "db/DBTable.hh"
#include "db/DB.hh"
#include "sd/ScintillatorHit.hh"

namespace COSMIC {

//------------------------------------------------------------------
AWEScintillatorSD::AWEScintillatorSD(DBTable tbl):
    VDetector(tbl.GetIndexName(), "awescintillator"), fHCID(-1)
{
    std::cout << "Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // By default also include the auto processor
    if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
        Analysis::Get()->RegisterProcessor(new AWEScintillatorProcessor(this));
    }

    collectionName.push_back(GetID());
}

void AWEScintillatorSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new ScintillatorHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4bool AWEScintillatorSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) {
        return false;
    }
    G4Track* track = step->GetTrack();

    // Get the step inside the detector
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4TouchableHistory* touchable = (G4TouchableHistory*)(preStepPoint->GetTouchable());

    // Get the position of the volume associated with the step
    G4ThreeVector volume_position = touchable->GetVolume()->GetTranslation() / m;

    // Get the hitTime
    G4double hitTime = preStepPoint->GetGlobalTime();

    ScintillatorHit* hit = new ScintillatorHit();
    hit->SetParticleType((int) track->GetParticleDefinition()->GetPDGEncoding());
    hit->SetEdep(edep);
    hit->SetTime(hitTime);
    hit->SetPos(preStepPoint->GetPosition());
    hit->SetVolPos(volume_position);
    hit->SetAngles(track->GetMomentumDirection());
    fHitsCollection->insert(hit);

    return true;
}


//------------------------------------------------------------------
AWEScintillatorProcessor::AWEScintillatorProcessor(AWEScintillatorSD* trkr, bool autosave) :
    VProcessor(trkr->GetID()), fSave(autosave)
{
    fDetector = trkr;
    fHCID = trkr->GetHCID();
}

bool AWEScintillatorProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    Reset();
    return true;
}

bool AWEScintillatorProcessor::ProcessEvent(const G4Event* event) {

    // Average over hits
    if (fHCID < 0) fHCID = fDetector->GetHCID();
    ScintillatorHitsCollection* hc = static_cast<ScintillatorHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));
    int nhits = (int)  (hc)->GetSize();
    if (nhits < 1) {
        return false;
    }
    // std::cout << "Got Some Hits" << std::endl;
    G4double T = (event->GetPrimaryVertex())->GetT0();

    for (int ihit = 0; ihit < nhits; ihit++) {

        fEdep += ( *(hc) )[ihit]->GetEdep() / MeV;
        fTime += (( ( *(hc) )[ihit]->GetTime() / ns) - T / ns);

        fPosX += ( *(hc) )[ihit]->GetPos()[0] / m;
        fPosY += ( *(hc) )[ihit]->GetPos()[1] / m;
        fPosZ += ( *(hc) )[ihit]->GetPos()[2] / m;

        fVolPosX += ( *(hc) )[ihit]->GetVolPos()[0] / m;
        fVolPosY += ( *(hc) )[ihit]->GetVolPos()[1] / m;
        fVolPosZ += ( *(hc) )[ihit]->GetVolPos()[2] / m;

        fThetaXZ += ( *(hc) )[ihit]->GetThetaXZ();
        fThetaYZ += ( *(hc) )[ihit]->GetThetaYZ();
    }

    // Now average
    fTime /= nhits + 0.;

    fPosX /= nhits + 0.;
    fPosY /= nhits + 0.;
    fPosZ /= nhits + 0.;

    fVolPosX /= nhits + 0.;
    fVolPosY /= nhits + 0.;
    fVolPosZ /= nhits + 0.;

    fThetaXZ /= nhits + 0.;
    fThetaYZ /= nhits + 0.;

    fPDG = (double) ( *(hc) )[0]->GetType();

    // Register Trigger State
    fHasInfo = fEdep > 0.0;
    fEnergy  = fEdep;

}

void AWEScintillatorProcessor::Reset() {
    VProcessor::Reset();
    fTime = 0.0;
    fEdep = 0.0;
    fPosX = 0.0;
    fPosY = 0.0;
    fPosZ = 0.0;
    fVolPosX = 0.0;
    fVolPosY = 0.0;
    fVolPosZ = 0.0;
    fPDG     = 0.0;
    fThetaXZ = 0.0;
    fThetaYZ = 0.0;
}


} // - namespace COSMIC
