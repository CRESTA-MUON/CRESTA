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
#ifndef __CosmicRun_hh__
#define __CosmicRun_hh__ 

#include "G4Run.hh"

// Forward Declarations
class G4Event;
class G4Run;

// namespace COSMIC
namespace COSMIC {

/// Main CosmicRun class used for event recording and generation
class CosmicRun : public G4Run
{
  public:
    /// Constructor
    CosmicRun();
    /// Destructor
    virtual ~CosmicRun();
    
    /// Process to register the event
    virtual void RecordEvent(const G4Event*);

    /// Merges run outputs between slave jobs
    virtual void Merge(const G4Run*);

  protected:
    int fPrintSize; ///< Counter to check processing size is sufficient
    long int fStartTime; ///< Start Time of this Run
    long int fCurTime; ///< End Time of this Run
};

} // - namespace COSMIC
#endif

    
