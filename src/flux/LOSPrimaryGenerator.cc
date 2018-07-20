#include "LOSPrimaryGenerator.hh"
#include "db/DB.hh"
#include "G4EmCalculator.hh"
#include "analysis/Analysis.hh"


namespace COSMIC {

//---------------------------------------------------------------------------------
LOSPrimaryGenerator::LOSPrimaryGenerator()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0)
{

    std::cout << "FLX: Building LOS Generator" << std::endl;
    fCheckTargetBoxes = false;
    fMinCosTheta=0;
    fMaxCosTheta=1;

    // Setup Table
    DBTable table = DB::Get()->GetTable("LOS", "config");

    // Get the particle table
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Save particle definitions
    // fParticleGun->SetParticleDefinition(particleTable->FindParticle("geantino"));
    fParticleDefs.push_back(particleTable->FindParticle("geantino"));

    // Setup Defaults + Table Inputs
    fMinEnergy = 0.1;
    fMaxEnergy = 5000.0;

    fPar_I0  = 88.5; // m-2 s-1 sr-1
    fPar_n   = 3.00;
    fPar_E0  = 4.28;
    fPar_eps = 854;
    fPar_rad = 6371.0;
    fPar_dis = 36.61;
    fMuonPDG = 13;//  Initialize this as a muon first (samples +/- later on)

    // Check for predef sets
    if (table.Has("parameters")) {
        std::string parset = table.GetS("parameters");

        if (parset.compare("nottingham") == 0) {
            std::cout << "FLX: --> Using Nottingham parset." << std::endl;

        } else if (parset.compare("princealbert")==0) {
            std::cout << "FLX: --> Using Nottingham-PA parset." << std::endl;
            fPar_I0 = 110.0;
            fPar_E0 = 3.6;

        } else if (parset.compare("hamburg") == 0) {
            std::cout << "FLX: --> Using Hamburg parset." << std::endl;
            fPar_I0  = 71.0;
            fPar_E0  = 23.78;
            fPar_eps = 2000.0;

        } else if (parset.compare("proton") == 0) {
            std::cout << "FLX: --> Using Proton parset." << std::endl;
            fPar_I0 = 8952.0;
            fPar_n = 2.93;
            fPar_E0 = 1.42;
            fPar_eps = 1e12;// 1/epsilon = 0

            // Set particle as a proton
            fMuonPDG = 2212;

        } else if (parset.compare("helium") == 0) {
            std::cout << "FLX: --> Using Helium parset." << std::endl;
            fPar_I0 = 5200.0;
            fPar_n = 2.75;
            fPar_E0 = 0.28;
            fPar_eps = 1e12;// 1/epsilon = 0
            // Set particle as an alpha particle
            fMuonPDG = 47;
        }
    } else {
        std::cout << "FLX: --> Using default Nottingham parset." << std::endl;
    }

    // Now look for manual overrides
    if (table.Has("min_energy")) fMinEnergy = table.GetD("min_energy");
    if (table.Has("max_energy")) fMaxEnergy = table.GetD("max_energy");

    if (table.Has("I0"))       fPar_I0  = table.GetD("I0");
    if (table.Has("n"))        fPar_n   = table.GetD("n");
    if (table.Has("E0"))       fPar_E0  = table.GetD("E0");
    if (table.Has("epsilon"))  fPar_eps = table.GetD("epsilon");
    if (table.Has("radius"))   fPar_rad = table.GetD("radius");
    if (table.Has("distance")) fPar_dis = table.GetD("distance");

    // Print Setup
    std::cout << "FLX: --> Min Energy : " << fMinEnergy << " GeV" << std::endl;
    std::cout << "FLX: --> Max Energy : " << fMaxEnergy << " GeV" << std::endl;
    std::cout << "FLX: --> I0         : " << fPar_I0 << " m-2 s-1 sr -1" << std::endl;
    std::cout << "FLX: --> n          : " << fPar_n << std::endl;
    std::cout << "FLX: --> E0         : " << fPar_E0 << " GeV" << std::endl;
    std::cout << "FLX: --> epsilon    : " << fPar_eps << " GeV" << std::endl;
    std::cout << "FLX: --> radius     : " << fPar_rad << " km"  << std::endl;
    std::cout << "FLX: --> distance   : " << fPar_dis << " km"  << std::endl;
    std::cout << "FLX: --> R/d        : " << fPar_rad / fPar_dis << std::endl;

    // Check limits
    if (fMinEnergy < 0.1) {
        std::cout << "min_energy must be >= 0.1 GeV!" << std::endl;
        throw;
    }
    if (fMaxEnergy > 5000.0) {
        std::cout << "max_energy must be <= 5000 GeV" << std::endl;
        throw;
    }

    // // Setup the energy PDF
    std::cout << "FLX: --> Setting up PDFs." << std::endl;
    fEnergyPDF = new TF1("energy_pdf",
                         "[3]*((x+[0])**(-[2]))*([1]/([1]+x))", fMinEnergy * GeV, fMaxEnergy * GeV);

    // 0 degrees parameters from fit in Nottingham
    fEnergyPDF->SetParameter(0, fPar_E0 * GeV);
    fEnergyPDF->SetParameter(1, fPar_eps * GeV);
    fEnergyPDF->SetParameter(2, fPar_n);
    fEnergyPDF->SetParameter(3, 1.0);

    // Normalize the PDF
    G4double norm = fEnergyPDF->Integral(0.1 * GeV, 5000.0 * GeV);
    fEnergyPDF->SetParameter(3, 1.0 / norm);

    // Get the speed up factor
    fSpeedUp = fEnergyPDF->Integral(fMinEnergy * GeV, fMaxEnergy * GeV);
    std::cout << "FLX: --> fSpeedUp        : " << fSpeedUp << std::endl;

    // From dataset fit in Shukla paper
    G4double vertical_flux_rate = fPar_I0;// m-2 s-1 sr-1
    std::cout << "FLX: --> VerticalFlux : " << vertical_flux_rate << " m-2 s-1 sr-1" << std::endl;

    // Parameters : [0] = r, x = cos_theta
    fZenithPDF = new TF1("fZenithPDF",
                         "(TMath::Sqrt([0]*[0]*x*x+2*[0]+1) - [0]*x)**(-[1]+1)", 0.0, 1.0);
    fZenithPDF->SetParameter(0, fPar_rad / fPar_dis);
    fZenithPDF->SetParameter(1, fPar_n);

    // For a vertical source, 0 < phi < 2*pi (both directions), 0 < theta < pi/2 (0 < cos_theta < 1)
    fFluxIntegrated = (vertical_flux_rate * CLHEP::twopi
                       * fZenithPDF->Integral(0.0, 1.0)
                       * fEnergyPDF->Integral(fMinEnergy * GeV, fMaxEnergy * GeV));

    // The muon rate
    std::cout << "FLX: --> Integrated Flux : " << fFluxIntegrated << " m-2 s-1" << std::endl;
    std::cout << "FLX: --> Integrated Flux : " << 60.0 * fFluxIntegrated*cm*cm / m / m  << " cm-2 min-1" << std::endl;

    // Setup Particle Gun
    std::cout << "FLX: --> Creating Particle Gun." << std::endl;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);

    // Now setup the particle integrals and source/target boxes
    fEnergyPDF->SetRange(fMinEnergy * GeV, fMaxEnergy  * GeV);
    fSourceBox = false;
    GetSourceBox();
    GetTargetBoxes();
    fEnergyPDF->SetRange(fMinEnergy  * GeV, fMaxEnergy * GeV); // E in MeV

    std::cout << "FLX: --> Complete." << std::endl;


    // Make a new processor
    Analysis::Get()->SetFluxProcessor(new LOSPrimaryFluxProcessor(this));

}

