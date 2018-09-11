#include "DynamicObjectLoader.hh"
#include "TRegexp.h"
#include <dirent.h>
#include <dlfcn.h>
#include "db/DBTable.hh"
#include "db/DB.hh"

using namespace COSMIC;

DynamicObjectLoader::DynamicObjectLoader() : NObjects(0), NManifests(0) {
	LoadPlugins();
	std::cout << "Loaded : " << NObjects
	          << " from " << NManifests
	          << " shared object libraries." << std::endl;
}
DynamicObjectLoader* DynamicObjectLoader::glblDSF = NULL;

DynamicObjectLoader::PluginManifest::~PluginManifest() {
}

std::string EnsureTrailingSlash(std::string const& inp) {
	if (!inp.length()) {
		return "/";
	}
	if (inp[inp.length() - 1] == '/') {
		return inp;
	}
	return inp + "/";
}


void DynamicObjectLoader::LoadPlugins() {

	std::vector<std::string> SearchDirectories = DB::Get()->GetDynamicLibraryPath();

	for (size_t sp_it = 0; sp_it < SearchDirectories.size(); ++sp_it) {
		std::string dirpath = EnsureTrailingSlash(SearchDirectories[sp_it]);

		std::cout << "Searching for dynamic object manifests in: " << dirpath << std::endl;

		Ssiz_t len = 0;
		DIR* dir;
		struct dirent* ent;
		dir = opendir(dirpath.c_str());
		if (dir == NULL) { std::cout << "Tried to open non-existant directory." << std::endl; continue; }

		TRegexp matchExp("*.cresta", true);
		while ((ent = readdir(dir)) != NULL) {
			if (matchExp.Index(TString(ent->d_name), &len) != Ssiz_t(-1)) {

				std::cout << "Loading : " << (dirpath + ent->d_name) << std::endl;

				void* dlobj =
				    dlopen((dirpath + ent->d_name).c_str(), RTLD_NOW | RTLD_GLOBAL);
				char const* dlerr_cstr = dlerror();
				std::string dlerr;
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}

				if (dlerr.length()) {
					// ERROR(WRN, "\tDL Load Error: " << dlerr);
					continue;
				}

				// Create a new generic plugin manifest
				PluginManifest plugin;
				plugin.dllib = dlobj;
				plugin.soloc = (dirpath + ent->d_name);

				std::cout << "Searching DSF_NDynamicObjects" << std::endl;
				plugin.DSF_NDynamicObjects =
				    reinterpret_cast<DSF_NDynamicObjects_ptr>(dlsym(dlobj, "DSF_NDynamicObjects"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					dlclose(dlobj);
					continue;
				}

				plugin.DSF_GetDynamicObjectName =
				    reinterpret_cast<DSF_GetDynamicObjectName_ptr>(dlsym(dlobj, "DSF_GetDynamicObjectName"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					dlclose(dlobj);
					continue;
				}

				// Processor Function Loading
				bool validprocessor = true;
				if (validprocessor) {
					plugin.DSF_ConstructProcessor =
					    reinterpret_cast<DSF_ConstructProcessor_ptr>(dlsym(dlobj, "DSF_ConstructProcessor"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validprocessor = false; }
				}
				if (validprocessor) {
					plugin.DSF_DestroyProcessor =
					    reinterpret_cast<DSF_DestroyProcessor_ptr>(dlsym(dlobj, "DSF_DestroyProcessor"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validprocessor = false; }
				}

				// Detector Function Loading
				bool validdetector = true;
				if (validdetector) {
					plugin.DSF_ConstructDetector =
					    reinterpret_cast<DSF_ConstructDetector_ptr>(dlsym(dlobj, "DSF_ConstructDetector"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validdetector = false; }
				}
				if (validdetector) {
					plugin.DSF_DestroyDetector =
					    reinterpret_cast<DSF_DestroyDetector_ptr>(dlsym(dlobj, "DSF_DestroyDetector"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validdetector = false; }
				}

				// Detector Function Loading
				bool validgeometry = true;
				if (validgeometry) {
					plugin.DSF_ConstructGeometry =
					    reinterpret_cast<DSF_ConstructGeometry_ptr>(dlsym(dlobj, "DSF_ConstructGeometry"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validgeometry = false; }
				}
				if (validgeometry) {
					plugin.DSF_DestroyGeometry =
					    reinterpret_cast<DSF_DestroyGeometry_ptr>(dlsym(dlobj, "DSF_DestroyGeometry"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validgeometry = false; }
				}

				// Detector Function Loading
				bool validgenerator = true;
				if (validgenerator) {
					plugin.DSF_LoadFluxGenerator =
					    reinterpret_cast<DSF_LoadFluxGenerator_ptr>(dlsym(dlobj, "DSF_LoadFluxGenerator"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validgenerator = false; }
				}
				if (validgenerator) {
					plugin.DSF_DestroyFluxGenerator =
					    reinterpret_cast<DSF_DestroyFluxGenerator_ptr>(dlsym(dlobj, "DSF_DestroyFluxGenerator"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validgenerator = false; }
				}

				// Print loading summary
				plugin.NObjects = (*(plugin.DSF_NDynamicObjects))();
				std::cout << "\tSuccessfully loaded dynamic processor manifest: "
				          << plugin.soloc << ". Contains " << plugin.NObjects
				          << " Objects." << std::endl;

				if (plugin.NObjects) {
					Manifests.push_back(plugin);
					std::cout << "Added : " << plugin.soloc << " manifest." << std::endl;
					NManifests++;
				} else {
					dlclose(dlobj);
				}
			}
		}
		closedir(dir);
	}
}


DynamicObjectLoader* DynamicObjectLoader::Get() {
	if (!glblDSF) { glblDSF = new DynamicObjectLoader(); }
	return glblDSF;
}

VProcessor* DynamicObjectLoader::ConstructDynamicProcessor(DBTable tbl) {
	VProcessor* vp = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].DSF_ConstructProcessor == NULL) continue;
		vp = (*(Manifests[i].DSF_ConstructProcessor))(&tbl);
		if (vp) break;
	}
	return vp;
}

VDetector* DynamicObjectLoader::ConstructDynamicDetector(DBTable tbl) {
	VDetector* vd = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].DSF_ConstructDetector == NULL) continue;
		vd = (*(Manifests[i].DSF_ConstructDetector))(&tbl);
		if (vd) break;
	}
	return vd;
}

GeoObject* DynamicObjectLoader::ConstructDynamicGeometry(DBTable tbl) {
	GeoObject* geo = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].DSF_ConstructGeometry == NULL) continue;
		std::cout << "Attempting to get dyn geometry" << std::endl;
		geo = (*(Manifests[i].DSF_ConstructGeometry))(&tbl);
		std::cout << "Got dyn geometry : " << geo << std::endl;
		if (geo) {
			std::cout << "Exitting, no need to search more" << std::endl;
			break;
		}
	}
	std::cout << "Returning dynamic geometry " << geo << std::endl;
	return geo;
}


G4VUserPrimaryGeneratorAction*
DynamicObjectLoader::LoadDynamicFluxGenerator(std::string type) {
	G4VUserPrimaryGeneratorAction* vd = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].DSF_LoadFluxGenerator == NULL) continue;
		G4VUserPrimaryGeneratorAction* vd = (*(Manifests[i].DSF_LoadFluxGenerator))(type);
		if (vd) break;
	}
	return vd;
}




DynamicObjectLoader::~DynamicObjectLoader() { Manifests.clear(); }


