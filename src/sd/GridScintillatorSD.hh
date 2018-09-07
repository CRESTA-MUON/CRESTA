#ifndef __GridScintillatorSD_HH__
#define __GridScintillatorSD_HH__

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
#include "ScintillatorHit.hh"
#include "Randomize.hh"

#include "db/DBTable.hh"

namespace COSMIC {

/// True Muon Tracker Detector Object :
/// Saves the highest momentrum muon track
/// that entered a detector logical voluem
class GridScintillatorSD : public VDetector {
public:

  /// Main constructor from a database table
  GridScintillatorSD(DBTable table);
  /// Grid C++ constructor
  GridScintillatorSD(std::string name, std::string id, bool autoprocess = true, bool autosave = true);
  /// Destructor
  ~GridScintillatorSD() {};

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
  G4double fEfficiency;

};

/// GridScintillator Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class GridScintillatorProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  GridScintillatorProcessor(GridScintillatorSD* trkr, bool autosave = true);
  /// Destructor
  ~GridScintillatorProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Reset all information
  void Reset();

protected:

  GridScintillatorSD* fDetector; ///< Pointer to associated to SD
  G4int fHCID;

  bool fSave; ///< Flag to save event info automatically

  std::vector<float> fVoxelE;
  std::vector<float> fVoxelX;
  std::vector<float> fVoxelY;
  std::vector<float> fVoxelZ;
  std::vector<float> fVoxelT;
  std::vector<int> fVoxelP;
};


}
#endif