LOSPrimaryGenerator::~LOSPrimaryGenerator()
{
    delete fParticleGun;
}

void LOSPrimaryGenerator::GetSourceBox() {

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

        if (tbl.Has("require_n")) {
          fSourceBoxRequireN  = tbl.GetI("require_n");
        } else{
          fSourceBoxRequireN  = 1;
        }

        fArea = size[0] * size[1] / m / m;
        break;
    }
    fSourceBox = true;

    if (fSourceBox) return;

    // Cant find
    std::cout << "Cannot find source box table!" << std::endl;
    throw;
}

std::vector<G4Box*> LOSPrimaryGenerator::GetTargetBoxes() {

    // If presences of target boxes already been set then just return
    if (fCheckTargetBoxes) {
        return fTargetBoxes;
    }

    // If its not set but we have boxes return boxes
    if (fTargetBoxes.size() > 0) {
        fCheckTargetBoxes = true;
        return fTargetBoxes;
    }
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

        // Create objects
        std::vector<G4double> size = tbl.GetVecG4D("size");
        std::vector<G4double> pos  = tbl.GetVecG4D("position");


        G4Box* box_sol = new G4Box(index, 0.5 * size[0], 0.5 * size[1], 0.5 * size[2]);
        G4ThreeVector box_pos = G4ThreeVector(pos[0], pos[1], pos[2]);

        std::cout << "Box Pos: " << box_pos << std::endl;
      // Save Box
        fTargetBoxes.push_back(box_sol);
        fTargetBoxPositions.push_back(box_pos);

    }


    // Calculate the corners of a box
    std::vector<G4ThreeVector> corners0,corners1;
    for(int i=-1;i<2;i++){
      for(int j=-1;j<2;j++){
        for(int k=-1;k<2;k++){
          G4ThreeVector corner0(fSourceBoxPosition[0] + i*fSourceBoxWidth[0],fSourceBoxPosition[1] + j*fSourceBoxWidth[1],fSourceBoxPosition[2] + k*fSourceBoxWidth[2]);
          corners0.push_back(corner0);

          G4ThreeVector corner1(fTargetBoxPositions.at(0)[0] + i*fTargetBoxes.at(0)->GetXHalfLength(),fTargetBoxPositions.at(0)[1] + j*fTargetBoxes.at(0)->GetYHalfLength(),fTargetBoxPositions.at(0)[2] + k*fTargetBoxes.at(0)->GetZHalfLength());
          corners1.push_back(corner1);

        }
      }
    }

    // Find the range of zenith angles sampled
    double min_cos_theta=10;
    double max_cos_theta=-10;
    for(int c0=0;c0<corners0.size();c0++){
      for(int c1=0;c1<corners1.size();c1++){
        // std::cout << "FLX: Corner0 : " << corners0.at(c0) << " Corner1 : " << corners1.at(c1) <<  std::endl;

        G4ThreeVector corner2corner = corners1.at(c1) - corners0.at(c0);
        double cos_theta = cos((corner2corner.unit()).theta());
        if(cos_theta<min_cos_theta) min_cos_theta = cos_theta;
        if(cos_theta>max_cos_theta) max_cos_theta = cos_theta;
        if(min_cos_theta<0) min_cos_theta=0;

        // std::cout << "Min Cos Theta: " << min_cos_theta << "Max Cos Theta: " << max_cos_theta << std::endl;
      }
    }

    fMinCosTheta = min_cos_theta;
    fMaxCosTheta = max_cos_theta;

    std::cout << "FLX: Previous Flux Integrated: " << fFluxIntegrated << std::endl;
    fFluxIntegrated = (fPar_I0 * CLHEP::twopi * fZenithPDF->Integral(min_cos_theta, max_cos_theta) * fEnergyPDF->Integral(fMinEnergy * GeV, fMaxEnergy * GeV));
    std::cout << "FLX: Min Cos Theta: " << min_cos_theta << " Max Cos Theta: " << max_cos_theta << std::endl;
    std::cout << "FLX: Angle-Limited Flux Integrated: " << fFluxIntegrated << std::endl;



    // Set flag and return
    fCheckTargetBoxes = true;
    return fTargetBoxes;
}

