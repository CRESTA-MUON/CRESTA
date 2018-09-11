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
#ifndef __COSMIC_MaterialsFactory_hh__
#define __COSMIC_MaterialsFactory_hh__

// System Headers
#include <iostream>

// Cosmic Headers
#include "db/DBTable.hh"

// Forward Declarations
class G4Material;
class G4Element;
class G4VisAttributes;

// namespace COSMIC
namespace COSMIC {


/// Detector Factory used to create SD from tables
namespace MaterialsFactory {

/// Get Element from string
G4Element* GetElement(std::string name);

/// Function to create detector objects from tables
G4Material* GetMaterial(std::string name);

/// Get some logical visualisation attributes depending on
/// material defaults
G4VisAttributes* GetVisForMaterial(DBTable table);

/// Get vis just based on material database name
G4VisAttributes* GetVisForMaterial(std::string name);

/// Get the material properties from its name
G4MaterialPropertiesTable* GetMaterialPropertiesTable(std::string name);

/// Build Material Properties from JSON table
G4MaterialPropertiesTable* GetMaterialPropertiesTable(DBTable table);


} // - namespace MaterialsFactory
} // - namespace COSMIC
#endif

