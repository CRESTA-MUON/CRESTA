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
#ifndef __COSMIC_DB_HH__
#define __COSMIC_DB_HH__

// System Headers
#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

// Cosmic Headers
#include "db/DBTable.hh"

namespace COSMIC {

class DB {
public:

  // Make a static way to access the DB
  static DB *Get();
  static DB *fPrimary;

  DB();
  ~DB();
  DB(std::string filename);

  static std::string GetDataPath(std::string dbtype="CRESTA");
  static void PrintSplashScreen();

  void CreateDataBase(std::string dataid);

  void SelectDataBase(std::string dataid);

  bool HasTables(std::string name);
  bool HasTable(std::string name, std::string index);

  int Load(std::string filename);
  int LoadAll(std::string dirname, std::string pattern = "/*");
  int LoadFile(std::string filename);
  void Finalise();

  DBTable  GetTable (std::string tablename, std::string index);
  DBTable  CreateTable (std::string tablename, std::string index);

  DBTable* GetLink  (std::string tablename, std::string index);

  std::vector<DBTable>  GetTableGroup (std::string tablename);
  std::vector<DBTable*> GetLinkGroup  (std::string tablename);

  void AddTable(DBTable tbl);

  inline void SetOutputFile(std::string filename) { fOutputFile = filename; };
  inline std::string GetOutputFile() { return fOutputFile; };

  static std::vector<double> RedColor();
  static std::vector<double> BlueColor();
  std::vector<std::string> GetDynamicLibraryPath();


protected:

  std::map<std::string, std::vector<DBTable> > fAllTables;
  std::vector<DBTable>* fCurrentTables;
  std::vector<DBTable>* fDefaultTables;
  std::string fOutputFile;
  DBTable fNullTable;
  int fDataBaseCalls;

};

// DB* CRESTA_DBInstance();


} // - namespace COSMIC
#endif