std::vector<G4ThreeVector> LOSPrimaryGenerator::GetTargetBoxPositions() {
    // If matching sizes its probs okay to return positions
    if (fTargetBoxes.size() == fTargetBoxPositions.size()) return fTargetBoxPositions;
    std::cout << "TargetBox Positions incorrect" << std::endl;
    throw;
}

void LOSPrimaryGenerator::GeneratePrimaries(G4Event* anEvent) {

    // Run Initial Setup Incase we are in a seperate thread
    if (!fSourceBox) {
        G4AutoLock lock(&myMutexLOS);
        GetSourceBox();
        lock.unlock();
    }

    if (!fCheckTargetBoxes) {
        G4AutoLock lock(&myMutexLOS);
        GetTargetBoxes();
        lock.unlock();
    }

    // Generate a point on the first target box
    // fEventSourcePosition[0] = fTargetBoxPositions.at(0)[0] + (fTargetBoxes.at(0))->GetXHalfLength() * (-1.0 + 2.0 * G4UniformRand()) ;
    // fEventSourcePosition[1] = fTargetBoxPositions.at(0)[1] + (fTargetBoxes.at(0))->GetYHalfLength() * (-1.0 + 2.0 * G4UniformRand()) ;
    // fEventSourcePosition[2] = fTargetBoxPositions.at(0)[2];
    fEventSourcePosition[0] = fSourceBoxPosition[0] + fSourceBoxWidth[0] * (-1.0 + 2.0 * G4UniformRand()) ;
    fEventSourcePosition[1] = fSourceBoxPosition[1] + fSourceBoxWidth[1] * (-1.0 + 2.0 * G4UniformRand()) ;
    fEventSourcePosition[2] = fSourceBoxPosition[2] + fSourceBoxWidth[2];

    // Generate a point on the second target box
    // - Sample equal cos_theta bins
    // fEventTargetPosition[0] = fTargetBoxPositions.at(1)[0] + (fTargetBoxes.at(1))->GetXHalfLength() * (-1.0 + 2.0 * G4UniformRand()) ;
    // fEventTargetPosition[1] = fTargetBoxPositions.at(1)[1] + (fTargetBoxes.at(1))->GetYHalfLength() * (-1.0 + 2.0 * G4UniformRand()) ;
    // fEventTargetPosition[2] = fTargetBoxPositions.at(1)[2];
    // fMuonDir = fEventTargetPosition - fEventSourcePosition;

    G4double cos_theta = (fMaxCosTheta - fMinCosTheta)*G4UniformRand() + fMinCosTheta;// cos_theta in [min_cos_theta,max_cos_theta]
    G4double phi = CLHEP::twopi * G4UniformRand(); //phi uniform in [-pi, pi];
    G4double sin_theta = std::sqrt(1 - cos_theta * cos_theta);
    G4double x = sin_theta * cos(phi);
    G4double y = sin_theta * sin(phi);
    G4double z = cos_theta;

    fMuonEnergy = fEnergyPDF->GetRandom();

    fMuonDir = G4ThreeVector(x, y, z);
    fParticleGun->SetParticleMomentumDirection(fMuonDir.unit());

    fParticleGun->SetParticleDefinition(fParticleDefs[0]);
    fParticleGun->SetParticleEnergy(fMuonEnergy);
    fParticleGun->SetParticleTime(0.0);

    // Get the PDF value at this direction
    fMuonAnglePDF = fZenithPDF->Eval(cos_theta);
    fMuonEnergyPDF = fEnergyPDF->Eval(fMuonEnergy);

    fMuonPos = fEventSourcePosition;
    fParticleGun->SetParticlePosition(fEventSourcePosition);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    if(anEvent->GetEventID()%10000==0){
      // Write out the estimated time and overall raate
      // std::cout << "FLX: Integrated Rate = " << fFluxIntegrated << " m-2 s-1" << std::endl;
      std::cout << "FLX: Open-Sky Exposure = " << (((double) Analysis::Get()->GetNSavedEvents())/GetIntegratedRate())/(60.0*60.0*24.0) << " Days" << std::endl;
    }

    return;
}
//---------------------------------------------------------------------------------




