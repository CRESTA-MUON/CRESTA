#include "ActionFactory.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4UserStackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4VUserActionInitialization.hh"

#include "action/CRESTAActionInitialization.hh"
#include "action/CRESTARun.hh"
#include "action/CRESTARunAction.hh"
#include "action/CRESTAStackingAction.hh"
#include "action/CRESTASteppingAction.hh"

#include "action/OpticalActionInitialization.hh"
#include "action/OpticalSteppingAction.hh"

using namespace CRESTA;

G4VUserActionInitialization* ActionFactory::ConstructActionInitialization() {
	// Get global definition table
	DBTable table = DB::Get()->GetTable("GLOBAL", "config");
	return ConstructActionInitialization(table);
}

G4VUserActionInitialization* ActionFactory::ConstructActionInitialization(DBTable table) {
	// Get the action type from the config table
	std::string type = table.GetS("action_initialization");
	if (type.compare("cresta") == 0) return new CRESTAActionInitialization();
	else if (type.compare("optical") == 0) return new OpticalActionInitialization();

	// Check we didn't get to here and fail string comparison
	std::cout << "Failed to create action initialization : " << type << std::endl;
	throw;
	return 0;
}

G4Run* ActionFactory::ConstructRun(DBTable table) {
	// Get the action type from the config table
	std::string type = table.GetS("run");
	if (type.compare("cresta") == 0) return new CRESTARun();

	// Check we didn't get to here and fail string comparison
	std::cout << "Failed to create action initialization : " << type << std::endl;
	throw;
	return 0;
}

G4UserRunAction* ActionFactory::ConstructRunAction(DBTable table) {
	// Get the action type from the config table
	std::string type = table.GetS("run_action");
	if (type.compare("cresta") == 0) return new CRESTARunAction();

	// Check we didn't get to here and fail string comparison
	std::cout << "Failed to create action initialization : " << type << std::endl;
	throw;
	return 0;
}

G4UserStackingAction* ActionFactory::ConstructStackingAction(DBTable table) {
	// Get the action type from the config table
	std::string type = table.GetS("stacking_action");
	if (type.compare("cresta") == 0) return new CRESTAStackingAction();

	// Check we didn't get to here and fail string comparison
	std::cout << "Failed to create action initialization : " << type << std::endl;
	throw;
	return 0;
}

G4UserSteppingAction* ActionFactory::ConstructSteppingAction(DBTable table) {
	// Get the action type from the config table
	std::string type = table.GetS("stepping_action");
	if (type.compare("cresta") == 0) return new CRESTASteppingAction();
	else if (type.compare("optical") == 0) return new OpticalSteppingAction();

	// Check we didn't get to here and fail string comparison
	std::cout << "Failed to create action initialization : " << type << std::endl;
	throw;
	return 0;
}
