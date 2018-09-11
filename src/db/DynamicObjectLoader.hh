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
	typedef int (*DSF_NDynamicObjects_ptr)(void);
	typedef char const* (*DSF_GetDynamicObjectName_ptr)(int);

	typedef VProcessor* (*DSF_ConstructProcessor_ptr)(DBTable*);
	typedef void (*DSF_DestroyProcessor_ptr)(VProcessor*);

	typedef VDetector* (*DSF_ConstructDetector_ptr)(DBTable*);
	typedef void (*DSF_DestroyDetector_ptr)(VDetector*);

	typedef GeoObject* (*DSF_ConstructGeometry_ptr)(DBTable*);
	typedef void (*DSF_DestroyGeometry_ptr)(GeoObject*);

	typedef G4VUserPrimaryGeneratorAction* (*DSF_LoadFluxGenerator_ptr)(std::string);
	typedef void (*DSF_DestroyFluxGenerator_ptr)(G4VUserPrimaryGeneratorAction*);


	// A plugin manifest for available dynamic pointers
	struct PluginManifest {
		void* dllib;
		std::string soloc;

		DSF_NDynamicObjects_ptr DSF_NDynamicObjects;
		DSF_GetDynamicObjectName_ptr DSF_GetDynamicObjectName;
		size_t NObjects;

		DSF_ConstructProcessor_ptr DSF_ConstructProcessor;
		DSF_DestroyProcessor_ptr DSF_DestroyProcessor;

		DSF_ConstructDetector_ptr DSF_ConstructDetector;
		DSF_DestroyDetector_ptr DSF_DestroyDetector;

		DSF_ConstructGeometry_ptr DSF_ConstructGeometry;
		DSF_DestroyGeometry_ptr DSF_DestroyGeometry;

		DSF_LoadFluxGenerator_ptr DSF_LoadFluxGenerator;
		DSF_DestroyFluxGenerator_ptr DSF_DestroyFluxGenerator;

		~PluginManifest();
	};

	std::vector<PluginManifest> Manifests;

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

