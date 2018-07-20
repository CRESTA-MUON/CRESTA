#include "PrimaryGeneratorROOT.hh"
#include "db/DB.hh"

namespace COSMIC {

//---------------------------------------------------------------------------------
PrimaryGeneratorROOT::PrimaryGeneratorROOT()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0),
      fMuonTime(0)
{
    G4AutoLock lock(&thisMutex);
    std::cout << "FLX: Building ROOT Source Generator" << std::endl;

    // Setup Table
    DBTable table = DB::Get()->GetTable("ROOT", "config");

    // Get the particle table
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    // Save particle definitions
    fParticleDefs.push_back(particleTable->FindParticle("neutron"));

    // Setup Defaults + Table Inputs
    fMinEnergy = 1e-8;// MeV
    fMaxEnergy = 1e4;//MeV

    if (table.Has("min_energy")) fMinEnergy = table.GetD("min_energy");
    if (table.Has("max_energy")) fMaxEnergy = table.GetD("max_energy");

    if (table.Has("spectrum_file")) fFilename = table.GetS("spectrum_file");
    if (table.Has("histogram")) fHistName = table.GetS("histogram");
    if (table.Has("I0")) fPar_I0  = table.GetD("I0");

    // Load the histogram root file
    fHandle = new TFile(fFilename.c_str());
    fSpectrum = (TH1F*) fHandle->Get(fHistName.c_str());

    // For a vertical source, 0 < phi < 2*pi (both directions), 0 < theta < pi/2 (0 < cos_theta < 1)
    fFluxIntegrated = (fPar_I0 * CLHEP::twopi
                       * fSpectrum->Integral(fMinEnergy, fMaxEnergy));

    fExposureTime=0;

    // The muon rate
    std::cout << "FLX: --> Integrated Flux : " << fFluxIntegrated << " m-2 s-1" << std::endl;
    std::cout << "FLX: --> Integrated Flux : " << 60.0 * fFluxIntegrated*cm*cm / m / m  << " cm-2 min-1" << std::endl;

    // Setup Particle Gun
    std::cout << "FLX: --> Creating Particle Gun." << std::endl;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);

    // Now setup the particle integrals and source/target boxes
    fSourceBox = false;
    GetSourceBox();
    std::cout << "FLX: --> Complete." << std::endl;

    // Make a new processor
    Analysis::Get()->SetFluxProcessor(new ROOTPrimaryFluxProcessor(this));
}


PrimaryGeneratorROOT::~PrimaryGeneratorROOT()
{
    fHandle->Close();
    delete fParticleGun;
}

G4ThreeVector PrimaryGeneratorROOT::SampleDirection() {
    G4double cos_theta = G4UniformRand();
    G4double phi = CLHEP::twopi * G4UniformRand(); //phi uniform in [-pi, pi];
    G4double sin_theta = std::sqrt(1 - cos_theta * cos_theta);
    G4double x = sin_theta * cos(phi);
    G4double y = sin_theta * sin(phi);
    G4double z = cos_theta;
    return G4ThreeVector(x, y, -z);
}

void PrimaryGeneratorROOT::SampleParticleType() {
  fParticleGun->SetParticleDefinition(fParticleDefs[0]);
}

void PrimaryGeneratorROOT::GetSourceBox() {

    // Already has good source_box
    if (fSourceBox) return;
    std::cout << "FLX: --> Creating Source box" << std::endl;

    std::vector<DBTable> targetlinks = DB::Get()->GetTableGroup("FLUX");
    for (uint i = 0; i < targetlinks.size(); i++) {
        DBTable tbl = targetlinks[i];

        // Select tables with target box names
        std::string index = tbl.GetIndexName();
        if (index.compare("source_box") != 0) continue;

        std::vector<G4double> size = tbl.GetVecG4D("size");
        std::vector<G4double> pos  = tbl.GetVecG4D("position");
        fSourceBoxWidth    = G4ThreeVector(0.5 * size[0], 0.5 * size[1], 0.0);
        fSourceBoxPosition = G4ThreeVector(pos[0], pos[1], pos[2]);

        fArea = size[0] * size[1] / m / m;
        break;
    }
    fSourceBox = true;

    if (fSourceBox) return;

    // Cant find
    std::cout << "Cannot find source box table!" << std::endl;
    throw;
}


