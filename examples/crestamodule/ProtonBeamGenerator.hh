//******************************************************************************
// PrimaryGeneratorAction.hh
//
// This class is a class derived from G4VUserPrimaryGeneratorAction for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef __CRESTA_ProtonBeamGenerator_HH__
#define __CRESTA_ProtonBeamGenerator_HH__

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

//---------------------------------------------------------------------------------
class ProtonBeamGenerator : public G4VUserPrimaryGeneratorAction
{
public:

  /// Constructor
  ProtonBeamGenerator();
  /// Destructor
  ~ProtonBeamGenerator();

  /// Generate the primary muon, and apply acceptance filter.
  void GeneratePrimaries(G4Event* anEvent);

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

  G4ParticleGun* fParticleGun; ///< Main particle gun

  // Throws are tracked regardless of acceptance, so integrated time always correct.
  /// Current Integrated Exposure time. Derivide from NThrows and Integrated flux.
  G4double fExposureTime;
  int fNThrows; ///< Number of throws ran so far.

  G4double fProtonRate;       ///< Time spacing between protons
  G4double fParticleTime;     ///< ParticleTime   Info for auto flux processor
  G4double fParticleEnergy;   ///< ParticleEnergy Info for auto flux processor
  G4ThreeVector fParticleDir; ///< ParticleDir    Info for auto flux processor
  G4ThreeVector fParticlePos; ///< ParticlePos    Info for auto flux processor
  G4double fParticlePDG;      ///< ParticlePDG    Info for auto flux processor
};
//---------------------------------------------------------------------------------




//---------------------------------------------------------------------------------
/// ShuklaFluxProcessor class : Automatically saves the true muon information
/// for each event into the TTree
class ProtonBeamProcessor : public VFluxProcessor {
public:
  /// Processor can only be created with an associated
  /// shukla generator object.
  ProtonBeamProcessor(ProtonBeamGenerator* gen, bool autosave = true);
  /// Destructor
  ~ProtonBeamProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Save the information from the generator for this event
  bool ProcessEvent(const G4Event* event);

  /// Return an integrated exposure time in s. Used for
  /// ending the run after so many seconds.
  G4double GetExposureTime();
  void ResetExposureTime() { fGenerator->SetExposureTime(0.0);};

protected:

  ProtonBeamGenerator* fGenerator; ///< Pointer to associated generator
};
//---------------------------------------------------------------------------------

} // - namespace CRESTA
#endif
