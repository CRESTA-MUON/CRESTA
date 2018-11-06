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
#ifndef __CRESTA_Analysis_hh__
#define __CRESTA_Analysis_hh__

// G4 Headers
#include "g4root.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4VVisManager.hh"

// Cosmic Headers
#include "analysis/VFluxProcessor.hh"
#include "geo/GeoUtils.hh"

// namespace CRESTA
namespace CRESTA {

// Forward Declarations
class VProcessor;
class VTrigger;
class VDetector;

/// Singleton Analysis Manager that handles all cosmic run processing
/// Contains pointers to all processors, and triggers so that it can
/// determine when events should be saved. Everything is based on the ROOT
/// analysis manager as the output file is quick to use.
class Analysis {
public:

  // ---------------------------------------------------------
  // Constructors
  // ---------------------------------------------------------
  /// Simple Constructor
  Analysis();
  /// Destructor keeps all processors alive for now....
  ~Analysis();


  // ---------------------------------------------------------
  // Singleton Access
  // ---------------------------------------------------------
  /// Singleton Access Function
  static inline Analysis *Get()
  { return fPrimary == 0 ? fPrimary = new Analysis : fPrimary; };
  /// Access G4 Analysis Manager
  inline G4AnalysisManager* GetG4AnalysisManager() { return fG4Manager; };
  /// Quick access function to ROOT manager
  static inline G4AnalysisManager* GetG4Manager() {
    return Get()->GetG4AnalysisManager();
  }


  // ---------------------------------------------------------
  // COUNTERS / IO
  // ---------------------------------------------------------
  /// Flags to use for current processing run mode
  enum RunMode {
    kEventMode,
    kTimeExposureMode,
    kTriggerMode
  };
  /// Manually set the processing run mode
  inline void SetMode(int i) { fRunMode = i;};

  /// Check if the run needs to end.
  void CheckAbortState();

  /// Start the timer for this run
  inline void StartTheClock() { fStartTime = time(0); };

  /// Set the output tag : file = outputtag.run.subrun.root
  inline void SetOutputTag(std::string s) { fOutputTag = s; };
  /// Set the output run : file = outputtag.run.subrun
  inline void SetOutputRun(int i) { fRunID = i; };
  /// Set the output subrun : file = outputtag.run.subrun
  inline void SetOutputSubRun(int i) { fSubRunID = i; };
  
  /// Set chunk processing size. Each subrun is this size.
  inline void SetChunkSize(int i) { fChunkSize = i; };
  /// Get Chunk Processing size. Each subrun is this size.
  inline int GetChunkSize() { return fChunkSize; };

  /// Add one to subrun number. Used in chunk processing.
  inline void IncrementSubRun() { fSubRunID++; };

  /// Get number of events generated so far
  inline int GetNEvents() { return fGeneratedEvents; };
  /// Get number of triggers saved so far
  inline int GetNSavedEvents() { return fSavedEvents; };

  /// Reset all run event counters
  void ResetCounters();

  /// Get the average trigger rate
  double GetEventRate();

  /// Print the current state of processing
  void PrintProgress(int curcount, int totalcount);


  // ---------------------------------------------------------
  // FLUX PROCESSORS / RUN CONTROL
  // ---------------------------------------------------------
  /// Set the flux processor (special case)
  inline void SetFluxProcessor(VFluxProcessor* p) {  fFluxProcessor = p; };
  /// Get the flux processor
  inline VFluxProcessor* GetFluxProcessor() {return fFluxProcessor;};

  /// Return current exposure from flux processor
  inline G4double GetExposureTime() { return fFluxProcessor->GetExposureTime(); };

  /// Set the exposure for run mode = kTimeExposureMode
  inline void SetRequiredExposure(double d) { fRequiredExposure = d; };
  /// Get the exposure for run mode = kTimeExposureMode
  inline double GetRequiredExposure() { return fRequiredExposure; };