//------------------------------------------------------------------
LOSPrimaryFluxProcessor::LOSPrimaryFluxProcessor(LOSPrimaryGenerator* gen, bool autosave) :
    VFluxProcessor("LOS"), fSave(autosave)
{
    fGenerator = gen;
}


bool LOSPrimaryFluxProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    std::string tableindex = "LOS";
    std::cout << "FLX: Registering LOSPrimaryFluxProcessor NTuples " << tableindex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill index energy
    fMuonIDIndex = man ->CreateNtupleIColumn(tableindex + "_id");
    fMuonEnergyIndex = man ->CreateNtupleDColumn(tableindex + "_E");
    fMuonDirXIndex   = man ->CreateNtupleDColumn(tableindex + "_dx");
    fMuonDirYIndex   = man ->CreateNtupleDColumn(tableindex + "_dy");
    fMuonDirZIndex   = man ->CreateNtupleDColumn(tableindex + "_dz");
    fMuonPosXIndex   = man ->CreateNtupleDColumn(tableindex + "_x");
    fMuonPosYIndex   = man ->CreateNtupleDColumn(tableindex + "_y");
    fMuonPosZIndex   = man ->CreateNtupleDColumn(tableindex + "_z");
    fMuonThXZIndex   = man ->CreateNtupleDColumn(tableindex + "_thxz");
    fMuonThYZIndex   = man ->CreateNtupleDColumn(tableindex + "_thyz");
    fMuonAnglePDFIndex = man ->CreateNtupleDColumn(tableindex + "_thweight");
    fMuonEnergyPDFIndex = man ->CreateNtupleDColumn(tableindex + "_Eweight");
    fMuonRatePDFIndex = man ->CreateNtupleDColumn(tableindex + "_rate");
    std::cout << "FLX: Finished registration of LOSPrimaryFluxProcessor " << tableindex << std::endl;
    return true;
}

