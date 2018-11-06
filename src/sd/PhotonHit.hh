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
// $Id: LXePMTHit.hh 72250 2013-07-12 08:59:26Z gcosmo $
//
/// \file optical/LXe/include/LXePMTHit.hh
/// \brief Definition of the LXePMTHit class
//
//
#ifndef PhotonHit_h
#define PhotonHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4VPhysicalVolume.hh"

#include "tls.hh"

class G4VTouchable;

class PhotonHit : public G4VHit
{
  public:
 
    PhotonHit();
    virtual ~PhotonHit();
    PhotonHit(const PhotonHit &right);

    const PhotonHit& operator=(const PhotonHit &right);
    G4int operator==(const PhotonHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
 
    virtual void Draw();
    virtual void Print();

    inline void SetPosition(G4ThreeVector v){ fPos = v; }
    inline G4ThreeVector GetPosition(){ return fPos; }

    inline void SetEnergy(G4double e){ fEnergy = e; }
    inline G4double GetEnergy(){ return fEnergy; }

    inline void SetTime(G4double e){ fTime = e; }
    inline G4double GetTime(){ return fTime; }

  private:

    G4ThreeVector fPos;
    G4double fEnergy;
    G4double fTime;
};

typedef G4THitsCollection<PhotonHit> PhotonHitsCollection;

extern G4ThreadLocal G4Allocator<PhotonHit>* PhotonHitAllocator;

inline void* PhotonHit::operator new(size_t){
  if(!PhotonHitAllocator)
      PhotonHitAllocator = new G4Allocator<PhotonHit>;
  return (void *) PhotonHitAllocator->MallocSingle();
}

inline void PhotonHit::operator delete(void *aHit){
  PhotonHitAllocator->FreeSingle((PhotonHit*) aHit);
}

#endif
