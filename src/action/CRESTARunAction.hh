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
#ifndef __CRESTA_CRESTARunAction_hh__
#define __CRESTA_CRESTARunAction_hh__

// G4 Headers
#include "G4UserRunAction.hh"
#include "globals.hh"

// Forward Declarations
class G4Run;

// namespace CRESTA
namespace CRESTA {

/// User's CRESTARunAction class. this class implements all the user actions to be executed at each run.
class CRESTARunAction : public G4UserRunAction
{
public:

  /// constructor
  CRESTARunAction();
  /// destructor
  virtual ~CRESTARunAction();

  /// Create a new run
  virtual G4Run* GenerateRun();

  /// Called at the beginning of each run
  virtual void BeginOfRunAction(const G4Run*);
  /// Called at the end of each run
  virtual void   EndOfRunAction(const G4Run*);

protected:
  int fCurrentRun; ///< Index of current run
};

} // - namespace CRESTA
#endif

