#include "PrimaryGeneratorCos2.hh"
#include "db/DB.hh"

namespace CRESTA {

//---------------------------------------------------------------------------------
PrimaryGeneratorCos2::PrimaryGeneratorCos2()
    : G4VUserPrimaryGeneratorAction(),
      fTime(0)
{
    std::cout << "FLX: Building Cos2 Source Generator" << std::endl;

    // Setup Table
    DBTable table = DB::Get()->GetTable("cos2", "config");

    // Setup Defaults + Table Inputs
    fMinEnergy = table.Has("min_energy") ? table.GetD("min_energy") : 1e-8 * MeV; // MeV
    fMaxEnergy = table.Has("max_energy") ? table.GetD("max_energy") : 1e4 * MeV;  //MeV

    fMinTheta = table.Has("min_theta") ? table.GetD("min_theta") : 0.0; // radians
    fMaxTheta = table.Has("max_theta") ? table.GetD("max_theta") : 2 * CLHEP::twopi; // radians

    fMinPhi = table.Has("min_phi") ? table.GetD("min_phi") : +1 * CLHEP::twopi; // radians
    fMaxPhi = table.Has("max_phi") ? table.GetD("max_phi") : +1 * CLHEP::twopi; // radians

    fRate = table.Has("rate") ? table.GetD("rate") : 1.0; // Hz / m-2

    // Setup Angular PDF
    fThetaPDF = new TF1("ThetaPDF","cos(x) * cos(x)", fMinTheta, fMaxTheta);

    // Setup Particle Gun
    std::cout << "FLX: --> Creating Particle Gun." << std::endl;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    fParticleGun->SetParticleDefinition(particleTable->FindParticle(table.GetS("particle")));

    // Now setup the particle integrals and source/target boxes
    fSourceBox = false;
    GetSourceBox();
    GetTargetBoxes();

    // Make a new processor
    Analysis::Get()->SetFluxProcessor(new Cos2PrimaryFluxProcessor(this));

    std::cout << "FLX: --> Complete." << std::endl;
}


PrimaryGeneratorCos2::~PrimaryGeneratorCos2()
{
    delete fParticleGun;
}

void PrimaryGeneratorCos2::GetSourceBox() {

    if (fSourceBox) return;
    std::cout << "FLX: --> Creating Source box" << std::endl;

    // Get table. This is required
    DBTable tbl = DB::Get()->GetTable("FLUX", "source_box");

    // Read size
    std::vector<G4double> size = tbl.GetVecG4D("size");
    fSourceBoxWidth    = G4ThreeVector(0.5 * size[0], 0.5 * size[1], 0.0);

    // Read position
    std::vector<G4double> pos  = tbl.GetVecG4D("position");
    fSourceBoxPosition = G4ThreeVector(pos[0], pos[1], pos[2]);

    // Set flag
    fSourceBox = true;
}

void PrimaryGeneratorCos2::GetTargetBoxes() {

    if (fCheckTargetBoxes) return;
    std::cout << "FLX: --> Creating Target boxes" << std::endl;

    // If none set then make it
    std::vector<DBTable> targetlinks = DB::Get()->GetTableGroup("FLUX");
    for (uint i = 0; i < targetlinks.size(); i++) {
        DBTable tbl = targetlinks[i];

        // Select tables with target box names
        std::string index = tbl.GetIndexName();
        if (index.find("target_box_") == std::string::npos) continue;

        // If it has position and size we can use it
        if (!tbl.Has("position") || !tbl.Has("size")) {
            std::cout << "Failed to find/create target box!" << std::endl;
            throw;
        }

        // Create size object
        std::vector<G4double> size = tbl.GetVecG4D("size");
        G4Box* box_sol = new G4Box(index, 0.5 * size[0], 0.5 * size[1], 0.5 * size[2]);

        // Create pos object
        std::vector<G4double> pos  = tbl.GetVecG4D("position");
        G4ThreeVector box_pos = G4ThreeVector(pos[0], pos[1], pos[2]);

        // Save Box
        fTargetBoxes.push_back(box_sol);
        fTargetBoxPositions.push_back(box_pos);
    }

    // Set flag and return
    fCheckTargetBoxes = true;
}

bool PrimaryGeneratorCos2::CheckTargetBoxes(G4ThreeVector& pos, G4ThreeVector& dir) {

    // If no target boxes defined all events are good
    if (fTargetBoxes.empty()) return true;

    // If target boxes defined only save trajectories that hit at least one
    for (uint i = 0; i < fTargetBoxes.size(); i++) {

        // Check distance to entry
        G4double d = (fTargetBoxes.at(i))->DistanceToIn(
                       pos - fTargetBoxPositions.at(i), dir);

        // If intercept found
        if (d != kInfinity) return true;

    }

    // If none found by this point return false
    return false;
}


void PrimaryGeneratorCos2::SampleKinematics(G4ThreeVector & pos, G4ThreeVector & dir, G4double& energy) {

    // Throw position uniformly inside box
    pos[0] = fSourceBoxPosition[0] + fSourceBoxWidth[0] * (-1.0 + 2.0 * G4UniformRand());
    pos[1] = fSourceBoxPosition[1] + fSourceBoxWidth[1] * (-1.0 + 2.0 * G4UniformRand());
    pos[2] = fSourceBoxPosition[2] + fSourceBoxWidth[2] * (-1.0 + 2.0 * G4UniformRand());

    // Throw direction isotropically, with a cos2theta distribution
    G4double phi = fMinPhi + (fMaxPhi - fMinPhi) * G4UniformRand(); //phi uniform in [-pi, pi];
    G4double theta = fThetaPDF->GetRandom();
    G4double cos_theta = cos(theta);
    G4double sin_theta = sin(theta);
    dir[0] = sin_theta * cos(phi);
    dir[1] = sin_theta * sin(phi);
    dir[2] = -cos_theta;

    // Throw energy uniformly between limits
    energy = fMinEnergy + (fMaxEnergy - fMinEnergy) * G4UniformRand();

}

void PrimaryGeneratorCos2::GeneratePrimaries(G4Event * anEvent) {

    // Start of the rejection sampling of tracks
    bool good_event = false;
    uint throws = 0;
    fDir = G4ThreeVector();
    fPos = G4ThreeVector();
    fEnergy = 0.0;

    // Convert to XYZ
    do {
        // Keep track of throw count
        throws++;
        fNThrows++;

        // Sample point direction and energy
        SampleKinematics(fPos, fDir, fEnergy);

        // Increment time for each throw
        fTime -= std::log(1 - G4UniformRand()) * (1.0 / fRate);

        // Check target boxes
        good_event = CheckTargetBoxes( fPos, fDir );

        // Iterate until good trajectory found
    } while (!good_event and throws < 1E8);

    // Failed to find event
    if (!good_event) {
        std::cout << "Failed to find good trajectory in 1E8 throws! Check target boxes!" << std::endl;
        throw;
    }

    // Update variables for processor
    fParticleGun->SetParticleEnergy(fEnergy);
    fParticleGun->SetParticleTime(fTime);
    fParticleGun->SetParticleMomentumDirection(fDir);
    fParticleGun->SetParticlePosition(fPos);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    return;
}
//---------------------------------------------------------------------------------


//------------------------------------------------------------------
Cos2PrimaryFluxProcessor::Cos2PrimaryFluxProcessor(PrimaryGeneratorCos2* gen, bool autosave) :
    VFluxProcessor("shukla"), fSave(autosave)
{
    fGenerator = gen;
}

bool Cos2PrimaryFluxProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    std::string tableindex = "Cos2";
    std::cout << "FLX: Registering Cos2PrimaryFluxProcessor NTuples " << tableindex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill index energy
    fTimeIndex   = man ->CreateNtupleDColumn(tableindex + "_t");
    fEnergyIndex = man ->CreateNtupleDColumn(tableindex + "_E");
    fDirXIndex   = man ->CreateNtupleDColumn(tableindex + "_dx");
    fDirYIndex   = man ->CreateNtupleDColumn(tableindex + "_dy");
    fDirZIndex   = man ->CreateNtupleDColumn(tableindex + "_dz");
    fPosXIndex   = man ->CreateNtupleDColumn(tableindex + "_x");
    fPosYIndex   = man ->CreateNtupleDColumn(tableindex + "_y");
    fPosZIndex   = man ->CreateNtupleDColumn(tableindex + "_z");
    fPDGIndex    = man ->CreateNtupleIColumn(tableindex + "_pdg");

