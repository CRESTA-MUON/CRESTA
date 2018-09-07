// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
#ifndef __COSMIC_VFluxProcessor_hh__
#define __COSMIC_VFluxProcessor_hh__

// G4 Headers
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

// namespace COSMIC
namespace COSMIC {

/// Virtual Processor Template Class.
/// Used as a template for processors that convert
/// information from run information, e.g. detector hits,
/// into useful information and save it to the outputs.
class VFluxProcessor {
public:
  
  /// Empty Constructor
  VFluxProcessor(){};
  /// Named Constructor
  VFluxProcessor(std::string id) : fID(id) {};
  /// Destructor
  ~VFluxProcessor(){};

  /// Virtual start of run action. All processors
  /// should implement this, as it is used to setup
  /// Ntuple arrays.
  virtual bool BeginOfRunAction(const G4Run* run) = 0;

  /// Virtual Event Processing action. All processors
  /// should implement this, as it is used to extract
  /// information from the event.
  virtual bool ProcessEvent(const G4Event* event) = 0;

  /// Virtual reset function, used to reset processor
  /// information at the start of an event.
  virtual inline void Reset() {
    fHasInfo = false;
    fTime      = -999.9;
    fEnergy    = -999.9;
  }

  /// Set Processor ID
  virtual inline void SetID(std::string id){ fID = id; };

  /// Return Processor ID
  virtual inline std::string GetID(){ return fID; };

  /// Return whether the processor has any valid info.
  virtual inline bool HasInfo() { return fHasInfo; };

  /// Return a processed time if available. Used for
  /// simple co-incident triggers.
  virtual inline G4double GetTime() { return fTime; };

  /// Return a processed energy if available. Used for
  /// simple co-incident triggers.
  virtual inline G4double GetEnergy() { return fEnergy; };

  /// Return an integrated exposure time in s. Used for
  /// ending the run after so many seconds.
  virtual G4double GetExposureTime() = 0;

  /// Set exposure time back to 0
  virtual void ResetExposureTime() = 0;

  /// Get event rate
  virtual double GetEventRate(){return 1.0;};

protected:

  bool fHasInfo; ///< Whether the event had anything useful in it.
  G4double fTime; ///< Average hit time  that can be set by the user.
  G4double fEnergy; ///< Average energy deposited that can be set by the user.
  std::string fID; ///< ID of this processor

};

} // - namespace COSMIC
#endif
