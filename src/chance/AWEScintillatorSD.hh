#ifndef __AWEScintillatorSD_HH__
#define __AWEScintillatorSD_HH__

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
#include "sd/ScintillatorHit.hh"

#include "db/DBTable.hh"

namespace COSMIC {

/// True Muon Tracker Detector Object :
/// Saves the highest momentrum muon track
/// that entered a detector logical voluem
class AWEScintillatorSD : public VDetector {
public:

  /// Main constructor from a database table
  AWEScintillatorSD(DBTable table);
  /// AWE C++ constructor
  AWEScintillatorSD(std::string name, std::string id, bool autoprocess = true, bool autosave = true);
  /// Destructor
  ~AWEScintillatorSD() {};

  /// Main processing. Looks for highest momentum track.
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Initialize the Hit Collection
  void Initialize(G4HCofThisEvent*HCE);


  /// Getter functions
  inline ScintillatorHitsCollection* GetHC() { return fHitsCollection; };
  inline G4int GetHCID() { return fHCID; };


protected:

  ScintillatorHitsCollection* fHitsCollection;
  G4int fHCID;

};

/// AWEScintillator Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class AWEScintillatorProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  AWEScintillatorProcessor(AWEScintillatorSD* trkr, bool autosave = true);
  /// Destructor
  ~AWEScintillatorProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Reset all information
  void Reset();

  // Getter Functions
  inline G4double       GetTime() { return fTime; };
  inline G4double       GetPosX() { return fPosX;  };
  inline G4double       GetPosY() { return fPosY;  };
  inline G4double       GetPosZ() { return fPosZ;  };
  inline G4double       GetVolPosX() { return fVolPosX;  };
  inline G4double       GetVolPosY() { return fVolPosY;  };
  inline G4double       GetVolPosZ() { return fVolPosZ;  };
  inline G4double       GetEDep() { return fEdep;  };
  inline G4double       GetThetaXZ() { return fThetaXZ;  };
  inline G4double       GetThetaYZ() { return fThetaYZ;  };


protected:

  AWEScintillatorSD* fDetector; ///< Pointer to associated to SD
  G4int fHCID;

  bool fSave; ///< Flag to save event info automatically

  G4double fTime;
  G4double fEdep;
  G4double fPosX;
  G4double fPosY;
  G4double fPosZ;
  G4double fVolPosX;
  G4double fVolPosY;
  G4double fVolPosZ;
  G4double fPDG;
  G4double fThetaXZ;
  G4double fThetaYZ;

  int fTimeIndex;
  int fPDGIndex;
  int fPosXIndex;
  int fPosYIndex;
  int fPosZIndex;
  int fVolPosXIndex;
  int fVolPosYIndex;
  int fVolPosZIndex;
  int fThXZIndex;
  int fThYZIndex;
  int fEdepIndex;

};


}
#endif
