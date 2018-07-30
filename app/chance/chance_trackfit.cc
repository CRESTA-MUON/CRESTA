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

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnUserParameters.h"
#include "chance/TrueMCReader.hh"

using namespace std;
using namespace COSMIC;

bool debugflag = false;

// Run Inputs
std::string gRunTag = "trackfit";
int gRunID = 0;
int gSubRunID = 0;

long int gSeed = -1; ///< Seed. -1 means generate from time+pid

/// Mode : Interactive
bool gInteractive = false; //< Run interactive viewer

// Mode : generate N events
int gNEvents = -1; ///< Number of events to generate regardless of trigger

// Mode : generate N triggers
int gNTriggers = -1; ///< Number of triggered events to generate

// Mode : Generate t exposure
int gExposureTime = -1; ///< Number of seconds exposure to generate

int gProcessingChunks = 10000; ///< By default what the chunk size should be

std::string gInputFile;
std::string gOutputTag = "trackfit";

enum {
  kUseAll,
  kUseRPC,
  kUseDrift
};

int gInputMode = kUseAll;

// User Defined Geometry files
std::vector<std::string> gGeomtryFiles;

///-------------------------------------------------------------------------------------------
void FillComboVect(std::vector<std::vector<bool> >& combomap, int n) {

  for (int i = 0; i < (1 << n); i++) {
    std::vector<bool> combo;
    for (int j = 0; j < n; j++) {
      combo.push_back(((i & (1 << j)) ? 1 : 0));
    }
    combomap.push_back(combo);
  }

}

///-------------------------------------------------------------------------------------------
void PrintHelpScreen() {

  std::cout << "USAGE" << std::endl << std::endl;
  std::cout << " chance_trackfit -i inputfile [-o outputtag]" << std::endl << std::endl;
  std::cout << " -i inputfile : Input MC file to run trackfit over" << std::endl;
  std::cout << " -o outputtag : Output tag. Output file becomes inputfile.outputtag.root. Default is 'trackfit'" << std::endl;

  exit(0);

}


