#include "IsotropicSphereSource.hh"
#include "db/DB.hh"

namespace COSMIC {

//---------------------------------------------------------------------------------
IsotropicSphereGenerator::IsotropicSphereGenerator()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0),
      fNThrows(0),
      fParticleTime(0)
{
    //    G4AutoLock lock(&myMutex);
    std::cout << "FLX: Building Isotropic Sphere Generator" << std::endl;

    // Setup Table
    DBTable table = DB::Get()->GetTable("IsotropicSphere", "config");
    fSphereRadius = table.GetG4D("sphere_radius");

    // Setup PDFs
    std::string pdffunc = "1.0"; //< Default Isotropic
    std::vector<double> pdfvars = std::vector<double>(0);

    // --> Sphere Zenith
    pdffunc = "1.0";
    pdfvars.clear();
    if (table.Has("sphere_zenith_pdf")) {
        pdffunc = table.GetS("sphere_zenith_pdf");
        pdfvars = table.GetVecG4D("sphere_zenith_vars");
    }
    fSphereZenithPDF = new TF1("sphere_zenith_pdf", pdffunc.c_str(), 0, 6.28318530718);
    for (int i = 0; i < pdfvars.size(); i++) {
        fSphereZenithPDF->SetParameter(i, pdfvars[i]);
    }

    // --> Sphere Azimuth
    pdffunc = "1.0";
    pdfvars.clear();
    if (table.Has("sphere_azimuthal_pdf")) {
        pdffunc = table.GetS("sphere_azimuthal_pdf");
        pdfvars = table.GetVecG4D("sphere_azimuthal_vars");
    }
    fSphereAzimuthalPDF = new TF1("sphere_azimuthal_pdf", pdffunc.c_str(), 0., 6.28318530718);
    for (int i = 0; i < pdfvars.size(); i++) {
        fSphereAzimuthalPDF->SetParameter(i, pdfvars[i]);
    }

    // --> Particle Zenith
    pdffunc = "1.0";
    pdfvars.clear();
    if (table.Has("particle_zenith_pdf")) {
        pdffunc = table.GetS("particle_zenith_pdf");
        pdfvars = table.GetVecG4D("particle_zenith_vars");
    }
    fParticleZenithPDF = new TF1("particle_zenith_pdf", pdffunc.c_str(), 0., 6.28318530718);
    for (int i = 0; i < pdfvars.size(); i++) {
        fParticleZenithPDF->SetParameter(i, pdfvars[i]);
    }

    // --> Particle Azimuthal
    pdffunc = "1.0";
    pdfvars.clear();
    if (table.Has("particle_azimuthal_pdf")) {
        pdffunc = table.GetS("particle_azimuthal_pdf");
        pdfvars = table.GetVecG4D("particle_azimuthal_vars");
    }
    fParticleAzimuthalPDF = new TF1("particle_azimuthal_pdf", pdffunc.c_str(), 0., 6.28318530718);
    for (int i = 0; i < pdfvars.size(); i++) {
        fParticleAzimuthalPDF->SetParameter(i, pdfvars[i]);
    }

    // --> Particle Energy
    pdffunc = "1.0";
    pdfvars.clear();
    fMinEnergy = 1 * GeV;
    fMaxEnergy = 2 * GeV;
    if (table.Has("particle_energy_pdf")) {
        pdffunc = table.GetS("particle_energy_pdf");
        pdfvars = table.GetVecG4D("particle_energy_vars");
    }
    if (table.Has("particle_energy_min")) fMinEnergy = table.GetG4D("particle_energy_min");
    if (table.Has("particle_energy_max")) fMaxEnergy = table.GetG4D("particle_energy_max");
    fParticleEnergyPDF = new TF1("particle_energy_pdf", pdffunc.c_str(), fMinEnergy, fMaxEnergy);
    for (int i = 0; i < pdfvars.size(); i++) {
        fParticleEnergyPDF->SetParameter(i, pdfvars[i]);
    }
    fParticleEnergyPDF->SetRange(fMinEnergy, fMaxEnergy);

    // Setup Particle Gun
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    std::vector<std::string> particle_ids   = table.GetVecS("particle_ids");
    std::vector<double>      particle_probs = table.GetVecG4D("particle_probs");

    double probcount = 0.0;
    for (int i = 0; i < particle_ids.size(); i++) {

        std::string id = particle_ids[i];
        double prob = particle_probs[i];
        probcount += prob;

        fParticleDefs.push_back( particleTable->FindParticle(id) );
        fParticlePDGs.push_back( 0 );
        fParticleProb.push_back(probcount);
    }

    for (int i = 0; i < fParticleProb.size(); i++) {
        fParticleProb[i] /= probcount;
    }

    // Setup Particle Gun
    std::cout << "FLX: --> Creating Particle Gun." << std::endl;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);

    // Now setup the particle integrals and source/target boxes
    std::cout << "FLX: --> Complete." << std::endl;

    // Make a new processor
    Analysis::Get()->SetFluxProcessor(new IsotropicSphereFluxProcessor(this));
}

