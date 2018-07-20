#ifndef __LineOfSightSD_HH__
#define __LineOfSightSD_HH__

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
#include "G4EmCalculator.hh"

#include "G4Run.hh"
#include "globals.hh"
#include "g4root.hh"
#include "analysis/VProcessor.hh"
#include "analysis/VDetector.hh"
#include "G4EmCalculator.hh"

namespace COSMIC {

// Forward Declarations
class DBTable;

/// True LineOfSight Tracker Detector Object :
class LineOfSightSD : public VDetector {

public:

  /// Main constructor from a database table
  LineOfSightSD(DBTable table);
  /// Simple C++ constructor
  LineOfSightSD(std::string name, std::string id, bool autoprocess=true, bool autosave=true);
  /// Destructor
  ~LineOfSightSD(){};

  /// Main processing
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  // Get the line-of-sight mass in g/cm2
  G4double GetLOSMass(){return fLOSMass;}
  void SetCutOffDepth(G4double cutoff){fZCutOff = cutoff;};
  void SetCutOffDepth();

  G4double GetEnergy(){return fLOSMass;};// Use the LOSmass as a surrogate for energy
  G4Material* GetMaterial(){return fMaterial;};

  G4double GetRange(){return fRange;};
  G4double GetDEDX(){return fdedx;};

protected:

  G4double fLOSMass;
  bool fCheckedCutOff;
  G4double fZCutOff;
  G4Material* fMaterial;
  G4double fEnergy;

  double fEventRate;// Sum for calculating the rate through this detector
  double fdedx;// The dEdx for this particle and detector
  double fRange;// The range of the particle in this detector region

  G4EmCalculator* fCalc;

};

///  LineOfSight Processor Object :
/// By default this is created alongside the true LineOfSight
/// tracker object, so the info is automatically added
/// to the TTree.
class LineOfSightProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  LineOfSightProcessor(LineOfSightSD* trkr, bool autosave=true);
  /// Destructor
  ~LineOfSightProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  double GetEnergy(){return fEnergy;};

  /// Draw function
  void DrawEvent();

  G4double GetExposureTime(){ return 0.0;};

protected:

  LineOfSightSD* fTracker; ///< Pointer to associated tracker SD

  bool fSave; ///< Flag to save event info automatically
  int fLineOfSightMassIndex; ///< Time Ntuple Index
  int fDEDxIndex; ///< Ntuple Index
  int fRangeIndex; ///< Ntuple Index
  double fEnergy;

};


}
#endif
