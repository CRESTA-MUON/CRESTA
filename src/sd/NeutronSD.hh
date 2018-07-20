#ifndef __NeutronSD_HH__
#define __NeutronSD_HH__

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "g4root.hh"
#include "analysis/VProcessor.hh"
#include "analysis/VDetector.hh"

namespace COSMIC {

// Forward Declarations
class DBTable;

/// True Neutron Tracker Detector Object :
class NeutronSD : public VDetector {

public:

  /// Main constructor from a database table
  NeutronSD(DBTable table);
  /// Simple C++ constructor
  NeutronSD(std::string name, std::string id, bool autoprocess=true, bool autosave=true);
  /// Destructor
  ~NeutronSD(){};

  /// Main processing
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  // // Getter Functions
  // // - Return the number of unique track/parent ids
  // inline int GetMultiplicity(){ return fNeutronEnergyMapped.size(); };
  //
  // inline double      GetTotalEnergyDep(){
  //     double total_energy=0;
  //     for ( std::map< std::pair<G4int, G4int> , double >::iterator  it = fNeutronEnergyMapped.begin(); it != fNeutronEnergyMapped.end(); it++ ) total_energy+= (it->second);
  //     return total_energy;
  //   };
  //
  // inline G4ThreeVector GetAverageMomentum(){
  //   G4ThreeVector mom(0,0,0);
  //   for ( std::map< std::pair<G4int, G4int> , G4ThreeVector >::iterator  it = fNeutronMomMapped.begin(); it != fNeutronMomMapped.end(); it++ ) mom+=it->second;
  //   mom/=fHits;
  //   return mom;
  // };
  //
  //     inline double GetAverageTime(){
  //       double t = 0;
  //       for ( std::map< std::pair<G4int, G4int> , double >::iterator  it = fNeutronTimeMapped.begin(); it != fNeutronTimeMapped.end(); it++ ) t+=it->second;
  //       t/=fHits;
  //       return t;
  //     };
  //
  //         inline G4ThreeVector GetAveragePosition(){
  //           G4ThreeVector p(0,0,0);
  //           for ( std::map< std::pair<G4int, G4int> , G4ThreeVector >::iterator  it = fNeutronPosMapped.begin(); it != fNeutronPosMapped.end(); it++ ) p+=it->second;
  //           p/=fHits;
  //           return p;
  //         };
  //
  //     inline double GetAverageKE(){
  //       double ke = 0;
  //       for ( std::map< std::pair<G4int, G4int> , double >::iterator  it = fNeutronKEMapped.begin(); it != fNeutronKEMapped.end(); it++ ) ke+=it->second;
  //       ke/=fHits;
  //       return ke;
  //     };

      inline std::vector<int> GetParentID(){return fNeutronParentID;};
      inline std::vector<int> GetTrackID(){return fNeutronTrackID;};
      inline std::vector<int> GetEventID(){return fNeutronEventID;};

      inline std::vector<double> GetEnergyDeposited(){return fNeutronEnergyDeposited;};
      inline std::vector<double> GetKineticEnergy(){return fNeutronKE;};
      inline std::vector<double> GetTimes(){return fNeutronTime;};
      inline std::vector<G4ThreeVector> GetPosition(){return fNeutronPos;};
      inline std::vector<G4ThreeVector> GetMomentum(){return fNeutronMom;};

protected:

  int fHits; //< Number of hits


  std::vector<G4double> fNeutronEnergyDeposited;
  std::vector<G4double> fNeutronKE;
  std::vector<G4double> fNeutronTime;
  std::vector<G4ThreeVector> fNeutronPos;
  std::vector<G4ThreeVector> fNeutronMom;

  std::vector<G4int> fNeutronEventID;
  std::vector<G4int> fNeutronParentID;
  std::vector<G4int> fNeutronTrackID;

  // // Need to know about unqiue particles so keep a map data structure
  // // Key : A pair of unique track and parent IDs
  // // Value : Energy deposited, time
  // std::map< std::pair<G4int,G4int>, double > fNeutronEnergyMapped;// Summed
  // std::map< std::pair<G4int,G4int>, double > fNeutronTimeMapped;// Averaged
  // std::map< std::pair<G4int,G4int>, G4ThreeVector > fNeutronPosMapped;// Averaged
  // std::map< std::pair<G4int,G4int>, G4ThreeVector > fNeutronMomMapped;// Averaged within event
  // std::map< std::pair<G4int,G4int>, G4double > fNeutronKEMapped;// Averaged within event
  //
  // int fNeutronEventID; ///< Edep Ntuple Index
  // int fNeutronTrackID; ///< Edep Ntuple Index
  // int fNeutronParentID; ///< Edep Ntuple Index

};

///  Neutron Processor Object :
/// By default this is created alongside the true Neutron
/// tracker object, so the info is automatically added
/// to the TTree.
class NeutronProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  NeutronProcessor(NeutronSD* trkr, bool autosave=true);
  /// Destructor
  ~NeutronProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Draw function
  void DrawEvent();

protected:

  NeutronSD* fTracker; ///< Pointer to associated tracker SD

  bool fSave; ///< Flag to save event info automatically

  int fNeutronTimeIndex; ///< Time Ntuple Index
  int fNeutronEdepIndex; ///< Edep Ntuple Index
  int fNeutronIDIndex; ///< Edep Ntuple Index
  int fNeutronKEIndex; ///< Edep Ntuple Index
  int fNeutronMultIndex;
  int fNeutronPosXIndex;
  int fNeutronPosYIndex;
  int fNeutronPosZIndex;
  int fNeutronMomXIndex; ///< MomX Ntuple Index
  int fNeutronMomYIndex; ///< MomY Ntuple Index
  int fNeutronMomZIndex; ///< MomZ Ntuple Index
  int fNeutronPosRIndex;
  int fNeutronPosThetaIndex;

  int fNeutronTrackIDIndex; ///< MomY Ntuple Index
  int fNeutronParentIDIndex; ///< MomZ Ntuple Index
  int fNeutronEventIDIndex; ///< MomZ Ntuple Index


};


}
#endif
