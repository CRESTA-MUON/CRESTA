//******************************************************************************
// PrimaryGeneratorAction.hh
//
// This class is a class derived from G4VUserPrimaryGeneratorAction for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef __COSMIC_LOSPrimaryGenerator_HH__
#define __COSMIC_LOSPrimaryGenerator_HH__

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

namespace COSMIC {

// Self Contained Mutex for locking threads and avoiding non threadsafe issues.
namespace { G4Mutex myMutexLOS = G4MUTEX_INITIALIZER; }

//---------------------------------------------------------------------------------
/// LOS Flux Generator, derived from CRESTA code written by Chris Steer.
class LOSPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:

    /// Constructor
    LOSPrimaryGenerator();
    /// Destructor
    ~LOSPrimaryGenerator();


    /// Generate the primary muon, and apply acceptance filter.
    void GeneratePrimaries(G4Event* anEvent);

    /// Make sure the source box is setup.
    void GetSourceBox();
    /// Get the Target Box setup
    std::vector<G4Box*> GetTargetBoxes();
    /// Get the Target Box Position Setup
    std::vector<G4ThreeVector> GetTargetBoxPositions();

    // Get Functions
    inline G4ThreeVector GetMuonDir() { return fMuonDir;      };
    inline G4ThreeVector GetMuonPos() { return fMuonPos;      };

    double GetMuonAnglePDF(){return fMuonAnglePDF;};
    double GetMuonEnergyPDF(){return fMuonEnergyPDF;};
    double GetMuonEnergy(){return fMuonEnergy;};

    double GetVerticalRate(){return fPar_I0;};
    // Half widths so multiply by 4
    double GetIntegratedRate(){return fFluxIntegrated*4.0*(fSourceBoxWidth[0]/m)*(fSourceBoxWidth[1]/m);};

private:

    //
    G4ThreeVector fEventSourcePosition;

    /// Definitions for particle gun to avoid string comparisons
    std::vector<G4ParticleDefinition*> fParticleDefs;
    G4ParticleGun* fParticleGun; ///< Main particle gun

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

    G4ThreeVector fMuonDir; ///< MuonDir    Info for auto flux processor
    G4ThreeVector fMuonPos; ///< MuonPos    Info for auto flux processor

    double fMinCosTheta,fMaxCosTheta;

    double fMuonAnglePDF;
    double fMuonEnergyPDF;

    G4double fMinEnergy; ///< Min Energy Range to integrate/throw
    G4double fMaxEnergy; ///< Max Energy Range to integrate/throw

    TF1 *fEnergyPDF; ///< Energy Function from Shukla Paper
    TF1 *fZenithPDF; ///< Zenith Function from Shukla Paper

    G4double fFluxIntegrated; ///< Integral of flux for norm.

    G4double fPar_I0;  ///< IO  Par from Shukla Paper
    G4double fPar_n;   ///< n   Par from Shukla Paper
    G4double fPar_E0;  ///< E0  Par from Shukla Paper
    G4double fPar_eps; ///< eps Par from Shukla Paper

    G4double fPar_rad; ///< radius Par from Shukla Paper
    G4double fPar_dis; ///< distance Par from Shukla Paper


    G4double fMuonTime;     ///< MuonTime   Info for auto flux processor
    G4double fMuonEnergy;   ///< MuonEnergy Info for auto flux processor
    G4double fMuonPDG;      ///< MuonPDG    Info for auto flux processor
    G4double fSpeedUp;      ///< MuonPDG    Info for auto flux processor





};
//---------------------------------------------------------------------------------




//---------------------------------------------------------------------------------
/// LOSFluxProcessor class : Automatically saves the true muon information
/// for each event into the TTree
class LOSPrimaryFluxProcessor : public VFluxProcessor {
public:
    /// Processor can only be created with an associated
    /// LOS generator object.
    LOSPrimaryFluxProcessor(LOSPrimaryGenerator* gen, bool autosave = true);
    /// Destructor
    ~LOSPrimaryFluxProcessor() {};

    /// Setup Ntuple entries
    bool BeginOfRunAction(const G4Run* run);
    /// Save the information from the generator for this event
    bool ProcessEvent(const G4Event* event);

    /// Return an integrated exposure time in s. Used for
    /// ending the run after so many seconds.
    G4double GetExposureTime(){return 0.0;};

    void ResetExposureTime(){};
    

protected:

    LOSPrimaryGenerator* fGenerator; ///< Pointer to associated generator

    bool fSave; ///< Flag to save event info automatically

    int fMuonTimeIndex; ///< Time Ntuple Index
    int fMuonEnergyIndex; ///< Energy Ntuple Index
    int fMuonDirXIndex; ///< DirX Ntuple Index
    int fMuonDirYIndex; ///< DirY Ntuple Index
    int fMuonDirZIndex; ///< DirZ Ntuple Index
    int fMuonPosXIndex; ///< PosX Ntuple Index
    int fMuonPosYIndex; ///< PosY Ntuple Index
    int fMuonPosZIndex; ///< PosZ Ntuple Index
    int fMuonPDGIndex;  ///< MPDG Ntuple Index

    int fMuonThXZIndex;
    int fMuonThYZIndex;

    int fMuonIDIndex;

    int fMuonAnglePDFIndex;
    int fMuonEnergyPDFIndex;
    int fMuonRatePDFIndex;

};
//---------------------------------------------------------------------------------

} // - namespace COSMIC
#endif