  /// Set the triggers for mode = kTriggerMode
  inline void SetRequiredTriggers(int i) { fRequiredTriggers = i; };
  /// Get the triggers for mode = kTriggerMode
  inline double GetRequiredTriggers() { return fRequiredTriggers; };


  // ---------------------------------------------------------
  // Main Run Processing
  // ---------------------------------------------------------
  /// Setup NTuple processor info at start of run
  void BeginOfRunAction(const G4Run* run);
  /// Write NTuple processor info at end of run
  void EndOfRunAction(const G4Run* run);

  /// Reset processors/triggers at the start of event
  void BeginOfEventAction();
  /// Main processing loop
  void ProcessEvent(const G4Event* event);
  /// Record event using NTuple Manager
  void RecordEvent();
  /// Reset all processors and triggers
  void ResetState();


  // ---------------------------------------------------------
  // TRIGGERS
  // ---------------------------------------------------------
  /// Add a new trigger to vector of triggers
  void RegisterTrigger(VTrigger* t);
  /// Check trigger combination. Default is to search
  /// for at least one trigger being true.
  bool IsTriggered();
  /// Set all trigger states back to default values
  void ResetTriggers();
  /// Get Trigger by ID
  VTrigger* GetTrigger(std::string id, bool silentfail = false);
  /// Destroy Triggers
  void DestroyTriggers();


  // ---------------------------------------------------------
  // PROCESSORS
  // ---------------------------------------------------------
  /// Add a new processor to vector of processors
  void RegisterProcessor(VProcessor* p);
  /// Reset the state of all processors
  void ResetProcessors();
  /// Get Processor by ID
  VProcessor* GetProcessor(std::string id, bool silentfail = false);
  /// Remove all existing processors
  void DestroyProcessors();


  // ---------------------------------------------------------
  // DETECTORS
  // ---------------------------------------------------------
  /// Add a new detector to vector
  void RegisterDetector(VDetector* d);
  /// Reset the state of all detectors
  void ResetDetectors();
  /// Get the detector by its own ID
  VDetector* GetDetector(std::string id, bool silentfail = false);
  /// Remove all existing detectors
  void DestroyDetectors();


  // ---------------------------------------------------------
  // VISUALISATION
  // ---------------------------------------------------------
  /// Set analysis flag that visualisation is enabled
  inline void SetInteractive(bool b = true) { fInteractive = b; };

  /// Return the Vis Manager. If fInteractive=false, return NULL;
  G4VVisManager* GetVisManager();


  // ---------------------------------------------------------
  // OTHER
  // ---------------------------------------------------------
  /// Generates an output file containing maps of average density
  void BuildMCMap();



protected:

  static Analysis *fPrimary; ///< Singleton Object

  G4AnalysisManager* fG4Manager; ///< G4 Output Manager

  std::vector<VProcessor*> fProcessors; ///< Vector of all processing chunks
  std::vector<VTrigger*> fTriggers;     ///< Vector of all triggering chunks
  std::vector<VDetector*> fDetectors;   ///< Vector of all sensitive detectors

  VFluxProcessor* fFluxProcessor; ///< Flux processor special case. Can decide when to end run.

  int fSavedEvents;      ///< Number of saved events so far

  int fGeneratedEvents;       ///< Number of gen. events so far

  bool fNTuplesSetup; ///< Check Ntuples OKAY

  std::string fOutputTag; ///< Output file tag
  int fRunID; ///< Current run ID
  int fSubRunID; ///< Current subrun ID
  int fRunMode; ///< Processing run mode
  int fChunkSize; ///< Processing subrun event chunk size

  long int fStartTime; ///< Start time for this run
  double fLastCount; ///< Last time the counter was triggered  for printing

  G4double fRequiredExposure; ///< Total required exposure before run will end
  G4int fRequiredTriggers; ///< Total required triggers before run will end

  bool fInteractive; ///< Flag for if visualisation is turned on

};

} // - namespace CRESTA
#endif

