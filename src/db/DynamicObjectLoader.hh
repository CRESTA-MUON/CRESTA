#ifndef __DYNAMIC_PROCESSOR_FACTORY_HH__
#define __DYNAMIC_PROCESSOR_FACTORY_HH__

#include <iostream>
#include "analysis/VProcessor.hh"
#include "geo/GeoObject.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

namespace CRESTA {

// Forward Declarations
class DBTable;
class VDetector;

/// Detector Factory used to create SD from tables
class DynamicObjectLoader {
public:

	// Main Constructors/Destructors for Singleton
	DynamicObjectLoader();
	~DynamicObjectLoader();
	static DynamicObjectLoader* glblDSF;
	static DynamicObjectLoader* Get();

	// Available functions that can be found in the working area
	typedef std::string (*CRESTA_PrintManifest_ptr)(void);
	typedef VProcessor* (*CRESTA_ConstructProcessor_ptr)(DBTable*);
	typedef VDetector* (*CRESTA_ConstructDetector_ptr)(DBTable*);
	typedef GeoObject* (*CRESTA_ConstructGeometry_ptr)(DBTable*);
	typedef G4VUserPrimaryGeneratorAction* (*CRESTA_LoadFluxGenerator_ptr)(std::string);

	// A plugin manifest for available dynamic pointers
	struct PluginManifest {
		void* dllib;
		std::string soloc;

		CRESTA_PrintManifest_ptr CRESTA_PrintManifest;
		CRESTA_ConstructProcessor_ptr CRESTA_ConstructProcessor;
		CRESTA_ConstructDetector_ptr CRESTA_ConstructDetector;
		CRESTA_ConstructGeometry_ptr CRESTA_ConstructGeometry;
		CRESTA_LoadFluxGenerator_ptr CRESTA_LoadFluxGenerator;

		~PluginManifest();
	};

	std::vector<PluginManifest> Manifests;
	std::vector<std::string> fLoadedManifests;
	size_t NObjects;
	size_t NManifests;

	void LoadPlugins();

	VProcessor* ConstructDynamicProcessor(DBTable tbl);
	VDetector* ConstructDynamicDetector(DBTable tbl);

	G4VUserPrimaryGeneratorAction* LoadDynamicFluxGenerator(std::string type);

	GeoObject* ConstructDynamicGeometry(DBTable tbl);

}; // - class DYNAMIC PROCESSOR FACTORY
}; // - namespace CRESTA
#endif