    return true;
}

bool Cos2PrimaryFluxProcessor::ProcessEvent(const G4Event* /*event*/) {

    // Register Trigger State
    fHasInfo = true;
    fTime    = fGenerator->GetTime();
    fEnergy  = fGenerator->GetEnergy();

    // Set Ntuple to defaults

    if (fHasInfo) {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fTimeIndex,   fGenerator->GetTime());
        man->FillNtupleDColumn(fEnergyIndex, fGenerator->GetEnergy());
        man->FillNtupleDColumn(fDirXIndex,   fGenerator->GetDir().x() / MeV);
        man->FillNtupleDColumn(fDirYIndex,   fGenerator->GetDir().y() / MeV);
        man->FillNtupleDColumn(fDirZIndex,   fGenerator->GetDir().z() / MeV);
        man->FillNtupleDColumn(fPosXIndex,   fGenerator->GetPos().x() / m);
        man->FillNtupleDColumn(fPosYIndex,   fGenerator->GetPos().y() / m);
        man->FillNtupleDColumn(fPosZIndex,   fGenerator->GetPos().z() / m);
        man->FillNtupleIColumn(fPDGIndex ,   fGenerator->GetPDG());
        return true;
    } else {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fTimeIndex, -999.);
        man->FillNtupleDColumn(fEnergyIndex, -999.);
        man->FillNtupleDColumn(fDirXIndex, -999.);
        man->FillNtupleDColumn(fDirYIndex, -999.);
        man->FillNtupleDColumn(fDirZIndex, -999.);
        man->FillNtupleDColumn(fPosXIndex, -999.);
        man->FillNtupleDColumn(fPosYIndex, -999.);
        man->FillNtupleDColumn(fPosZIndex, -999.);
        man->FillNtupleIColumn(fPDGIndex,  -999 );
        return false;
    }
    return true;
}

G4double Cos2PrimaryFluxProcessor::GetExposureTime() {
    return fGenerator->GetTime();
}
//---------------------------------------------------------------------------------

} // - namespace CRESTA
