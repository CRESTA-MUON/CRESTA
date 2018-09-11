#ifndef __CRESTA_ActionFactory_HH__
#define __CRESTA_ActionFactory_HH__

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4UserStackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4VUserActionInitialization.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "action/CRESTAActionInitialization.hh"
#include "action/CRESTARun.hh"
#include "action/CRESTARunAction.hh"
#include "action/CRESTAStackingAction.hh"
#include "action/CRESTASteppingAction.hh"

namespace CRESTA {

/// Globally accessible Factory for producing actions.
namespace ActionFactory {

/// Function to load global configured actions
G4VUserActionInitialization* ConstructActionInitialization();

/// Function to load an action initialization from table
G4VUserActionInitialization* ConstructActionInitialization(DBTable table);

/// Function to load a run definition from table
G4Run* ConstructRun(DBTable table);

/// Function to load a run action from table
G4UserRunAction* ConstructRunAction(DBTable table);

/// Function to load a stacking action from table
G4UserStackingAction* ConstructStackingAction(DBTable table);

/// Function to load a stepping action from table
G4UserSteppingAction* ConstructSteppingAction(DBTable table);

} // - namespace ActionFactory
} // - namespace CRESTA
#endif
