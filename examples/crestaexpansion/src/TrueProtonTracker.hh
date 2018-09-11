#ifndef __TrueProtonTracker_HH__
#define __TrueProtonTracker_HH__

#include "G4UserRunAction.hh"
#include "globals.hh"

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
#include "G4GlobalMagFieldMessenger.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "g4root.hh"
#include "analysis/VProcessor.hh"
#include "analysis/VDetector.hh"

namespace CRESTA {

// Forward Declarations
class DBTable;

/// True Proton Tracker Detector Object :
/// Saves the highest momentrum muon track
/// that entered a detector logical voluem
class TrueProtonTracker : public VDetector {
public:

  /// Main constructor from a database table
  TrueProtonTracker(DBTable table);
  /// Simple C++ constructor
  TrueProtonTracker(std::string name, std::string id, bool autoprocess=true, bool autosave=true);
  /// Destructor
  ~TrueProtonTracker(){};

  /// Main processing. Looks for highest momentum track.
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  // Getter Functions
  inline G4double      GetProtonTime(){ return fProtonTime; };
  inline G4ThreeVector  GetProtonMom(){ return fProtonMom;  };
  inline G4ThreeVector  GetProtonPos(){ return fProtonPos;  };
  inline G4double       GetTotEDep(){ return fTotEDep;  };
  inline int            GetProtonPDG(){ return fProtonPDG;  };

protected:
  
  G4double      fProtonTime; ///< HM Proton Step Time
  G4ThreeVector fProtonMom;  ///< HM Proton Mom Vector
  G4ThreeVector fProtonPos;  ///< HM Proton Pos Vector
  G4double      fTotEDep;  ///< Total Energy Deposited this event
  int           fProtonPDG;  ///< Proton PDG Code
  int           fNProtonHits; //< Number of deposits

};

/// True Proton Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class TrueProtonProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  TrueProtonProcessor(TrueProtonTracker* trkr, bool autosave=true);
  /// Destructor
  ~TrueProtonProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Draw function
  void DrawEvent();
  
protected:

  TrueProtonTracker* fTracker; ///< Pointer to associated tracker SD

  bool fSave; ///< Flag to save event info automatically

  int fProtonTimeIndex; ///< Time Ntuple Index
  int fProtonMomXIndex; ///< MomX Ntuple Index
  int fProtonMomYIndex; ///< MomY Ntuple Index
  int fProtonMomZIndex; ///< MomZ Ntuple Index
  int fProtonPosXIndex; ///< PosX Ntuple Index
  int fProtonPosYIndex; ///< PosY Ntuple Index
  int fProtonPosZIndex; ///< PosZ Ntuple Index
  int fProtonPDGIndex;  ///< MPDG Ntuple Index

};


}
#endif

