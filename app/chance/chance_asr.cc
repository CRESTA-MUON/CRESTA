/**
 * @file rat.cc
 * @author Stan Seibert <volsung@physics.utexas.edu>
 */
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "sd/LongDriftSD.hh"
#include "sd/SimpleScintillatorSD.hh"
#include "TH1.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TVirtualFitter.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>

#include <G4UImanager.hh>
#include <G4UIterminal.hh>
#include <G4UItcsh.hh>
#include <Randomize.hh>
#include <globals.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>

// time.h must come after Randomize.hh on RedHat 7.3
#include <time.h>

#include "db/DB.hh"
#include "sd/DetectorConstruction.hh"
#include "action/CosmicActionInitialization.hh"
#include "physics/PhysicsFactory.hh"
#include <TStopwatch.h>
#include <TRandom.h>
#include <iostream>
#include <string>

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
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
#include "G4Polyline.hh"
#include "Math/Functor.h"

#include "db/ROOTHeaders.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"
#include "sd/DetectorManager.hh"



#include "G4UImanager.hh"

#include "Randomize.hh"
//#ifdef G4MULTITHREADED
//#include "G4MTRunManager.hh"
//#else
#include "G4RunManager.hh"
//#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#include "g4root_defs.hh"
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif
#include "globals.hh"
#include "action/CosmicRun.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"

#include <ctime>
#include <iostream>

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "analysis/Analysis.hh"

#include "chance/BristolPoCAFitter.hh"
#include "TVector3.h"

#include "TROOT.h"
#include "TMinuit.h"
#include "TFitter.h"
#include "TF1.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TVector3.h"
#include "TMatrixT.h"
#include "TMatrixDEigen.h"
#include "TVirtualFFT.h"
#include "TProfile.h"
#include "TRandom.h"
#include <map>
#include "TH3D.h"

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnUserParameters.h"

#include "chance/EventVertex.hh"
#include "chance/BackProjectionGrid.hh"

using namespace std;
using namespace COSMIC;


std::vector<std::string> gInputFiles;
std::string gOutputTag = "output";
int gSmearingOption = 0;
///-------------------------------------------------------------------------------------------
void PrintHelpScreen() {

  std::cout << "USAGE" << std::endl << std::endl;


  exit(0);

}




// Have option to pass any set of files or a list of files
// The MuonGrid just gets given a new hit, so doesn't care about rotations itself.
// RotationManager handles that in the list. Applies rotation to track parameters themselves.
// Whole analysis runs over a for loop. Tells the BackProjectionGrid, process voxels A->B, then B->C,
// Update the corresponding grids accordingly.




