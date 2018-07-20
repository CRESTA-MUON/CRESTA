#include "LineOfSightSD.hh"
#include "flux/LOSPrimaryGenerator.hh"
#include "analysis/Analysis.hh"
#include "db/DBTable.hh"
#include "G4EmCalculator.hh"


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


namespace COSMIC {

//------------------------------------------------------------------
LineOfSightSD::LineOfSightSD(DBTable tbl):
  VDetector(tbl.GetIndexName(), "LineOfSight")
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  // std::cout << "TRUE LineOfSight PROCESSOR : " << tbl->GetI("processor") << std::endl;
  if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
    Analysis::Get()->RegisterProcessor(new LineOfSightProcessor(this));
  }

  fCheckedCutOff = false;

  fCalc = new G4EmCalculator();

}

LineOfSightSD::LineOfSightSD(std::string name, std::string id, bool autoprocess, bool autosave):
  VDetector(name, id)
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (autoprocess) {
    Analysis::Get()->RegisterProcessor(new LineOfSightProcessor(this, autosave));
  }

  fCheckedCutOff = false;

  fCalc = new G4EmCalculator();

}

void LineOfSightSD::SetCutOffDepth(){

  // Get the target plane to stop the LOS integration of density
  LOSPrimaryGenerator* LOS_gen = (LOSPrimaryGenerator*)  (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  // // Extract the target boxes
  // std::vector<G4ThreeVector> target_box = LOS_gen->GetTargetBoxPositions();
  //
  // // Save the cuttoff depth
  // fZCutOff = (target_box.at(0))[2];

  // std::cout << "LOS: Vertical Cut-Off at " << fZCutOff/cm << "cm " << std::endl;

  fCheckedCutOff = true;
}

void LineOfSightSD::ResetState() {

  VDetector::ResetState();
  fLOSMass=0;
  fEnergy = 0;
  fMaterial = 0;

}

G4bool LineOfSightSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

  // Get the cutoff depth
  if(!fCheckedCutOff) SetCutOffDepth();

  // Get the material, density and KE
  fMaterial = (step->GetTrack())->GetMaterial();
  G4double density = fMaterial->GetDensity()/(g/cm3);
  fEnergy = (step->GetPreStepPoint())->GetKineticEnergy();

  // Check the depth
  G4ThreeVector pos = (step->GetPreStepPoint())->GetPosition();
  G4double step_length = step->GetStepLength()/cm;
  // Get the step length
  fLOSMass += density*step_length;// in g/cm2

  // Get the particle table
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  // Calculate dE/dX for this particle and detector
  // - Convert dEdx MeV/mm to MeV / g cm-2
  fdedx = (cm/density)*fCalc->GetDEDX(fEnergy,particleTable->FindParticle("mu+"), fMaterial);// density effect differences between +/- muons?
  fRange = density*(fCalc->GetRange(fEnergy, particleTable->FindParticle("mu+"), fMaterial)/cm);// density effect differences between +/- muons?

  // if(step_length<fRange){
  //   LOSPrimaryGenerator* LOS_gen = (LOSPrimaryGenerator*)  (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  //   fEventRate = (LOS_gen->GetMuonAnglePDF())
  //                 * (LOS_gen->GetMuonEnergyPDF())
  //                 * CLHEP::twopi
  //                 * LOS_gen->GetVerticalRate();
  // }// End if

}// End function







//------------------------------------------------------------------
LineOfSightProcessor::LineOfSightProcessor(LineOfSightSD* trkr, bool autosave) :
  VProcessor(trkr->GetID()), fSave(autosave)
{
  fTracker = trkr;
}

bool LineOfSightProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = GetID();
  std::cout << "DET: Registering LineOfSightProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fLineOfSightMassIndex = man ->CreateNtupleDColumn(tableindex + "_los");
  fDEDxIndex = man ->CreateNtupleDColumn(tableindex + "_dEdx");
  fRangeIndex= man ->CreateNtupleDColumn(tableindex + "_range");

  return true;
}

bool LineOfSightProcessor::ProcessEvent(const G4Event* /*event*/) {

    fEnergy = fTracker->GetEnergy();

    // Check data
    if(fTracker->GetEnergy()>0){

    // Fill LineOfSight vectors
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fLineOfSightMassIndex, fTracker->GetLOSMass());


    // Save these values out
    man->FillNtupleDColumn(fDEDxIndex ,fTracker->GetDEDX());
    man->FillNtupleDColumn(fRangeIndex , fTracker->GetRange());

  } else {

    // Fill LineOfSight vectors
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fLineOfSightMassIndex, -999);
    man->FillNtupleDColumn(fDEDxIndex ,-999);
    man->FillNtupleDColumn(fRangeIndex ,-999);
  }



    // LineOfSightProcessor::DrawEvent();

    return true;

}

void LineOfSightProcessor::DrawEvent(){
  // Draw Track if in interactive
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {

      // G4ThreeVector LineOfSightpos = fTracker->GetAveragePosition();
      //
      //   G4Circle LineOfSightMarker(LineOfSightpos);
      //
      //   G4Colour colour(1., 0., 0.);
      //   G4VisAttributes attribs(colour);
      //   LineOfSightMarker.SetVisAttributes(attribs);
      //
      //   pVVisManager->Draw(LineOfSightMarker);

  }
}

} // - namespace COSMIC
