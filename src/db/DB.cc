#include "DB.hh"
#include <iostream>
#include <sstream>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "DBTable.hh"

using namespace CRESTA;

DB::DB()
{
  CreateDataBase("default");
  SelectDataBase("default");
  fDefaultTables = &(fAllTables["default"]);
}

// DB* CRESTA_DBInstance(){
//   static DB* singletonInstance = new DB();
//   return singletonInstance;
// }

DB* DB::Get() { 
  return fPrimary == 0 ? fPrimary = new DB() : fPrimary; };

DB::~DB()
{
}

void DB::CreateDataBase(std::string dataid)
{
  std::cout << "DB : --> Creating Database : " << dataid << std::endl;
  fAllTables[dataid] = std::vector<DBTable>();
}

void DB::SelectDataBase(std::string dataid)
{
  std::cout << "DB : --> Selecting Database : " << dataid << std::endl;
  fCurrentTables = &(fAllTables[dataid]);
}

int DB::Load(std::string filename)
{
  // Try to get file info assuming the name is literal
  struct stat s;
  stat(filename.c_str(), &s);
  std::string f = filename;
  std::string suf = ".db";

  if (S_ISDIR(s.st_mode)) {
    return LoadAll(filename);
  } else if (f.rfind(suf) == (f.size() - suf.size())) {
    return LoadFile(filename);
  } else {
    return 1;
  }

  std::cout << "Cannot Load Data Tables : " << filename << std::endl;
  throw;
  return 0;
}

int DB::LoadFile(std::string filename)
{
  std::cout << "DB : --> Loading '" << filename << "'' ... ";
  // Try to read and parse it as databse or JSON file.
  std::vector<DBTable> contents = DBJSONParser::parsevals(filename);

  std::vector<DBTable>::iterator itbl;
  for (itbl = contents.begin(); itbl != contents.end(); itbl++) {
    DBTable table = (*itbl);
    AddTable(table);
    // (*fCurrentTables).push_back(table);
  }

  std::cout << " --> Complete. (Loaded " << contents.size() << " Tables)" << std::endl;
  Finalise();
  return 1;
}

int DB::LoadAll(std::string dirname, std::string pattern)
{
  std::cout << "DB : Loading all DB files from " << dirname << pattern << std::endl;
  pattern = dirname + "/*"; //pattern;
  glob_t g;

  if (glob(pattern.c_str(), 0, 0, &g) == 0) {
    for (unsigned i = 0; i < g.gl_pathc; i++) {
      std::string path(g.gl_pathv[i]);
      //std::cout << "DB: Loading " << path << " ... ";
      if (!Load(path)) {
        std::cout << "Load Failed!" << std::endl;
        globfree(&g);
        return 0;
      }
    }
  }
  globfree(&g);
  return 1;
}

void DB::Finalise() {

  // To finalise the database we want to expand all databases
  // that have the entry "clone_table" in them.
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    DBTable tbl = (*fCurrentTables)[i];
    if (!tbl.Has("clone")) continue;

    // Get the table to be cloned
    std::string clonename = tbl.GetS("clone");
    DBTable clonetbl = GetTable(tbl.GetTableName(), clonename);

    // Make new table from clone
    DBTable newtbl = DBTable(tbl.GetJSON());

    // Loop over fields and update
    newtbl.UpdateFields(&clonetbl);
    // newtbl->SetIndexName(tbl->GetIndexName());
    (*fCurrentTables)[i] = newtbl;

  }
}

bool DB::HasTable(std::string tablename, std::string index) {
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    if (index.compare((*fCurrentTables)[i].GetIndexName()) != 0) continue;
    return true;
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      if (index.compare((*fDefaultTables)[i].GetIndexName()) != 0) continue;
      return true;
    }
  }
  return false;
}


DBTable  DB::GetTable (std::string tablename, std::string index) {
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    if (index.compare((*fCurrentTables)[i].GetIndexName()) != 0) continue;
    return (*fCurrentTables)[i].Clone();
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      if (index.compare((*fDefaultTables)[i].GetIndexName()) != 0) continue;
      return (*fDefaultTables)[i].Clone();
    }
  }
  throw DBNotFoundError(tablename, index, "");
  return fNullTable;
}

DBTable* DB::GetLink  (std::string tablename, std::string index) {
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    if (index.compare((*fCurrentTables)[i].GetIndexName()) != 0) continue;
    return &(*fCurrentTables)[i];
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      if (index.compare((*fDefaultTables)[i].GetIndexName()) != 0) continue;
      return &(*fDefaultTables)[i];
    }
  }
  throw DBNotFoundError(tablename, index, "");
  return 0;
}


bool DB::HasTables (std::string tablename) {
  std::vector<DBTable> tableset;
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    return true;
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      return true;
    }
  }
  return false;
}