IsotropicSphereGenerator::~IsotropicSphereGenerator()
{
    delete fParticleGun;
}

void IsotropicSphereGenerator::SamplePositionAndDirection(G4ThreeVector& pos, G4ThreeVector& dir)
{
    G4double theta = fSphereZenithPDF->GetRandom();
    G4double cos_theta = cos(theta);
    G4double sin_theta = sin(theta);

    G4double phi = fSphereAzimuthalPDF->GetRandom();
    G4double cos_phi   = cos(phi);
    G4double sin_phi   = sin(phi);

    G4double x = fSphereRadius * sin_theta * cos_phi;
    G4double y = fSphereRadius * sin_theta * sin_phi;
    G4double z = fSphereRadius * cos_theta;
    pos = G4ThreeVector(x, y, z);
    // pos = G4ThreeVector(0, y, 0.5*m);

    // pos = G4ThreeVector(fSphereRadius, 0, 0);

    // Smear the direction slightly
    // theta += G4RandGauss::shoot(0.0, 3.1415926536);
    // phi   += G4RandGauss::shoot(0.0, 3.1415926536);

    cos_theta = cos(theta);
    sin_theta = sin(theta);
    cos_phi   = cos(phi);
    sin_phi   = sin(phi);
    x = sin_theta * cos_phi;
    y = sin_theta * sin_phi;
    z = cos_theta;
    dir = G4ThreeVector(-x,-y,-z);
    // dir = G4ThreeVector(0.0, 0.0, -1.0);
    dir = (1.0/dir.mag()) * dir;





    // // Require the throws to be INWARDS (BRUTE FORCE METHOD)
    // bool validthrow = false;
    // while (!validthrow) {
    //     G4double dir_cos_theta = fParticleZenithPDF->GetRandom();
    //     G4double dir_sin_theta = std::sqrt(1 - dir_cos_theta * dir_cos_theta);
    //     G4double dir_cos_phi   = fParticleAzimuthalPDF->GetRandom();
    //     G4double dir_sin_phi   = std::sqrt(1 - dir_cos_phi * dir_cos_phi);
    //     G4double dir_x = dir_sin_theta * dir_cos_phi;
    //     G4double dir_y = dir_sin_theta * dir_sin_phi;
    //     G4double dir_z = dir_cos_theta;
    //     dir = G4ThreeVector(dir_x, dir_y, dir_z);


    //     G4ThreeVector throwpos = (pos + fSphereRadius * 0.05 * dir);
    //     if (throwpos.mag() < fSphereRadius) validthrow = true;
    // }

    return;
}



void IsotropicSphereGenerator::SampleParticleType() {

    G4double r = G4UniformRand();
    for (int i = 0; i < fParticleProb.size(); i++) {
        if (r < fParticleProb[i]) {
            fParticleGun->SetParticleDefinition(fParticleDefs[i]);
            fParticlePDG = fParticlePDGs[i];
            break;
        }
    }
}

std::vector<G4Box*> IsotropicSphereGenerator::GetTargetBoxes() {

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

        // Save Box
        fTargetBoxes.push_back(box_sol);
        fTargetBoxPositions.push_back(box_pos);

    }

    // Set flag and return
    fCheckTargetBoxes = true;
    return fTargetBoxes;
}


