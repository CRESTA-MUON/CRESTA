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
#include "chance/VertexGrid.hh"

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
// Whole analysis runs over a for loop. Tells the VertexGrid, process voxels A->B, then B->C,
// Update the corresponding grids accordingly.
// Use Union if we want



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

    } else if (std::strcmp(argv[i], "-c" == 0)) {
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
  int printsi = ntracks / 10;

  VertexGrid scangrid = VertexGrid(configuration.GetD("grid_size"),
                                   configuration.GetD("grid_minx"),
                                   configuration.GetD("grid_maxx"),
                                   configuration.GetD("grid_miny"),
                                   configuration.GetD("grid_maxy"),
                                   configuration.GetD("grid_minz"),
                                   configuration.GetD("grid_maxz"));

  scangrid.SetOffsets(configuration.GetD("grid_offsetx"),
                      configuration.GetD("grid_offsety"),
                      configuration.GetD("grid_offsetz"));

  double chosenmomentum = 0.0;
  if (configuration.Has("force_momentum")) chosenmomentum = configuration.GetD("force_momentum");

  gSmearingOption = 0;
  if (configuration.Has("smearing")) gSmearingOption = configuration.GetI("smearing");


  for (int i = 0; i < ntracks; i++) {

    if (i % printsi == 0) std::cout << "Processed " << i << " / " << ntracks << std::endl;

    // Get the entry
    t->GetEntry(i);

    // Get the Voxel
    double scatteranglex = fScattering[0];
    double scatterangley = fScattering[1];
    double scatterangle3d = fScattering[2];

    // double vertexx = fScattering[11];
    // double vertexy = fScattering[12];
    // double vertexz = fScattering[13];
    double vertexx = fPOCAScattering[3];
    double vertexy = fPOCAScattering[4];
    double vertexz = fPOCAScattering[5];

    double momentum = fMCTruth[1];

    double errorx = sqrt(fCovarMatrix[0]);
    double errory = sqrt(fCovarMatrix[8]);
    double errorz = sqrt(fCovarMatrix[16]);

    // std::cout << "Starting to smear" << std::endl;

    // Get Smearing
    int nsmear = gSmearingOption;
    for (int j = 0; j < nsmear + 1; j++) {

      // Get smear
      double smearx = vertexx + (j > 0) * G4RandGauss::shoot(0.0, errorx);
      double smeary = vertexy + (j > 0) * G4RandGauss::shoot(0.0, errory);
      double smearz = vertexz; // + (j > 0) * G4RandGauss::shoot(0.0, errorz);

      // Get the ID
      int voxelid = scangrid.GetVoxelID(smearx, smeary, smearz);

      // std::cout << "Loading VoxelID " << voxelid << " " << smearx << " " << smeary << " " << smearz << std::endl;
      // Skip bad voxels
      if (voxelid < 0) continue;

      // Make Track Object
      EventVertex newtrack;
      newtrack.x = vertexx;
      newtrack.y = vertexy;
      newtrack.z = vertexz;
      newtrack.thx = scatteranglex;
      newtrack.thy = scatterangley;
      newtrack.th  = scatterangle3d;
      if (chosenmomentum > 0.0) newtrack.mom = chosenmomentum;
      else newtrack.mom = momentum;

      scangrid.AddVertexToVoxel(voxelid, newtrack);

    }
  }

  // // Now loop through our TH3D and set values
  // int nx = (gXMAX - gXMIN) / gGridSize;
  // int ny = (gYMAX - gYMIN) / gGridSize;
  // int nz = (gZMAX - gZMIN) / gGridSize;

  // TH3F* hist25 = new TH3F("hist25", "hist25", nx, gXMIN, gXMAX, ny, gYMIN, gYMAX, nz, gZMIN, gZMAX);
  // TH3F* hist50 = new TH3F("hist50", "hist50", nx, gXMIN, gXMAX, ny, gYMIN, gYMAX, nz, gZMIN, gZMAX);
  // TH3F* hist75 = new TH3F("hist75", "hist75", nx, gXMIN, gXMAX, ny, gYMIN, gYMAX, nz, gZMIN, gZMAX);
  // TH3I* histNS = new TH3I("histNS", "histNS", nx, gXMIN, gXMAX, ny, gYMIN, gYMAX, nz, gZMIN, gZMAX);
  // TH3F* histBS = new TH3F("histBS", "histBS", nx, gXMIN, gXMAX, ny, gYMIN, gYMAX, nz, gZMIN, gZMAX);


  // int totalbins = histNS->GetNbinsX() * histNS->GetNbinsY() * histNS->GetNbinsZ();
  // int printbins = totalbins / 10;
  // int curntbins = 0;

  // for (int i = 0; i < histNS->GetNbinsX(); i++) {
  //   double x = histNS->GetXaxis()->GetBinCenter(i + 1);
  //   for (int j = 0; j < histNS->GetNbinsY(); j++) {
  //     double y = histNS->GetYaxis()->GetBinCenter(j + 1);
  //     for (int k = 0; k < histNS->GetNbinsZ(); k++) {
  //       double z = histNS->GetZaxis()->GetBinCenter(k + 1);
  //       int voxelid = GetVoxelID(x, y, z);
  //       if (voxelid == -1) continue;
  //       if (discriminators.find(voxelid) == discriminators.end()) continue;

  //       std::vector<vertex> vertices = discriminators[voxelid];
  //       if (vertices.size() < 2) continue;

  //       std::vector<double> disc;
  //       for (int q = 0; q < vertices.size(); q++){
  //         disc.push_back(vertices[q].thx * vertices[q].mom);
  //         disc.push_back(vertices[q].thy * vertices[q].mom);
  //       }
  //       std::sort(disc.begin(), disc.end());
  //       hist25->SetBinContent(i + 1, j + 1, k + 1, disc[0.25 * disc.size()]);
  //       hist50->SetBinContent(i + 1, j + 1, k + 1, disc[0.50 * disc.size()]);
  //       hist75->SetBinContent(i + 1, j + 1, k + 1, disc[0.75 * disc.size()]);
  //       histNS->SetBinContent(i + 1, j + 1, k + 1, disc.size());

  //       double bristoldisc;
  //       disc.clear();
  //       for (int q = 0; q < vertices.size(); q++){
  //         for (int p = 0; p < vertices.size(); p++){
  //           if (p <= q) continue;
  //           vertex v1 = vertices[q];
  //           vertex v2 = vertices[p];
  //           float distance = sqrt(pow(v1.x - v2.x,2) + pow(v1.y - v2.y,2) + pow(v1.z - v2.z,2) );

  //           disc.push_back(distance / (v1.th*v1.mom) / (v2.th*v2.mom));
  //         }
  //       }
  //       std::sort(disc.begin(), disc.end());

  //       histBS->SetBinContent(i + 1, j + 1, k + 1, disc[disc.size()/2]);
  //       discriminators[voxelid].clear();

  //       curntbins++;
  //       if (curntbins % printbins == 0) std::cout << "Processed " << curntbins << "/" << totalbins << " voxels. " << std::endl;
  //     }
  //   }
  // }

  TFile* ofile = new TFile((gOutputTag + ".discriminator.root").c_str(), "RECREATE");
  ofile->cd();
  scangrid.Write();
  ofile->Close();

  return 0;
}