void PrimaryGeneratorROOT::GeneratePrimaries(G4Event* anEvent) {

    // Run Initial Setup Incase we are in a seperate thread
    if (!fSourceBox) {
        G4AutoLock lock(&thisMutex);
        GetSourceBox();
        lock.unlock();
    }

    // Sample the energy and particle type
    G4double E = 0.0;
    E = fSpectrum->GetRandom();

    // Only want to sample the particle if we want muons
    SampleParticleType();

    // Sample direction
    G4ThreeVector direction = G4ThreeVector();
    direction = SampleDirection();

    // Sample position
    G4ThreeVector position = G4ThreeVector();
    position[0] = fSourceBoxPosition[0] + fSourceBoxWidth[0] * (-1.0 + 2.0 * G4UniformRand()) ;
    position[1] = fSourceBoxPosition[1] + fSourceBoxWidth[1] * (-1.0 + 2.0 * G4UniformRand()) ;
    position[2] = fSourceBoxPosition[2] + fSourceBoxWidth[2] ;


    fMuonDir = direction;
    fMuonPos = position;
    fMuonEnergy = E;

    fParticleGun->SetParticleEnergy(fMuonEnergy);
    fParticleGun->SetParticleTime(fMuonTime);
    fParticleGun->SetParticleMomentumDirection(fMuonDir);
    fParticleGun->SetParticlePosition(fMuonPos);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    return;
}
//---------------------------------------------------------------------------------


//------------------------------------------------------------------
ROOTPrimaryFluxProcessor::ROOTPrimaryFluxProcessor(PrimaryGeneratorROOT* gen, bool autosave) :
    VFluxProcessor("shukla"), fSave(autosave)
{
    fGenerator = gen;
}

bool ROOTPrimaryFluxProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    std::string tableindex = "root";
    std::cout << "FLX: Registering ROOTPrimaryFluxProcessor NTuples " << tableindex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill index energy
    fMuonTimeIndex   = man ->CreateNtupleDColumn(tableindex + "_t");
    fMuonEnergyIndex = man ->CreateNtupleDColumn(tableindex + "_E");
    fMuonDirXIndex   = man ->CreateNtupleDColumn(tableindex + "_dx");
    fMuonDirYIndex   = man ->CreateNtupleDColumn(tableindex + "_dy");
    fMuonDirZIndex   = man ->CreateNtupleDColumn(tableindex + "_dz");
    fMuonPosXIndex   = man ->CreateNtupleDColumn(tableindex + "_x");
    fMuonPosYIndex   = man ->CreateNtupleDColumn(tableindex + "_y");
    fMuonPosZIndex   = man ->CreateNtupleDColumn(tableindex + "_z");
    fMuonPDGIndex    = man ->CreateNtupleIColumn(tableindex + "_pdg");

    return true;
}

bool ROOTPrimaryFluxProcessor::ProcessEvent(const G4Event* /*event*/) {

    // Register Trigger State
    fHasInfo = true;
    fTime    = fGenerator->GetMuonTime();
    fEnergy  = fGenerator->GetMuonEnergy();

    // Set Ntuple to defaults

    if (fHasInfo) {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fMuonTimeIndex,   fGenerator->GetMuonTime());
        man->FillNtupleDColumn(fMuonEnergyIndex, fGenerator->GetMuonEnergy());
        man->FillNtupleDColumn(fMuonDirXIndex,   fGenerator->GetMuonDir().x() / MeV);
        man->FillNtupleDColumn(fMuonDirYIndex,   fGenerator->GetMuonDir().y() / MeV);
        man->FillNtupleDColumn(fMuonDirZIndex,   fGenerator->GetMuonDir().z() / MeV);
        man->FillNtupleDColumn(fMuonPosXIndex,   fGenerator->GetMuonPos().x() / m);
        man->FillNtupleDColumn(fMuonPosYIndex,   fGenerator->GetMuonPos().y() / m);
        man->FillNtupleDColumn(fMuonPosZIndex,   fGenerator->GetMuonPos().z() / m);
        man->FillNtupleIColumn(fMuonPDGIndex ,   fGenerator->GetMuonPDG());
        return true;
    } else {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fMuonTimeIndex, -999.);
        man->FillNtupleDColumn(fMuonEnergyIndex, -999.);
        man->FillNtupleDColumn(fMuonDirXIndex, -999.);
        man->FillNtupleDColumn(fMuonDirYIndex, -999.);
        man->FillNtupleDColumn(fMuonDirZIndex, -999.);
        man->FillNtupleDColumn(fMuonPosXIndex, -999.);
        man->FillNtupleDColumn(fMuonPosYIndex, -999.);
        man->FillNtupleDColumn(fMuonPosZIndex, -999.);
        man->FillNtupleIColumn(fMuonPDGIndex,  -999 );
        return false;
    }
    return true;
}

G4double ROOTPrimaryFluxProcessor::GetExposureTime() {
    return fGenerator->GetMuonTime();
}
//---------------------------------------------------------------------------------

} // - namespace COSMIC