void IsotropicSphereGenerator::Draw() {
    // G4VVisManager* vis = Analysis::Get()->GetVisManager();
    // if (vis) {

    //   G4Box drawbox = G4Box("source_box",  0.5 * fSourceBoxWidth[0], 0.5 * fSourceBoxWidth[1], 0.5 * fSourceBoxWidth[2]);
    //   G4Transform3D tr = G4Transform3D(G4RotationMatrix(), fSourceBoxPosition);
    //   G4Colour colour(0., 1., 0.);
    //   G4VisAttributes attribs(colour);
    //   vis->Draw(drawbox, attribs, tr);

    //   for (int i = 0; i < fTargetBoxes.size(); i++) {
    //     G4Box drawbox2 = G4Box("target_box", fTargetBoxes[i]->GetXHalfLength(), fTargetBoxes[i]->GetYHalfLength(), fTargetBoxes[i]->GetZHalfLength());
    //     G4Transform3D tr2 = G4Transform3D(G4RotationMatrix(), fTargetBoxPositions[i]);
    //     G4Colour colour2(1., 0., 0.);
    //     G4VisAttributes attribs2(colour2);
    //     vis->Draw(drawbox2, attribs2, tr2);
    //   }
    // }
}


std::vector<G4ThreeVector> IsotropicSphereGenerator::GetTargetBoxPositions() {
    // If matching sizes its probs okay to return positions
    if (fTargetBoxes.size() == fTargetBoxPositions.size()) return fTargetBoxPositions;
    std::cout << "TargetBox Positions incorrect" << std::endl;
    throw;
}

void IsotropicSphereGenerator::GeneratePrimaries(G4Event* anEvent) {

    if (!fCheckTargetBoxes) {
        //        G4AutoLock lock(&myMutex);
        GetTargetBoxes();
        //        lock.unlock();
    }

    // Create a mutex lock for the ROOT sampling functions (not thread-safe)
    // G4AutoLock lock(&myMutex);

    // By setting a fMinEnergy it is possible to remove a portion
    // of the muon energy spectra which you're sure will not contribute
    // to the detector counts
    // i.e. for a geometry involving a thick layer of rock, you could ignore
    // the energies that will definitely range out before the detector.
    // These muons with energy<fMinEnergy will be generated but rejected here.

    // G4double global_time = 0;//< Not needed as we have fMuonTime?

    // Sample the energy and particle type
    G4double E = 0.0;
    E = fParticleEnergyPDF->GetRandom();
    // Only want to sample the particle if we want muons
    SampleParticleType();

    // Start of the rejection sampling of muon tracks
    bool good_event = false;
    uint throws = 0;
    G4ThreeVector direction = G4ThreeVector();
    G4ThreeVector position = G4ThreeVector();

    G4int num_target_boxes_hit = 0;
    // The muon rate
    //  - fArea is in mm (internal G4 units) so need to convert to m
    G4double adjusted_rate = 1.0; //fFluxIntegrated*fArea/fSpeedUp;//< Adjust this rate if we are only sampling a smaller portion of the energy-angle PDF
    // G4cout << "Adj. Rate : " << adjusted_rate << G4endl;

    do {
        throws++;
        // Sample point and direction
        SamplePositionAndDirection(position, direction);

        //        position[0] = fSourceBoxPosition[0] + fSourceBoxWidth[0] * (-1.0 + 2.0 * G4UniformRand()) ;
        //        position[1] = fSourceBoxPosition[1] + fSourceBoxWidth[1] * (-1.0 + 2.0 * G4UniformRand()) ;
        //        position[2] = fSourceBoxPosition[2] + fSourceBoxWidth[2] ;

        // Keep track of global throws for integral
        fNThrows++;

        // If no target boxes defined all events are good
        if (fTargetBoxes.empty()) break;

        // If target boxes defined only save trajectories that hit at least one
        for (uint i = 0; i < fTargetBoxes.size(); i++) {

            G4double d = (fTargetBoxes.at(i))->DistanceToIn(
                             position - fTargetBoxPositions.at(i), direction);


            if (d != kInfinity) {
                // Increment the counter for the number of target boxes hit
                num_target_boxes_hit++;

                // Check whether we have the required number of hits
                if (num_target_boxes_hit >= fSourceBoxRequireN ) {
                    good_event = true;
                    break;
                }
            }

            // Regardless of whether the event is accepted increment the time
            fParticleTime -= std::log(1 - G4UniformRand()) * (1.0 / adjusted_rate);


        }// End for



        if (throws >= 1E8) {
            std::cout << "Failed to find any good events in 1E6 tries!" << std::endl;
            throw;
        }

    } while (!good_event and throws < 1E8);

    // Get exposure time
    // std::cout << "Exposure Time = " << fNThrows << "," << fArea << " : " << fNThrows / fFluxIntegrated << " m^{2} s" << std::endl;
    // std::cout << "Exposure Time : " << fNThrows / fFluxIntegrated / fArea << " s" << std::endl;

    // Generate Primary
    // std::cout << "EventID " << anEvent->GetEventID() << G4endl;
    // std::cout << " --> Position " << position << G4endl;
    // std::cout << " --> Direction " << direction << G4endl;
    // std::cout << " --> Energy " << E*1000 << G4endl;
    // std::cout << " --> Global Time " << fParticleTime << G4endl;


    /// This is incorrect. Need to incremenent the muon exposure time each throw using random stuff from Chris's code.
    // fParticleTime = fNThrows / fFluxIntegrated / fArea;
    fParticleDir = direction;
    fParticlePos = position;
    fParticleEnergy = E;

    fParticleGun->SetParticleEnergy(fParticleEnergy);
    fParticleGun->SetParticleTime(fParticleTime);
    fParticleGun->SetParticleMomentumDirection(fParticleDir);
    fParticleGun->SetParticlePosition(fParticlePos);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    Draw();

    return;
}
//---------------------------------------------------------------------------------


