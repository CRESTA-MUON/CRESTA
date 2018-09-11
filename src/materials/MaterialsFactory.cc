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
#include "MaterialsFactory.hh"

// G4 Headers
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4NistElementBuilder.hh"
#include "G4NistMaterialBuilder.hh"
#include "G4Pow.hh"
#include "G4NistManager.hh"

// COSMIC Headers
#include "db/DB.hh"

// namespace COSMIC
using namespace COSMIC;

G4Element* MaterialsFactory::GetElement(std::string name) {

  // Get the material manager
  G4NistManager* nist = G4NistManager::Instance();
  G4Element* mat;

  // Try to find by atomic symbol
  mat = nist->FindOrBuildElement(name);
  if (mat) return mat;

  // If not available we have to build the element
  DBTable mattbl     = DB::Get()->GetTable("ELEMENT", name);
  double atomicmass  = mattbl.GetG4D("atomic_mass");
  int atomicnumber   = mattbl.GetD("atomic_number");
  std::string symbol = mattbl.GetS("symbol");

  // Create material
  mat = new G4Element(name, symbol, atomicnumber, atomicmass );
  return mat;
}

G4Material* MaterialsFactory::GetMaterial(std::string name) {

  // Get the Material Manager
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* mat;

  // First Check if its inside the nist manager
  mat = nist->FindOrBuildMaterial(name);

  if (!mat) {

    // If not, find the material inside custom database
    DBTable mattbl = DB::Get()->GetTable("MATERIAL", name);
    std::vector<std::string> elements = mattbl.GetVecS("element_names");
    std::vector<double>      counts   = mattbl.GetVecD("element_counts");
    G4double density = mattbl.GetG4D("density");

    // Create the material
    std::cout << "MAT: Creating : " << name << std::endl;
    mat = new G4Material(name, density, counts.size());
    for (uint j = 0; j < counts.size(); j++) {
      std::cout << "MAT: --> Element : " << elements[j] << std::endl;
      G4Element* ele = GetElement(elements[j]);
      mat->AddElement( ele, counts[j] );
    }
    std::cout << "MAT: Total Material Density : " << mat->GetDensity()*cm3 / g << " g/cm3" << std::endl;

    // Dump the Table of registered materials
    // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  }

  G4MaterialPropertiesTable* props = GetMaterialPropertiesTable(name);
  if (props) {
    mat->SetMaterialPropertiesTable(props);
    if (props->ConstPropertyExists("BIRKSCONSTANT")) {
      // mat->GetIonisation()->SetBirksConstant( props->GetProperty("BIRKSCONSTANT"));
    }
  }


  return mat;
}


G4VisAttributes* MaterialsFactory::GetVisForMaterial(DBTable table) {

  // Make new vis attributes
  G4VisAttributes* vis = NULL;

  // If this isn't already a material table, check if database has some defaults
  if (table.GetTableName().compare("MATERIAL") != 0 and table.Has("material")) {
    std::string mat_name = table.GetS("material");
    vis = GetVisForMaterial(mat_name);
  }

  // Make if not already
  if (!vis) vis = new G4VisAttributes();

  // Optional visualization parts

  // Set the colour definition from table
  if (table.Has("color")) {
    std::vector<double> color = table.GetVecD("color");
    if (color.size() == 3) // RGB
      vis->SetColour(G4Colour(color[0], color[1], color[2]));
    else if (color.size() == 4) // RGBA
      vis->SetColour(G4Colour(color[0], color[1], color[2], color[3]));
  }

  // Set drawstyle from table
  if (table.Has("drawstyle")) {
    std::string drawstyle = table.GetS("drawstyle");
    if (drawstyle == "wireframe")
      vis->SetForceWireframe(true);
    else if (drawstyle == "solid")
      vis->SetForceSolid(true);
    else throw;
  }

  // Draw all edges from table
  if (table.Has("force_auxedge")) {
    int force_auxedge = table.GetI("force_auxedge");
    vis->SetForceAuxEdgeVisible(force_auxedge == 1);
  }

  return vis;
}

G4VisAttributes* MaterialsFactory::GetVisForMaterial(std::string name) {

  // Get the vis table for this material. If none found return NULL
  if (!DB::Get()->HasTable("MATERIAL", name)) return NULL;
  DBTable table = DB::Get()->GetTable("MATERIAL", name);

  // Build vis from this table
  G4VisAttributes* vis = GetVisForMaterial(table);

  // Return pointer to new vis
  return vis;
}

G4MaterialPropertiesTable* MaterialsFactory::GetMaterialPropertiesTable(std::string name) {

  G4MaterialPropertiesTable* mat = NULL;

  // Get the properties table for this material. If none found, return NULL
  if (DB::Get()->HasTable("PROPERTIES", name)) {
    DBTable table = DB::Get()->GetTable("PROPERTIES", name);
    mat = GetMaterialPropertiesTable(table);
  }


  if (DB::Get()->HasTable("MATERIAL", name)) {
    DBTable table = DB::Get()->GetTable("MATERIAL", name);
    mat = GetMaterialPropertiesTable(table);
  }

  return mat;
}

G4MaterialPropertiesTable* MaterialsFactory::GetMaterialPropertiesTable(DBTable table) {

  G4MaterialPropertiesTable* mat = new G4MaterialPropertiesTable();
  std::vector<std::string> allowed_properties;
  allowed_properties.push_back("WLSTIMECONSTANT");
  allowed_properties.push_back("BIRKSCONSTANT");
  allowed_properties.push_back("FASTCOMPONENT");
  allowed_properties.push_back("SLOWCOMPONENT");
  allowed_properties.push_back("RINDEX");
  allowed_properties.push_back("ABSLENGTH");
  allowed_properties.push_back("SCINTILLATIONYIELD");
  allowed_properties.push_back("RESOLUTIONSCALE");
  allowed_properties.push_back("SLOWTIMECONSTANT");
  allowed_properties.push_back("FASTTIMECONSTANT");
  allowed_properties.push_back("YIELDRATIO");
  allowed_properties.push_back("BIRKSCONSTANT");
  allowed_properties.push_back("REFLECTIVITY");
  allowed_properties.push_back("EFFICIENCY");

  for (int i = 0; i < allowed_properties.size(); i++) {
    std::string name = allowed_properties[i];
    if (table.Has(name + "_Y")) {

      std::vector<G4double> yvals = table.GetVecG4D(name + "_Y");

      std::vector<G4double> xvals;
      if (table.Has(name + "_X")) xvals = table.GetVecG4D(name + "_X");
      else if (table.Has("PROPERTIES_X")) xvals = table.GetVecG4D("PROPERTIES_X");
      else {
        std::cout << "NO X" << std::endl;
        throw;
      }

      if (xvals.size() != yvals.size()) {
        std::cout << "Material X Y must be equal in length" << std::endl;
        throw;
      }
      mat->AddProperty(name.c_str(), &xvals[0], &yvals[0], xvals.size());

    } else if (table.Has(name)) {

      mat->AddConstProperty(name.c_str(), table.GetG4D(name));

    }
  }

  return mat;
}