///-------------------------------------------------------------------------------------------
int main(int argc, char** argv) {

  // Print Splash Screen
  DB::PrintSplashScreen();

  std::string defaultconfig = DB::GetDataPath() + "/chance/discriminator_config.geo";

  // Get User Inputs
  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Getting User Inputs" << std::endl;

  // Loop over all arguments
  for (int i = 1; i < argc; ++i) {

    if (std::strcmp(argv[i], "-i") == 0) {
      gInputFiles.push_back(std::string(argv[++i]));

    } else if (std::strcmp(argv[i], "-o") == 0) {
      gOutputTag = std::string(argv[++i]);

    } else if (std::strcmp(argv[i], "-c") == 0) {
      defaultconfig = std::string(argv[++i]);

    } else if (std::strcmp(argv[i], "-h") == 0) {
      PrintHelpScreen();

      // Uknown command
    } else {
      std::cout << "Unknown COMMAND : " << argv[i] << std::endl;
      throw;
    }
  }

  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Reading Config" << std::endl;
  DB *rdb = DB::Get();
  rdb->LoadFile(defaultconfig);
  rdb->Finalise();

  DBTable configuration = rdb->GetTable("DISCRIMINATOR", "config");

  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Beginning Input Loop" << std::endl;

  // Read in input Trees
  TChain* t = new TChain("T");
  for (int i = 0; i < gInputFiles.size(); i++) {
    t->AddFile(gInputFiles[i].c_str());
  }
  t->SetCacheSize(100000000);

  Double_t fScattering[31] = {0.};
  Double_t fPOCAScattering[6] = {0.};
  Double_t fMCTruth[7] = {0.};
  double fCovarMatrix[49];

  t->SetBranchAddress("fitscattering", fScattering);
  t->SetBranchAddress("pocascattering", fPOCAScattering);
  t->SetBranchAddress("mctruth", fMCTruth);
  t->SetBranchAddress("covarmatrix", fCovarMatrix);
  int maxsize = 0;

  int ntracks = t->GetEntries();
  int printsi = ntracks / 100;

  BackProjectionGrid scangrid = BackProjectionGrid(configuration.GetD("grid_sizex"),
                                configuration.GetD("grid_minx"),
                                configuration.GetD("grid_maxx"),
                                configuration.GetD("grid_sizey"),
                                configuration.GetD("grid_miny"),
                                configuration.GetD("grid_maxy"),
                                configuration.GetD("grid_sizez"),
                                configuration.GetD("grid_minz"),
                                configuration.GetD("grid_maxz"));

  scangrid.SetGridOffsets(configuration.GetD("grid_offsetx"),
                          configuration.GetD("grid_offsety"),
                          configuration.GetD("grid_offsetz"));

  scangrid.SetPointOffsets(configuration.GetD("point_offsetx"),
                           configuration.GetD("point_offsety"),
                           configuration.GetD("point_offsetz"));


  double chosenmomentum = 0.0;
  if (configuration.Has("force_momentum")) chosenmomentum = configuration.GetD("force_momentum");
  double smearmomentum = 0.0;
  if (configuration.Has("smear_momentum")) smearmomentum = configuration.GetD("smear_momentum");

  gSmearingOption = 0;
  if (configuration.Has("smearing")) gSmearingOption = configuration.GetI("smearing");

  int totchunks = 10;

  for (int ichunk = 0; ichunk < totchunks; ichunk++){

    std::cout << "Processing Voxel Chunk " << ichunk << std::endl;
    scangrid.SetVoxelChunk(ichunk, totchunks);

    for (int i = 0; i < ntracks; i++) {

      if (i % printsi == 0) std::cout << "Processed " << i << " / " << ntracks << " : " << int(100.0 * float(i)/float(ntracks)) << std::endl;

      // Get the entry
      t->GetEntry(i);

      // Get the Voxel
      double scatteranglex = fScattering[0];
      double scatterangley = fScattering[1];
      double scatterangle3d = fScattering[2];

      double vertexx = fScattering[11];
      double vertexy = fScattering[12];
      double vertexz = fScattering[13];

      double momentum = fMCTruth[1];
      if (chosenmomentum != 0.0) momentum = chosenmomentum;
      if (smearmomentum != 0.0)  momentum += G4RandGauss::shoot(0.0, smearmomentum * momentum);

      double errorx = sqrt(fCovarMatrix[0]);
      double errory = sqrt(fCovarMatrix[8]);
      double errorz = sqrt(fCovarMatrix[16]);

      double px1 = fScattering[15];
      double py1 = fScattering[17];
      double px2 = fScattering[14];
      double py2 = fScattering[16];

      // Get the ID
      int voxelid = scangrid.GetVoxelID(vertexx, vertexy, vertexz);

      // Skip bad voxels
      if (voxelid < 0) continue;

      // Make Track Object
      EventTracks newtrack;
      newtrack.x = vertexx;
      newtrack.y = vertexy;
      newtrack.z = vertexz;
      newtrack.thx = scatteranglex;
      newtrack.thy = scatterangley;
      newtrack.th  = scatterangle3d;
      newtrack.mom = momentum;
      newtrack.px1 = px1;
      newtrack.py1 = py1;
      newtrack.px2 = px2;
      newtrack.py2 = py2;

      scangrid.AddVertexToValidVoxels(newtrack);
    }

    scangrid.UpdateValidVoxelChunks();
  }

  TFile* ofile = new TFile((gOutputTag + ".discriminator.root").c_str(), "RECREATE");
  ofile->cd();
  scangrid.Write();
  ofile->Close();

  return 0;
}



