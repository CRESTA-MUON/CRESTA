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
#ifndef __CRESTA_VTrigger_hh__
#define __CRESTA_VTrigger_hh__

// System Headers
#include <iostream>

// G4 Headers
#include "G4Event.hh"
#include "G4Run.hh"

// namspace CRESTA
namespace CRESTA {

/// Virtual Trigger template class
/// Classes that inherit from this should
/// access the available processors in the
/// main analysis manager, or access event
/// information directly to determine if an
/// event is signal or not.
class VTrigger {
public:

  /// Emptry Constructor
  VTrigger() {};
  /// Named Constructor
  VTrigger(std::string id) : fID(id) {};
  /// Destructor
  virtual ~VTrigger() {};


  /// User should overwrite this function to return
  /// true when an event is triggered. The processor
  /// stage occurs before triggers are called, so
  /// this function can also use information from any
  /// loaded processors.
  virtual bool ProcessTrigger(const G4Event* event) = 0;
  /// Reset function to reset trigger state at start of event
  virtual inline void Reset() { fTriggered = false; };


  /// Set Trigger ID
  virtual inline void SetID(std::string id) { fID = id; };
  /// Return Trigger ID
  virtual inline std::string GetID() { return fID; };


  /// Return trigger, incase it is needed later
  virtual inline bool GetTrigger() { return fTriggered; };

protected:

  bool fTriggered; ///< Current state of this trigger
  std::string fID; ///< ID of this trigger

};

} // - namespace CRESTA
#endif