//------------------------------------------------------------------
IsotropicSphereFluxProcessor::IsotropicSphereFluxProcessor(IsotropicSphereGenerator* gen, bool autosave) :
    VFluxProcessor("iso"), fSave(autosave)
{
    fGenerator = gen;
}

bool IsotropicSphereFluxProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    std::string tableindex = "iso";
    std::cout << "FLX: Registering IsotropicSphereFluxProcessor NTuples " << tableindex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill index energy
    fParticleTimeIndex   = man ->CreateNtupleDColumn(tableindex + "_t");
    fParticleEnergyIndex = man ->CreateNtupleDColumn(tableindex + "_E");
    fParticleDirXIndex   = man ->CreateNtupleDColumn(tableindex + "_dx");
    fParticleDirYIndex   = man ->CreateNtupleDColumn(tableindex + "_dy");
    fParticleDirZIndex   = man ->CreateNtupleDColumn(tableindex + "_dz");
    fParticlePosXIndex   = man ->CreateNtupleDColumn(tableindex + "_x");
    fParticlePosYIndex   = man ->CreateNtupleDColumn(tableindex + "_y");
    fParticlePosZIndex   = man ->CreateNtupleDColumn(tableindex + "_z");
    fParticlePDGIndex    = man ->CreateNtupleIColumn(tableindex + "_pdg");

    return true;
}

bool IsotropicSphereFluxProcessor::ProcessEvent(const G4Event* /*event*/) {

    // Register Trigger State
    fHasInfo = true;
    fTime    = fGenerator->GetParticleTime();
    fEnergy  = fGenerator->GetParticleEnergy();

    // Set Ntuple to defaults

    if (fHasInfo) {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fParticleTimeIndex,   fGenerator->GetParticleTime());
        man->FillNtupleDColumn(fParticleEnergyIndex, fGenerator->GetParticleEnergy());
        man->FillNtupleDColumn(fParticleDirXIndex,   fGenerator->GetParticleDir().x() / MeV);
        man->FillNtupleDColumn(fParticleDirYIndex,   fGenerator->GetParticleDir().y() / MeV);
        man->FillNtupleDColumn(fParticleDirZIndex,   fGenerator->GetParticleDir().z() / MeV);
        man->FillNtupleDColumn(fParticlePosXIndex,   fGenerator->GetParticlePos().x() / m);
        man->FillNtupleDColumn(fParticlePosYIndex,   fGenerator->GetParticlePos().y() / m);
        man->FillNtupleDColumn(fParticlePosZIndex,   fGenerator->GetParticlePos().z() / m);
        man->FillNtupleIColumn(fParticlePDGIndex ,   fGenerator->GetParticlePDG());
        return true;
    } else {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fParticleTimeIndex, -999.);
        man->FillNtupleDColumn(fParticleEnergyIndex, -999.);
        man->FillNtupleDColumn(fParticleDirXIndex, -999.);
        man->FillNtupleDColumn(fParticleDirYIndex, -999.);
        man->FillNtupleDColumn(fParticleDirZIndex, -999.);
        man->FillNtupleDColumn(fParticlePosXIndex, -999.);
        man->FillNtupleDColumn(fParticlePosYIndex, -999.);
        man->FillNtupleDColumn(fParticlePosZIndex, -999.);
        man->FillNtupleIColumn(fParticlePDGIndex,  -999 );
        return false;
    }
    return true;
}

G4double IsotropicSphereFluxProcessor::GetExposureTime() {
    return fGenerator->GetParticleTime();
}
//---------------------------------------------------------------------------------

} // - namespace COSMIC