///-------------------------------------------------------------------------------------------
int main(int argc, char** argv) {

  // Print Splash Screen
  DB::PrintSplashScreen();
  std::string defaultconfig = DB::GetDataPath() + "/chance/trackfit_config.geo";


  // Get User Inputs
  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Getting User Inputs" << std::endl;

  // Loop over all arguments
  for (int i = 1; i < argc; ++i) {

    // N Events input " -n nevents"
    if (std::strcmp(argv[i], "-i") == 0) {
      gInputFile = std::string(argv[++i]);

      // N Triggers Input " -j ntriggers"
    } else if (std::strcmp(argv[i], "-o") == 0) {
      gOutputTag = std::string(argv[++i]);

    } else if (std::strcmp(argv[i], "-c" == 0)) {
      defaultconfig = std::string(argv[++i]);

    } else if (std::strcmp(argv[i], "--rpc-only") == 0) {
      gInputMode = kUseRPC;

    } else if (std::strcmp(argv[i], "--drift-only") == 0) {
      gInputMode = kUseDrift;

      // Print Splash Screen
    } else if (std::strcmp(argv[i], "-h") == 0) {
      PrintHelpScreen();

      // Uknown command
    } else {
      std::cout << "Unknown COMMAND : " << argv[i] << std::endl;
      throw;
    }
  }

  // std::cout << "========================================= " << std::endl;
  std::cout << "APP: Reading Configuration" << std::endl;
  // bool fUseRPC = true;
  // bool fUseDrift = true;
  int fMinARPCX = 2;
  int fMinARPCY = 2;
  int fMinBRPCX = 2;
  int fMinBRPCY = 2;
  int fMinADriftX = 3;
  int fMinADriftY = 3;
  int fMinBDriftX = 3;
  int fMinBDriftY = 3;
  if (gInputMode == kUseRPC) {
    fMinARPCX = 3;
    fMinARPCY = 3;
    fMinBRPCX = 3;
    fMinBRPCY = 3;
  }

  // std::cout << "========================================= " << std::endl;
  std::cout << "APP: Loading CONFIG : " << defaultconfig << std::endl;

  DB *rdb = DB::Get();
  rdb->LoadFile(defaultconfig);
  rdb->Finalise();

  DBTable configuration = rdb->GetTable("TRACKFIT", "config");

  // std::cout << "========================================= " << std::endl;
  std::cout << "APP: Beginning Input Loop" << std::endl;

  // Read in input Trees
  TFile* f = new TFile(gInputFile.c_str(), "READ");
  TTree* t = (TTree*)f->Get("detectorevents");
  // std::cout << "========================================= " << std::endl;
  std::cout << "APP: Reading Event Inputs" << std::endl;

  std::string prefixa = "det_above";
  std::string prefixb = "det_below";

  BristolPoCAFitter* pocafit = new BristolPoCAFitter();
  pocafit->ReadInputTTree(t, prefixa, prefixb);


  // List of configs we care about
  // - Resolution
  // - Use RPC or Use Drift
  if (gInputMode == kUseRPC) {
    pocafit->SetUseRPC(true);
    pocafit->SetUseDrift(true);

  } else if (gInputMode == kUseDrift) {
    pocafit->SetUseRPC(true);
    pocafit->SetUseDrift(true);
  } else {
    pocafit->SetUseRPC(true);
    pocafit->SetUseDrift(true);
  }

  // Read Truth Trees
  // TBranch* truthbranch = (TBranch*) t->GetBranch((prefixa + "_trueexit_t").c_str());
  // if (truthbranch) {
  //   // std::cout << "========================================= " << std::endl;
  //   std::cout << "APP: Reading Truth Inputs" << std::endl;
  // }
  bool fIsMC = true;
  TrueMCReader* truefit = NULL;
  if (fIsMC) {
    truefit = new TrueMCReader();
    truefit->ReadInputTTree(t, prefixa, prefixb);
  }

  t->SetCacheSize(100000000);


  // Make output File
  // std::cout << "========================================= " << std::endl;
  std::cout << "APP: Creating Output Tree" << std::endl;

  std::string strippedinput = gInputFile;
  // strippedinput.erase( strippedinput.find(".root"), 4);
  TFile* o = new TFile((strippedinput + "." + gOutputTag + ".root").c_str(), "RECREATE");
  TTree* otree = new TTree("T", "T");

  double fBestFitPars[7];
  double fCovarMatrix[49];
  double fMinuitParams[6];
  // double fErrorDef;

  Double_t fScattering[31] = {0.};
  Double_t fPOCAScattering[6] = {0.};
  Double_t fMCTruth[7] = {0.};

  std::string prefix = "fit";

  std::string bestfitparslab = "trackfit";
  std::string bestfitparsval = "x/D:y:z:px1:py1:px2:py2";
  otree->Branch(bestfitparslab.c_str(), fBestFitPars, bestfitparsval.c_str() );

  std::string minuitparslab = "minuitparams";
  std::string minuitparsval = "fmin/D:edm:errdef:nvpar:nparx:istat";
  otree->Branch(minuitparslab.c_str(), fMinuitParams, minuitparsval.c_str());

  std::string covariancelab = "covarmatrix";
  std::string covarianceval = "";
  covarianceval += "c00/D:c01:c02:c03:c04:c05:c06:";
  covarianceval += "c10:c11:c12:c13:c14:c15:c16:";
  covarianceval += "c20:c21:c22:c23:c24:c25:c26:";
  covarianceval += "c30:c31:c32:c33:c34:c35:c36:";
  covarianceval += "c40:c41:c42:c43:c44:c45:c46:";
  covarianceval += "c50:c51:c52:c53:c54:c55:c56:";
  covarianceval += "c60:c61:c62:c63:c64:c65:c66";
  otree->Branch(covariancelab.c_str(), fCovarMatrix, covarianceval.c_str());

  otree->Branch("fitscattering", fScattering, "scatteranglex/D:scatterangley:scatterangle3d:uoffsetx:uoffsety:loffsetx:loffsety:offset3d:vlongerror:vtranserror1:vtranserror2:vx:vy:vz:px1:px2:py1:py2:uxchi2:uychi2:lxchi2:lychi2:xchi2:ychi2:CFchi2:uxpoca:lxpoca:uypoca:lypoca:estp:estpcorr");
  otree->Branch("pocascattering", fPOCAScattering, "scatteranglex/D:scatterangley:scatterangle3d:vx:vy:vz");
  otree->Branch("mctruth", fMCTruth, "energy/D:p:px:py:pz:v:scatterangle:passflag");


  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Event Loop" << std::endl;

  // ROOT::Minuit2::MnPrint::SetLevel(-1);
  gErrorIgnoreLevel = 1001;

  std::vector<bool> combobx;
  std::vector<bool> comboby;
  std::vector<bool> comboay;
  std::vector<bool> comboax;

  int savecount = 0;
  int n = t->GetEntries();

  int nbadtotal = 0;
  int nbadsingle = 0;
  int nlowrpcs = 0;
  int ndriftrpcs = 0;

  // Loop over all entries
  for (int i = 0; i < t->GetEntries(); i++) {

    // Read Event
    t->GetEntry(i);
    pocafit->PreProcessData();

    if (i % 20000 == 0) std::cout << "Processed " << i << "/" << n << " events. Saved : " << savecount << std::endl;
    // std::cout << " New Event " << std::endl;

    // Apply NHit cuts
    if (gInputMode == kUseRPC || gInputMode == kUseAll) {
      if (pocafit->GetNAboveRPCX()   != fMinARPCX ||
          pocafit->GetNAboveRPCY()   != fMinARPCY ||
          pocafit->GetNBelowRPCX()   != fMinBRPCX ||
          pocafit->GetNBelowRPCY()   != fMinBRPCY) {

        nlowrpcs++;
        continue;
      }
    }

    if (gInputMode == kUseDrift || gInputMode == kUseAll) {
      if (pocafit->GetNAboveDriftX() != fMinADriftX ||
          pocafit->GetNAboveDriftY() != fMinADriftY ||
          pocafit->GetNBelowDriftX() != fMinBDriftX ||
          pocafit->GetNBelowDriftY() != fMinBDriftY) {

        ndriftrpcs++;
        continue;
      }
    }

    // Get the best fit drift combinations for this event
    if (pocafit->GetNAboveDriftX()) {
      pocafit->GetBestComboForDriftHits(kDriftAboveX);
    }

    if (pocafit->GetNBelowDriftX()) {
      pocafit->GetBestComboForDriftHits(kDriftBelowX);
    }

    if (pocafit->GetNAboveDriftY()) {
      pocafit->GetBestComboForDriftHits(kDriftAboveY);
    }

    if (pocafit->GetNBelowDriftY()) {
      pocafit->GetBestComboForDriftHits(kDriftBelowY);
    }

    // Run an upper and lower single track fit here
    double pocafitparams[16] = {0.};
    pocafit->PerformDoubleTrackPoCAFit(pocafitparams);
    double stf_above_x  = pocafitparams[0];
    double stf_above_px = pocafitparams[1];

    double stf_above_y  = pocafitparams[2];
    double stf_above_py = pocafitparams[3];

    double stf_below_x  = pocafitparams[4];
    double stf_below_px = pocafitparams[5];

    double stf_below_y  = pocafitparams[6];
    double stf_below_py = pocafitparams[7];

    // Save PoCA Results
    fPOCAScattering[0] = pocafitparams[8]; // ScatterX
    fPOCAScattering[1] = pocafitparams[9]; // ScatterY
    fPOCAScattering[2] = pocafitparams[10]; // Scatter3D

    fPOCAScattering[3] = pocafitparams[11]; // fitVx;
    fPOCAScattering[4] = pocafitparams[12]; // fitVy;
    fPOCAScattering[5] = pocafitparams[13]; // fitVz;

    // Apply Cuts to fits
    double chi2arpcx = pocafit->EvaluateTrackResidual( stf_above_x, stf_above_px, 0.0, kRPCAboveX);
    double chi2arpcy = pocafit->EvaluateTrackResidual( stf_above_y, stf_above_py, 0.0, kRPCAboveY);
    double chi2brpcx = pocafit->EvaluateTrackResidual( stf_below_x, stf_below_px, 0.0, kRPCBelowX);
    double chi2brpcy = pocafit->EvaluateTrackResidual( stf_below_y, stf_below_py, 0.0, kRPCBelowY);

    double chi2adriftx = pocafit->EvaluateTrackResidual( stf_above_x, stf_above_px, 0.0, kDriftAboveX);
    double chi2adrifty = pocafit->EvaluateTrackResidual( stf_above_y, stf_above_py, 0.0, kDriftAboveY);
    double chi2bdriftx = pocafit->EvaluateTrackResidual( stf_below_x, stf_below_px, 0.0, kDriftBelowX);
    double chi2bdrifty = pocafit->EvaluateTrackResidual( stf_below_y, stf_below_py, 0.0, kDriftBelowY);

    fScattering[18] = chi2arpcx + chi2adriftx; // event.xUchi2;
    fScattering[19] = chi2arpcy + chi2adrifty; // event.yUchi2;
    fScattering[20] = chi2brpcx + chi2bdriftx; // event.xLchi2;
    fScattering[21] = chi2brpcy + chi2bdrifty; // event.yLchi2;

    fScattering[22] = pocafitparams[14]; //chi2arpcx + chi2adriftx + chi2brpcx + chi2bdriftx; // event.xchi2;
    fScattering[23] = pocafitparams[15]; //chi2arpcy + chi2adrifty + chi2brpcy + chi2bdrifty; // event.ychi2;
    fScattering[24] = fMinuitParams[0]; // chi2


    double ChiSquareCut3Points = 2000;
    if (gInputMode != kUseAll) ChiSquareCut3Points = 1000;

    if (fScattering[18] > ChiSquareCut3Points ||
        fScattering[19] > ChiSquareCut3Points ||
        fScattering[20] > ChiSquareCut3Points ||
        fScattering[21] > ChiSquareCut3Points) {
      nbadsingle++;

      // std::cout << " Ind Chi2 : " << fScattering[18] << " " << fScattering[19] << " " << fScattering[20] << " " << fScattering[21] << std::endl;
      // pocafit->PrintCombos();
      // sleep(2);
      continue;
    }

    int npars = 7;
    ROOT::Minuit2::MnUserParameters mn_param;

    mn_param.Add("vx", 225.0, 10);
    mn_param.Add("vy", 225.0, 10);
    mn_param.Add("vz", -250.0, 10);
    mn_param.Add("px1", 0.0, 10);
    mn_param.Add("px2", 0.0, 10);
    mn_param.Add("py1", 0.0, 10);
    mn_param.Add("py2", 0.0, 10);


    // Minimisation
    ROOT::Minuit2::MnMigrad migrad( *pocafit, mn_param, 2 ); //Strategy 2
    ROOT::Minuit2::FunctionMinimum min = migrad();

    ROOT::Minuit2::MnAlgebraicSymMatrix MinCovarMatrix = min.Error().Matrix();
    ROOT::Minuit2::MnAlgebraicVector MinParams = min.Parameters().Vec();

    for (int j = 0; j < npars; j++) {
      fBestFitPars[j] = MinParams[j];
    }

    // end Minuit2
    Double_t istat = 0;
    if (min.HasPosDefCovar()) istat = 3;

    fMinuitParams[0] = pocafit->DoEval(fBestFitPars);
    fMinuitParams[1] = min.Edm();
    fMinuitParams[2] = min.Up();
    fMinuitParams[3] = 7;
    fMinuitParams[4] = 7;
    fMinuitParams[5] = istat;



    for (Int_t i = 0; i < 7; ++i) {
      for (Int_t j = 0; j < 7; ++j) {
        Int_t pos = (i * 7) + j;
        fCovarMatrix[pos] = MinCovarMatrix(i, j);
      }
    }



    double CombinedChi2Cut = 2000;
    if (gInputMode != kUseAll) CombinedChi2Cut = 1000;
    if (fMinuitParams[0] > CombinedChi2Cut) {
      nbadtotal++;
      // std::cout << "Final Chi2 " << fMinuitParams[0] << std::endl;
      // pocafit->PrintCombos();
      continue;
    }

    // Get the covariance
    // Read params for calculations
    double fitVx  = fBestFitPars[0];
    double fitVy  = fBestFitPars[1];
    double fitVz  = fBestFitPars[2];
    double fitpx1 = fBestFitPars[3];
    double fitpx2 = fBestFitPars[4];
    double fitpy1 = fBestFitPars[5];
    double fitpy2 = fBestFitPars[6];


    TVector3 grad1X(fitpx1, 0., 1.);
    TVector3 grad2X(fitpx2, 0., 1.);

    TVector3 grad1Y(0., fitpy1, 1.);
    TVector3 grad2Y(0., fitpy2, 1.);

    TVector3 grad1(fitpx1, fitpy1, 1.);
    TVector3 grad2(fitpx2, fitpy2, 1.);

    fScattering[0] = grad1X.Angle(grad2X); // Scatter Angle X
    fScattering[1] = grad1Y.Angle(grad2Y); // Scatter Angle Y
    fScattering[2] = grad1.Angle(grad2); // Scatter Angle 3d

    double projux = (fitVx + fitpx1 * (fitVz - pocafit->GetLowestZ(kAboveX)));
    double projuy = (fitVy + fitpy1 * (fitVz - pocafit->GetLowestZ(kAboveY)));
    double projlx = (fitVx + fitpx2 * (fitVz - pocafit->GetHighestZ(kBelowX)));
    double projly = (fitVy + fitpy2 * (fitVz - pocafit->GetHighestZ(kBelowY)));

    double U_offsetX = fabs( pocafit->GetLowestHit(kAboveX)  - projux);
    double U_offsetY = fabs( pocafit->GetLowestHit(kAboveY)  - projuy);
    double L_offsetX = fabs( pocafit->GetHighestHit(kBelowX) - projlx);
    double L_offsetY = fabs( pocafit->GetHighestHit(kBelowY) - projly);

    fScattering[3] = U_offsetX; // Find lowest Z point in X
    fScattering[4] = U_offsetY; // Find lowest Z point in Y
    fScattering[5] = L_offsetX; // Find highest Z point in X
    fScattering[6] = L_offsetY; // Find highest Z point in Y

    // Unknown
    fScattering[7] = 0.0;

    // Get PoCA errors
    TMatrixD* covarV = new TMatrixD(7, 7, fCovarMatrix);
    covarV->ResizeTo(3, 3);

    TVector3 vec_w_tmp = grad1;
    vec_w_tmp += grad2;
    vec_w_tmp = vec_w_tmp.Unit();
    TVector3 vec_v_tmp = grad1.Cross(grad2);
    vec_v_tmp = vec_v_tmp.Unit();
    TVector3 vec_u_tmp = vec_w_tmp.Cross(vec_v_tmp);

    Double_t wXYZ[3] = {vec_w_tmp.X(), vec_w_tmp.Y(), vec_w_tmp.Z()};
    TVectorD vec_w(3, wXYZ);
    Double_t vXYZ[3] = {vec_v_tmp.X(), vec_v_tmp.Y(), vec_v_tmp.Z()};
    TVectorD vec_v(3, vXYZ);
    Double_t uXYZ[3] = {vec_u_tmp.X(), vec_u_tmp.Y(), vec_u_tmp.Z()};
    TVectorD vec_u(3, uXYZ);

    fScattering[8] = sqrt(covarV->Similarity(vec_w)); //V_long_error;
    fScattering[9] = sqrt(covarV->Similarity(vec_v)); //V_trans1_error;
    fScattering[10] = sqrt(covarV->Similarity(vec_u));  //V_trans2_error;

    // Get actual fit values again?!
    fScattering[11] = fitVx;
    fScattering[12] = fitVy;
    fScattering[13] = fitVz;
    fScattering[14] = fitpx2;
    fScattering[15] = fitpx1;
    fScattering[16] = fitpy2;
    fScattering[17] = fitpy1;

    // Calculate POCA Quantities
    double UPoCA_Vx = fabs(fitVx - (stf_above_x + stf_above_px * (0.0 - fitVz)));
    double UPoCA_Vy = fabs(fitVy - (stf_above_y + stf_above_py * (0.0 - fitVz)));
    double LPoCA_Vx = fabs(fitVx - (stf_below_x + stf_below_px * (0.0 - fitVz)));
    double LPoCA_Vy = fabs(fitVy - (stf_below_y + stf_below_py * (0.0 - fitVz)));

    // // PoCA of upper/lower tracks to CF vertex.
    fScattering[25] = UPoCA_Vx;
    fScattering[26] = LPoCA_Vx;
    fScattering[27] = UPoCA_Vy;
    fScattering[28] = LPoCA_Vy;

    // // Get Truth Information
    if (truefit) {
      fMCTruth[0] = truefit->GetTrueEnergy();
      fMCTruth[1] = truefit->GetTrueP();
      fMCTruth[2] = truefit->GetTruePX();
      fMCTruth[3] = truefit->GetTruePY();
      fMCTruth[4] = truefit->GetTruePZ();
      fMCTruth[5] = truefit->GetTrueV();
      fMCTruth[6] = truefit->GetScatterAngle();
      fMCTruth[7] = truefit->GetPassFlag();
    }

    // Fill this event
    otree->Fill();
    savecount++;

    // Clean up look sharp
    //delete min;
  }

  std::cout << "N Bad Total : " << nbadtotal << std::endl;
  std::cout << "N Bad Single : " << nbadsingle << std::endl;
  std::cout << "N Low RPCs : " << nlowrpcs << std::endl;
  std::cout << "N Drift RPCs : " << ndriftrpcs << std::endl;

  // Save outputs to TTree
  o->cd();
  otree->Write();
  o->Close();

  return 0;
}



