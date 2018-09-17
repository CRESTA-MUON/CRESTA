#include "DynamicObjectLoader.hh"
#include "TRegexp.h"
#include <dirent.h>
#include <dlfcn.h>
#include "db/DBTable.hh"
#include "db/DB.hh"

using namespace CRESTA;

DynamicObjectLoader::DynamicObjectLoader() : NObjects(0), NManifests(0) {
	LoadPlugins();
	std::cout << "Loaded : " << NObjects
	          << " from " << NManifests
	          << " shared object libraries." << std::endl;
}
DynamicObjectLoader* DynamicObjectLoader::glblDSF = NULL;

DynamicObjectLoader::PluginManifest::~PluginManifest() {
	std::cout << "Deleting a plugin manifest : " << soloc << std::endl;

	std::cout << " dllib : " << dllib << std::endl;
	std::cout << " Print : " << CRESTA_PrintManifest << std::endl;
	std::cout << " Det : "   << CRESTA_ConstructDetector << std::endl;
	std::cout << " Geo : "   << CRESTA_ConstructGeometry << std::endl;
	std::cout << " Flux : "  << CRESTA_LoadFluxGenerator << std::endl;

	std::cout << "Done." << std::endl;
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
		std::cout << "New Directory" << std::endl;
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
				PluginManifest* plugin = new PluginManifest();
				plugin->dllib = dlobj;
				plugin->soloc = (dirpath + ent->d_name);
				/*
				std::cout << "Searching CRESTA_NDynamicObjects" << std::endl;
				plugin->CRESTA_NDynamicObjects =
				    reinterpret_cast<CRESTA_NDynamicObjects_ptr>(dlsym(dlobj, "CRESTA_NDynamicObjects"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					dlclose(dlobj);
					continue;
				}

				plugin->CRESTA_GetDynamicObjectName =
				    reinterpret_cast<CRESTA_GetDynamicObjectName_ptr>(dlsym(dlobj, "CRESTA_GetDynamicObjectName"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					dlclose(dlobj);
					continue;
				}
				*/

				// Processor Function Loading
				bool validmanifest = true;
				plugin->CRESTA_PrintManifest =
				    reinterpret_cast<CRESTA_PrintManifest_ptr>(dlsym(dlobj, "CRESTA_PrintManifest"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					validmanifest = false;
					plugin->CRESTA_PrintManifest = NULL;
					dlclose(dlobj);
					continue;
				}

				// Processor Function Loading
				bool validprocessor = true;
				plugin->CRESTA_ConstructProcessor =
				    reinterpret_cast<CRESTA_ConstructProcessor_ptr>(dlsym(dlobj, "CRESTA_ConstructProcessor"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					validprocessor = false;
					plugin->CRESTA_ConstructProcessor = NULL;
				}
				/*
				if (validprocessor) {
					plugin->CRESTA_DestroyProcessor =
					    reinterpret_cast<CRESTA_DestroyProcessor_ptr>(dlsym(dlobj, "CRESTA_DestroyProcessor"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validprocessor = false; }
				}
				*/

				// Detector Function Loading
				bool validdetector = true;
				plugin->CRESTA_ConstructDetector =
				    reinterpret_cast<CRESTA_ConstructDetector_ptr>(dlsym(dlobj, "CRESTA_ConstructDetector"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					validdetector = false;
					plugin->CRESTA_ConstructDetector = NULL;
				}

				/*
				if (validdetector) {
					plugin->CRESTA_DestroyDetector =
					    reinterpret_cast<CRESTA_DestroyDetector_ptr>(dlsym(dlobj, "CRESTA_DestroyDetector"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validdetector = false; }
				}
				*/

				// Detector Function Loading
				bool validgeometry = true;
				plugin->CRESTA_ConstructGeometry =
				    reinterpret_cast<CRESTA_ConstructGeometry_ptr>(dlsym(dlobj, "CRESTA_ConstructGeometry"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					validgeometry = false;
					plugin->CRESTA_ConstructGeometry = NULL;
				}
				/*
				if (validgeometry) {
					plugin->CRESTA_DestroyGeometry =
					    reinterpret_cast<CRESTA_DestroyGeometry_ptr>(dlsym(dlobj, "CRESTA_DestroyGeometry"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validgeometry = false; }
				}
				*/


				// Detector Function Loading
				bool validgenerator = true;
				plugin->CRESTA_LoadFluxGenerator =
				    reinterpret_cast<CRESTA_LoadFluxGenerator_ptr>(dlsym(dlobj, "CRESTA_LoadFluxGenerator"));
				dlerr = "";
				dlerr_cstr = dlerror();
				if (dlerr_cstr) {
					dlerr = dlerr_cstr;
				}
				if (dlerr.length()) {
					validgenerator = false;
					plugin->CRESTA_LoadFluxGenerator = NULL;
				}
				/*
				if (validgenerator) {
					plugin->CRESTA_DestroyFluxGenerator =
					    reinterpret_cast<CRESTA_DestroyFluxGenerator_ptr>(dlsym(dlobj, "CRESTA_DestroyFluxGenerator"));
					dlerr = "";
					dlerr_cstr = dlerror();
					if (dlerr_cstr) {
						dlerr = dlerr_cstr;
					}
					if (dlerr.length()) { validgenerator = false; }
				}
				*/


				// Print loading summary

				if (validmanifest && (validgenerator || validgeometry || validdetector || validprocessor)) {
					std::string manifestname = (*(plugin->CRESTA_PrintManifest))();

					if (std::find(fLoadedManifests.begin(), fLoadedManifests.end(), manifestname) == fLoadedManifests.end()) {

						std::cout << "\tSuccessfully loaded dynamic processor manifest: "
						          << plugin->soloc << "." << std::endl;
						std::cout << "Pushing Back" << std::endl;
						Manifests.push_back(*plugin);
						std::cout << "Adding loaded Manifest" << std::endl;
						fLoadedManifests.push_back(manifestname);
						std::cout << "Added : " << plugin->soloc << " manifest." << std::endl;
						NManifests++;
						std::cout << " End of if statement" << std::endl;

					} else {
						std::cout << "Closing object as we already have it." << std::endl;
						dlclose(dlobj);
					}
					std::cout << "End of valid manifest" << std::endl;
				} else {
					std::cout << "Closing object" << std::endl;
					dlclose(dlobj);
					delete plugin;
				}
				std::cout << "End of pointer checking" << std::endl;

			}
			std::cout << "End of readdir" << std::endl;
		}
		std::cout << "Closing directory" << std::endl;
		closedir(dir);
	}
	std::cout << "Loaded all plugins!" << std::endl;
}


DynamicObjectLoader* DynamicObjectLoader::Get() {
	if (!glblDSF) { glblDSF = new DynamicObjectLoader(); }
	return glblDSF;
}

VProcessor* DynamicObjectLoader::ConstructDynamicProcessor(DBTable tbl) {
	VProcessor* vp = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].CRESTA_ConstructProcessor == NULL) continue;
		vp = (*(Manifests[i].CRESTA_ConstructProcessor))(&tbl);
		if (vp) break;
	}
	return vp;
}

VDetector* DynamicObjectLoader::ConstructDynamicDetector(DBTable tbl) {
	VDetector* vd = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].CRESTA_ConstructDetector == NULL) continue;
		vd = (*(Manifests[i].CRESTA_ConstructDetector))(&tbl);
		if (vd) break;
	}
	return vd;
}

GeoObject* DynamicObjectLoader::ConstructDynamicGeometry(DBTable tbl) {
	GeoObject* geo = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].CRESTA_ConstructGeometry == NULL) continue;
		geo = (*(Manifests[i].CRESTA_ConstructGeometry))(&tbl);
		if (geo) { break; }
	}
	return geo;
}


G4VUserPrimaryGeneratorAction*
DynamicObjectLoader::LoadDynamicFluxGenerator(std::string type) {
	G4VUserPrimaryGeneratorAction* vd = NULL;
	for (int i = 0; i < Manifests.size(); i++) {
		if (Manifests[i].CRESTA_LoadFluxGenerator == NULL) continue;
		vd = (*(Manifests[i].CRESTA_LoadFluxGenerator))(type);
		if (vd) break;
	}
	return vd;
}




DynamicObjectLoader::~DynamicObjectLoader() { Manifests.clear(); }


