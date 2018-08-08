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
#ifndef __COSMIC_VDetector_hh_
#define __COSMIC_VDetector_hh_

#include "G4VSensitiveDetector.hh"
#include "G4Event.hh"

namespace COSMIC {

/// Template detector class for use in the analysis.
/// Uses most of the sensitive detector functionality,
/// but has a ResetState function to restore all variables
/// at the start of an event.
class VDetector : public G4VSensitiveDetector
{
public:

  /// Set ID on construction
  inline VDetector(std::string id, std::string type) :
    G4VSensitiveDetector(id) {
    fID = id;
    fType = type;
  };
  /// Destructor
  virtual ~VDetector() {};


  /// Reset detector state at start of event
  virtual void ResetState() {};


  /// Set this detectors ID tag
  inline void SetID(std::string id) {fID = id;};
  /// Get this detectors ID tag
  inline std::string GetID() {return fID;};

  /// Set this detectors type string for reference
  inline void SetType(std::string type) {fType = type;};
  /// Get this detectors type string for reference
  inline std::string GetType() {return fType;};


  /// Assign the detector to a logical volume
  virtual inline void SetLogicalVolume(G4LogicalVolume* logic, G4VPhysicalVolume* /*physical*/) {
    logic->SetSensitiveDetector(this);
  }

protected:
  std::string fType; ///< Detector type
  std::string fID;   ///< Detector Unique ID tag
};
}

#endif