bool LOSPrimaryFluxProcessor::ProcessEvent(const G4Event* event) {

    // Register Trigger State
    fHasInfo = true;

    // Set Ntuple to defaults
    if (fHasInfo) {

        G4AnalysisManager* man = G4AnalysisManager::Instance();

        man->FillNtupleIColumn(fMuonIDIndex, (int) event->GetEventID());
        man->FillNtupleDColumn(fMuonEnergyIndex, fGenerator->GetMuonEnergy());

        man->FillNtupleDColumn(fMuonDirXIndex,   fGenerator->GetMuonDir().x()/cm );
        man->FillNtupleDColumn(fMuonDirYIndex,   fGenerator->GetMuonDir().y()/cm );
        man->FillNtupleDColumn(fMuonDirZIndex,   fGenerator->GetMuonDir().z()/cm );

        man->FillNtupleDColumn(fMuonPosXIndex,   fGenerator->GetMuonPos().x() / cm);
        man->FillNtupleDColumn(fMuonPosYIndex,   fGenerator->GetMuonPos().y() / cm);
        man->FillNtupleDColumn(fMuonPosZIndex,   fGenerator->GetMuonPos().z() / cm);

        G4ThreeVector dir = (fGenerator->GetMuonDir()).unit();
        double fThetaXZ = std::atan2(dir.getZ(),dir.getX());
        double fThetaYZ = std::atan2(dir.getZ(),dir.getY());
        man->FillNtupleDColumn(fMuonThXZIndex,   fThetaXZ);
        man->FillNtupleDColumn(fMuonThYZIndex,   fThetaYZ);

        man->FillNtupleDColumn(fMuonAnglePDFIndex,   fGenerator->GetMuonAnglePDF() );
        man->FillNtupleDColumn(fMuonEnergyPDFIndex,  fGenerator->GetMuonEnergyPDF() );

        // Integrate this over the region to find the rate through the detector in m-2 s-1
        man->FillNtupleDColumn(fMuonRatePDFIndex,  fGenerator->GetIntegratedRate() );

        return true;

    } else {

        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleIColumn(fMuonIDIndex, (int) event->GetEventID());
        man->FillNtupleDColumn(fMuonEnergyIndex, -999);
        man->FillNtupleDColumn(fMuonEnergyPDFIndex,-999);
        man->FillNtupleDColumn(fMuonDirXIndex, -999.);
        man->FillNtupleDColumn(fMuonDirYIndex, -999.);
        man->FillNtupleDColumn(fMuonDirZIndex, -999.);
        man->FillNtupleDColumn(fMuonPosXIndex, -999.);
        man->FillNtupleDColumn(fMuonPosYIndex, -999.);
        man->FillNtupleDColumn(fMuonPosZIndex, -999.);
        man->FillNtupleDColumn(fMuonThXZIndex,   -999);
        man->FillNtupleDColumn(fMuonThYZIndex,   -999);
        man->FillNtupleDColumn(fMuonAnglePDFIndex,   -999);
        man->FillNtupleDColumn(fMuonRatePDFIndex,   -999);
        return false;

    }
    return true;
}

//---------------------------------------------------------------------------------

} // - namespace COSMIC
