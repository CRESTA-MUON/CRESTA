//
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
//
// $Id: PhotonHit.cc 72250 2013-07-12 08:59:26Z gcosmo $
//
/// \file optical/LXe/src/PhotonHit.cc
/// \brief Implementation of the PhotonHit class
//
//
#include "PhotonHit.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Square.hh"
#include "db/DBTable.hh"
#include "analysis/Analysis.hh"

using namespace COSMIC;

G4ThreadLocal G4Allocator<PhotonHit>* PhotonHitAllocator = 0;

PhotonHit::PhotonHit() {}

PhotonHit::~PhotonHit() {}

PhotonHit::PhotonHit(const PhotonHit &right) : G4VHit()
{
  fPos = right.fPos;
  fEnergy = right.fEnergy;
  fTime = right.fTime;
}

const PhotonHit& PhotonHit::operator=(const PhotonHit &right) {
  fPos = right.fPos;
  fEnergy = right.fEnergy;
  fTime = right.fTime;
  return *this;
}

G4int PhotonHit::operator==(const PhotonHit &right) const {
  return (fTime == right.fTime);
}

void PhotonHit::Draw() {
  G4VVisManager* pVVisManager = COSMIC::Analysis::Get()->GetVisManager();
  if (pVVisManager)
  {
    G4Square square(fPos);
    square.SetScreenSize(6);
    square.SetFillStyle(G4Square::filled);
    G4Colour colour(1., 0., 0.);
    G4VisAttributes attribs(colour);
    square.SetVisAttributes(attribs);
    pVVisManager->Draw(square);
  }
}

void PhotonHit::Print() {}
