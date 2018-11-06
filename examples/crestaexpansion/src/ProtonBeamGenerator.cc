#include "ProtonBeamGenerator.hh"
#include "db/DB.hh"

namespace CRESTA {

//---------------------------------------------------------------------------------
ProtonBeamGenerator::ProtonBeamGenerator()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0),
      fNThrows(0),
      fParticleTime(0)
{
//---------------------------------------------------------------------------------                                                                                                                                                        
    std::cout << "FLX: Building Proton Beam Generator" << std::endl;

    // Setup Table
    DBTable table = DB::Get()->GetTable("protonbeam", "config");

    // Setup Particle Gun
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Setup Particle Gun
    std::cout << "FLX: --> Creating Particle Gun." << std::endl;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);
    fParticleGun->SetParticleDefinition(particleTable->FindParticle("proton"));

    // Get the Particle Energy, Rate, Dir, and Position
    fParticleEnergy = table.GetG4D("energy");
    fProtonRate = 1.0 / table.GetG4D("rate");

    std::vector<G4double> pos = table.GetVecG4D("position");
    fParticlePos = G4ThreeVector(pos[0],pos[1],pos[2]);

    std::vector<G4double> dir = table.GetVecG4D("direction");
    fParticleDir = G4ThreeVector(dir[0],dir[1],dir[2]);

    fParticleTime = 0.0;

    // Now setup the particle integrals and source/target boxes
    std::cout << "FLX: --> Complete." << std::endl;

    // Make a new processor
    Analysis::Get()->SetFluxProcessor(new ProtonBeamProcessor(this));
}

ProtonBeamGenerator::~ProtonBeamGenerator()
{
    delete fParticleGun;
}

void ProtonBeamGenerator::GeneratePrimaries(G4Event* anEvent) {

  fParticleTime += fProtonRate;

  fParticleGun->SetParticleEnergy(fParticleEnergy);
  fParticleGun->SetParticleTime(fParticleTime);
  fParticleGun->SetParticleMomentumDirection(fParticleDir);
  fParticleGun->SetParticlePosition(fParticlePos);
  fParticleGun->GeneratePrimaryVertex(anEvent);

  return;
}
//---------------------------------------------------------------------------------


//------------------------------------------------------------------
ProtonBeamProcessor::ProtonBeamProcessor(ProtonBeamGenerator* gen, bool autosave) :
    VFluxProcessor("protonbeam")
{
    fGenerator = gen;
}

bool ProtonBeamProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    std::string tableindex = "iso";
    std::cout << "FLX: Registering ProtonBeamProcessor NTuples " << tableindex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    return true;
}

bool ProtonBeamProcessor::ProcessEvent(const G4Event* /*event*/) {

    // Register Trigger State
    fHasInfo = true;
    fTime    = fGenerator->GetParticleTime();
    fEnergy  = fGenerator->GetParticleEnergy();
    return true;
}

G4double ProtonBeamProcessor::GetExposureTime() {
    return fGenerator->GetParticleTime();
}
//---------------------------------------------------------------------------------

} // - namespace CRESTA
