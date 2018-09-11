#include "ProcessorFactory.hh"
#include "G4LogicalVolume.hh"
#include "db/DBTable.hh"
#include "TrueMuonPoCAProcessor.hh"
#include "analysis/Analysis.hh"
#include "db/DB.hh"
#include "db/DynamicObjectLoader.hh"
namespace COSMIC {

VProcessor* ProcessorFactory::Construct(DBTable tbl) {

  // Retrieve the table that matches this sensitive
  std::string type = tbl.GetS("type");

  // Now Search for different types
  if (type.compare("truemuonpoca") == 0) return new TrueMuonPoCAProcessor(tbl);

  VProcessor* proc = DynamicObjectLoader::Get()->ConstructDynamicProcessor(tbl);
  if (proc) return proc;

  return 0;
}


void ProcessorFactory::ConstructProcessors() {

  /// Get the DB Table Set
  std::cout << "===============================" << std::endl;
  std::cout << "PRC: Building PROCESSORS " << std::endl;
  std::vector<DBTable> tables_clone = DB::Get()->GetTableGroup("PROCESSOR");
  std::vector<DBTable>::iterator trg_iter = tables_clone.begin();
  for (trg_iter = tables_clone.begin(); trg_iter != tables_clone.end(); trg_iter++) {

    DBTable trg_tab = (*trg_iter);
    std::string trg_id = trg_tab.GetIndexName();

    VProcessor* trg_obj = NULL; //Analysis::Get()->GetProcessor(trg_id);
    if (trg_obj) continue;

    // Create and register to analysis manager
    trg_obj = ProcessorFactory::Construct(trg_tab);


    if (!trg_obj) {
      std::cout << "Failed to construct a processor!" << std::endl;
      throw;
    }

    Analysis::Get()->RegisterProcessor(trg_obj);
  }


  return;
}


} // - namespace COSMIC
