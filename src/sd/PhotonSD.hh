#ifndef __PhotonSD_HH__
#define __PhotonSD_HH__

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
#include "PhotonHit.hh"

namespace COSMIC {

// Forward Declarations
class DBTable;

/// Photon SD, Manually saves and kills all
/// photons entering a sensitive detector.
class PhotonSD : public VDetector {
public:

  /// Main constructor from a database table
  PhotonSD(DBTable table);
  /// Destructor
  ~PhotonSD() {};

  /// Main processing. Looks for highest momentum track.
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  /// Save the events
  G4TrackStatus ManuallyProcessHits(const G4Step* aStep, G4TouchableHistory* touch);

  /// Initialize the Hit Collection
  void Initialize(G4HCofThisEvent*HCE);

  /// Getter functions
  inline PhotonHitsCollection* GetHC() { return fHitsCollection; };
  inline G4int GetHCID() { return fHCID; };

protected:

  PhotonHitsCollection* fHitsCollection;
  G4int fHCID;

};

/// Photon Processor Object
/// Saves 2D vector of photon energies and times
class PhotonProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  PhotonProcessor(PhotonSD* trkr, bool autosave = true);
  /// Destructor
  ~PhotonProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Draw function
  void DrawEvent();

protected:
  std::vector<double> fPhotonE;
  std::vector<double> fPhotonT;
  PhotonSD* fDetector; ///< Pointer to associated tracker SD
  int fHCID;
};


}
#endif