std::vector<DBTable>  DB::GetTableGroup (std::string tablename) {

  std::vector<DBTable> tableset;
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    std::string index = (*fCurrentTables)[i].GetIndexName();
    tableset.push_back((*fCurrentTables)[i].Clone());
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      std::string index = (*fDefaultTables)[i].GetIndexName();
      tableset.push_back((*fDefaultTables)[i].Clone());
    }
  }
  return tableset;
}

std::vector<DBTable*> DB::GetLinkGroup  (std::string tablename) {
  std::vector<DBTable*> tableset;
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    std::string index = (*fCurrentTables)[i].GetIndexName();
    tableset.push_back(&(*fCurrentTables)[i]);
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      std::string index = (*fDefaultTables)[i].GetIndexName();
      tableset.push_back(&(*fDefaultTables)[i]);
    }
  }
  return tableset;
}

void DB::AddTable(DBTable tbl) {
  std::string name = tbl.GetTableName();
  std::string index = tbl.GetIndexName();
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (name.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    if (index.compare((*fCurrentTables)[i].GetIndexName()) != 0) continue;
    (*fCurrentTables)[i] = tbl;
    return;
  }
  fCurrentTables->push_back(tbl);
}


DB* DB::fPrimary = NULL;

std::string DB::GetDataPath(std::string dbtype) {
  std::cout << "getting : " << dbtype << "_DATA_PATH" << std::endl;
  
  const char* chars = getenv((dbtype + "_DATA_PATH").c_str());
  std::string data = std::string(chars);
  std::cout << "DATA : " << data << std::endl;
  return data;
}

std::vector<double> DB::BlueColor(){
  std::vector<double> col;
  col.push_back(0.0);
  col.push_back(0.0);
  col.push_back(1.0);
  col.push_back(1.0);
  return col;
}

std::vector<double> DB::RedColor(){
  std::vector<double> col;
  col.push_back(1.0);
  col.push_back(0.0);
  col.push_back(0.0);
  col.push_back(1.0);
  return col;
}


void DB::PrintSplashScreen() {
  std::cout << "|====================================================|" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                 `-``.`....```````  |" << std::endl;
  std::cout << "|                                `.````..```-`````.  |" << std::endl;
  std::cout << "|        +s/    -ss              ..`````````...``.   |" << std::endl;
  std::cout << "|       `NM-    hMy              -.``````````` `     |" << std::endl;
  std::cout << "|       oMh    .MM-             .``````..``.`        |" << std::endl;
  std::cout << "|      `NM:    yMh              ` ````   `           |" << std::endl;
  std::cout << "|      +Mm   `+MM-           `                       |" << std::endl;
  std::cout << "|      mMd../s+MN.-/        .+`      `/`             |" << std::endl;
  std::cout << "|     /Mssso/``shs+-       -o+`      `++.            |" << std::endl;
  std::cout << "|     mM.                 :+++`      `/++:           |" << std::endl;
  std::cout << "|    :MM                `/ooo+`      `+ooo/`         |" << std::endl;
  std::cout << "|    `--               .+o++++`      `/+o+o+`        |" << std::endl;
  std::cout << "|                     -oooooo+`      `+oooooo.       |" << std::endl;
  std::cout << "|                  `-+oo+oo+o+`      `/++oo+oo:      |" << std::endl;
  std::cout << "|             ...-:++ooooooo++````````/+o+o++++/     |" << std::endl;
  std::cout << "|       `...:++oo++++++ooooo+/````````/+oooooo+++`   |" << std::endl;
  std::cout << "| /--//++o+ooooo++o+oooooooo/-````````-/oo+oooo+o+/+ |" << std::endl;
  std::cout << "| ++++ooooo+++oooooo+++ooo+/````````````:o++++oooo++ |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|====================================================|" << std::endl;
  std::cout << "| CRESTA : Cosmic Rays Simulation Package            |" << std::endl;
  std::cout << "| Chris Steer, Patrick Stowell, Lee Thompson         |" << std::endl;
  std::cout << "| contact : p.stowell@sheffield.ac.uk                |" << std::endl;
  std::cout << "|====================================================|" << std::endl;
}

std::vector<std::string> DB::GetDynamicLibraryPath(){
  std::vector<std::string> dynamicdirs;

  char const* envPath = getenv("CRESTA_OBJECTS_PATH");
  if (envPath) {
    std::vector<std::string> paths = DBUtils::ParseToStr(std::string(envPath),":");
    for (int i = 0; i < paths.size(); i++){
      if (paths[i].length()){
	dynamicdirs.push_back(paths[i]);
      }
    }
  }
  dynamicdirs.push_back("./");
  return dynamicdirs;
}

