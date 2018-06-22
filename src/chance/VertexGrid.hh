#ifndef __VertexGrid_HH__
#define __VertexGrid_HH__

#include "EventVertex.hh"

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
#include "sd/LongDriftSD.hh"
#include "chance/AWEDriftSD.hh"
#include "chance/BristolRPCSD.hh"



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



class VertexGrid {
public:
  VertexGrid(DBTable table);
  VertexGrid(float gridsize, float minx, float maxx, float miny, float maxy, float minz, float maxz);
  ~VertexGrid();

  int GetVoxelID(float x, float y, float z);

  bool IsValidVertex(EventVertex& vertex);
  void SetOffsets(float xoff, float yoff, float zoff);
  void AddVertexToVoxel(uint voxelid, EventVertex vertex);
  void AddVertex(EventVertex vertex);

  void Write();

  // float gGridSize = 10;
  // float gXMIN = -1000.0;
  // float gXMAX =  1000.0;
  // float gXOFF = -250;
  // float gYMIN = -1000.0;
  // float gYMAX =  1000.0;
  // float gYOFF = -250;
  // float gZMIN = -400.0;
  // float gZMAX =  400.0;
  // float gZOFF =  0;

  float fGridSize;
  float fXMIN;
  float fXMAX;
  float fXOFF;
  float fYMIN;
  float fYMAX;
  float fYOFF;
  float fZMIN;
  float fZMAX;
  float fZOFF;

  int fZnbins;
  int fXnbins;
  int fYnbins;

  std::map< uint, std::vector<EventVertex> > fVertices;

};

#endif
