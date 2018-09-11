//******************************************************************************
// PrimaryGeneratorAction.hh
//
// This class is a class derived from G4VUserPrimaryGeneratorAction for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef __CRESTA_IsotropicSphereGenerator_HH__
#define __CRESTA_IsotropicSphereGenerator_HH__

#include <vector>
#include <cmath>
#include <iomanip>

#include "TF1.h"
#include "TMath.h"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4Types.hh"

#include "db/DB.hh"
#include "analysis/Analysis.hh"
#include "analysis/VFluxProcessor.hh"
#include "geo/simple/GeoBox.hh"

namespace CRESTA {

// Self Contained Mutex for locking threads and avoiding non threadsafe issues.
//namespace { G4Mutex myMutex = G4MUTEX_INITIALIZER; }

//---------------------------------------------------------------------------------
/// Shukla Flux Generator, derived from CRESTA code written by Chris Steer.
class IsotropicSphereGenerator : public G4VUserPrimaryGeneratorAction
{
public:

  /// Constructor
  IsotropicSphereGenerator();
  /// Destructor
  ~IsotropicSphereGenerator();

  /// Pick a muon/antimuon based on random number
  void SampleParticleType();

  /// Randomly sample direction according to Zenith PDF.

  /// Randomly sample position on sphere
void SamplePositionAndDirection(G4ThreeVector& pos, G4ThreeVector& dir);

  /// Generate the primary muon, and apply acceptance filter.
  void GeneratePrimaries(G4Event* anEvent);

  /// Make sure the source box is setup.
  void GetSourceBox();
  /// Get the Target Box setup
  std::vector<G4Box*> GetTargetBoxes();
  /// Get the Target Box Position Setup
  std::vector<G4ThreeVector> GetTargetBoxPositions();

  // Get Functions
  inline G4double GetParticleTime()     { return fParticleTime;     };
  inline G4double GetParticleEnergy()   { return fParticleEnergy;   };
  inline G4ThreeVector GetParticleDir() { return fParticleDir;      };
  inline G4ThreeVector GetParticlePos() { return fParticlePos;      };
  inline int GetParticlePDG()           { return fParticlePDG;      };
  inline G4double GetExposureTime() { return fExposureTime; };
  inline void SetExposureTime(G4double e) { fExposureTime = e; };
  void Draw();

private:

  G4double fMinEnergy; ///< Min Energy Range to integrate/throw
  G4double fMaxEnergy; ///< Max Energy Range to integrate/throw

  TF1 *fSphereZenithPDF;   ///< Sphere Zenith PDF for surface
  TF1 *fSphereAzimuthalPDF; ///< Sphere Azimuthal PDF for surface
  TF1 *fParticleEnergyPDF; ///< Energy function for particle ejection
  TF1 *fParticleZenithPDF; ///< Zenith function for particle ejection
  TF1 *fParticleAzimuthalPDF; ///< Azimuthal function for particle ejection

  /// Definitions for particle gun to avoid string comparisons
  std::vector<double> fParticleProb; ///< User probabilities
  std::vector<G4ParticleDefinition*> fParticleDefs;
  G4ParticleGun* fParticleGun; ///< Main particle gun
  std::vector<int> fParticlePDGs;
  // Source box setup originally used geometries from the main GEO
  // volume list, but no longer does. That is why it is setup this
  // way.
  bool fSourceBox; ///< FLAG : Whether source box been created/checked
  G4ThreeVector fSourceBoxWidth; ///< Length/Width of box.
  G4ThreeVector fSourceBoxPosition; ///< Position of box in world volume
  G4int fSourceBoxRequireN; ///< Requires at least this number of hits in different target boxes

  G4double fArea; ///< Area of box. Used for normalisation.

  // Target box also originally setup from GEO tables.
  bool fCheckTargetBoxes; ///< FLAG : Whether target boxes okay.
  std::vector<G4Box*> fTargetBoxes; ///< Geant4 Box Object for each target
  std::vector<G4ThreeVector> fTargetBoxPositions; ///< Position in world volume for each target
  G4int fTargetBoxesRequireN; ///< Requires at least this number of hits in different target boxes

  // Throws are tracked regardless of acceptance, so integrated time always correct.
  /// Current Integrated Exposure time. Derivide from NThrows and Integrated flux.
  G4double fExposureTime;
  int fNThrows; ///< Number of throws ran so far.

  G4double fParticleTime;     ///< ParticleTime   Info for auto flux processor
  G4double fParticleEnergy;   ///< ParticleEnergy Info for auto flux processor
  G4ThreeVector fParticleDir; ///< ParticleDir    Info for auto flux processor
  G4ThreeVector fParticlePos; ///< ParticlePos    Info for auto flux processor
  G4double fParticlePDG;      ///< ParticlePDG    Info for auto flux processor
  G4double fSpeedUp;
  G4double fSphereRadius;
};
//---------------------------------------------------------------------------------




//---------------------------------------------------------------------------------
/// ShuklaFluxProcessor class : Automatically saves the true muon information
/// for each event into the TTree
class IsotropicSphereFluxProcessor : public VFluxProcessor {
public:
  /// Processor can only be created with an associated
  /// shukla generator object.
  IsotropicSphereFluxProcessor(IsotropicSphereGenerator* gen, bool autosave = true);
  /// Destructor
  ~IsotropicSphereFluxProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Save the information from the generator for this event
  bool ProcessEvent(const G4Event* event);

  /// Return an integrated exposure time in s. Used for
  /// ending the run after so many seconds.
  G4double GetExposureTime();
  void ResetExposureTime() { fGenerator->SetExposureTime(0.0);};

protected:

  IsotropicSphereGenerator* fGenerator; ///< Pointer to associated generator

  bool fSave; ///< Flag to save event info automatically

  int fParticleTimeIndex; ///< Time Ntuple Index
  int fParticleEnergyIndex; ///< Energy Ntuple Index
  int fParticleDirXIndex; ///< DirX Ntuple Index
  int fParticleDirYIndex; ///< DirY Ntuple Index
  int fParticleDirZIndex; ///< DirZ Ntuple Index
  int fParticlePosXIndex; ///< PosX Ntuple Index
  int fParticlePosYIndex; ///< PosY Ntuple Index
  int fParticlePosZIndex; ///< PosZ Ntuple Index
  int fParticlePDGIndex;  ///< MPDG Ntuple Index

};
//---------------------------------------------------------------------------------

} // - namespace CRESTA
#endif
