//******************************************************************************
// PrimaryGeneratorAction.hh
//
// This class is a class derived from G4VUserPrimaryGeneratorAction for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef __CRESTA_PrimaryGeneratorGeneric_HH__
#define __CRESTA_PrimaryGeneratorGeneric_HH__

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
/// Shukla Flux Generator, derived from CRESTA code written by Chris Steer.
class PrimaryGeneratorGeneric : public G4VUserPrimaryGeneratorAction
{
public:

    /// Constructor
    PrimaryGeneratorGeneric();

    /// Destructor
    ~PrimaryGeneratorGeneric();

    /// Generate the primary muon, and apply acceptance filter.
    void GeneratePrimaries(G4Event* anEvent);

    /// Make sure the source box is setup.
    void GetSourceBox();

    /// Get the Target Box setup
    void GetTargetBoxes();

    /// Sample the scattering rate
    void SampleKinematics(G4ThreeVector& pos, G4ThreeVector& dir, G4double& energy);

    // Check for intercepts
    bool CheckTargetBoxes(G4ThreeVector& pos, G4ThreeVector& dir);

    // I/O Functions
    inline G4double GetTime()     { return fTime;     };
    inline G4double GetEnergy()   { return fEnergy;   };
    inline G4ThreeVector GetDir() { return fDir;      };
    inline G4ThreeVector GetPos() { return fPos;      };
    inline int GetPDG()           { return fPDG;      };
    inline G4double GetExposureTime() { return fTime; };
    inline void SetExposureTime(G4double e) { fTime = e; };

private:

    G4double fMinEnergy; ///< Min Energy Range to integrate/throw
    G4double fMaxEnergy; ///< Max Energy Range to integrate/throw

    G4double fMinTheta; ///< Min Theta Range to integrate/throw
    G4double fMaxTheta; ///< Max Theta Range to integrate/throw

    G4double fMinPhi; ///< Min Phi Range to integrate/throw
    G4double fMaxPhi; ///< Max Phi Range to integrate/throw

    G4double fRate; ///< Rate of generation in Hz / m-2
    G4int    fNThrows; ///< Counter for acceptance tracking

    /// Definitions for particle gun to avoid string comparisons
    G4ParticleGun* fParticleGun; ///< Main particle gun

    // Source box setup originally used geometries from the main GEO
    // volume list, but no longer does. That is why it is setup this
    // way.
    bool fSourceBox; ///< FLAG : Whether source box been created/checked
    G4ThreeVector fSourceBoxWidth; ///< Length/Width of box.
    G4ThreeVector fSourceBoxPosition; ///< Position of box in world volume
    G4double fArea; ///< Area of box. Used for normalisation.

    // Target box also originally setup from GEO tables.
    bool fCheckTargetBoxes; ///< FLAG : Whether target boxes okay.
    std::vector<G4Box*> fTargetBoxes; ///< Geant4 Box Object for each target
    std::vector<G4ThreeVector> fTargetBoxPositions; ///< Position in world volume for each target

    G4double fTime;     ///< Time   Info for auto flux processor
    G4double fEnergy;   ///< Energy Info for auto flux processor
    G4ThreeVector fDir; ///< Dir    Info for auto flux processor
    G4ThreeVector fPos; ///< Pos    Info for auto flux processor
    G4double fPDG;      ///< PDG    Info for auto flux processor

};
//---------------------------------------------------------------------------------




//---------------------------------------------------------------------------------
/// Generic Flux Processor class : Automatically saves the true muon information
/// for each event into the TTree
class GenericPrimaryFluxProcessor : public VFluxProcessor {
public:
    /// Processor can only be created with an associated
    /// shukla generator object.
    GenericPrimaryFluxProcessor(PrimaryGeneratorGeneric* gen, bool autosave = true);
    /// Destructor
    ~GenericPrimaryFluxProcessor() {};

    /// Setup Ntuple entries
    bool BeginOfRunAction(const G4Run* run);

    /// Save the information from the generator for this event
    bool ProcessEvent(const G4Event* event);

    /// Return an integrated exposure time in s. Used for
    /// ending the run after so many seconds.
    G4double GetExposureTime();

    void ResetExposureTime(){fGenerator->SetExposureTime(0.0);};


protected:

    PrimaryGeneratorGeneric* fGenerator; ///< Pointer to associated generator

    bool fSave; ///< Flag to save event info automatically

    int fTimeIndex; ///< Time Ntuple Index
    int fEnergyIndex; ///< Energy Ntuple Index
    int fDirXIndex; ///< DirX Ntuple Index
    int fDirYIndex; ///< DirY Ntuple Index
    int fDirZIndex; ///< DirZ Ntuple Index
    int fPosXIndex; ///< PosX Ntuple Index
    int fPosYIndex; ///< PosY Ntuple Index
    int fPosZIndex; ///< PosZ Ntuple Index
    int fPDGIndex;  ///< MPDG Ntuple Index

};
//---------------------------------------------------------------------------------

} // - namespace CRESTA
#endif
