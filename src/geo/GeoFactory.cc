#include "geo/GeoFactory.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "db/DynamicObjectLoader.hh"

#include "simple/GeoBox.hh"
#include "simple/GeoTubs.hh"
#include "simple/GeoCone.hh"
#include "simple/GeoEllipticalTube.hh"
#include "simple/GeoEllipticalTunnel.hh"

#include "cadmesh/GeoCADMesh.hh"

using namespace COSMIC;

GeoObject* GeoFactory::ConstructGeometry(DBTable table) {

  std::string type = table.GetS("type");
  std::cout << "GEO: Constructing " << type << " : " << table.GetIndexName() <<  std::endl;

  // Core Constructors
  if (type.compare("box") == 0) return new GeoBox(table);
  if (type.compare("tubs") == 0) return new GeoTubs(table);
  if (type.compare("cons") == 0) return new GeoCone(table);
  if (type.compare("eliptube") == 0) return new GeoEllipticalTube(table);
  if (type.compare("eliptunnel") == 0) return new GeoEllipticalTunnel(table);

  // Cadmesh Constructors
#ifdef __USE_CADMESH__
  if (type.compare("cadmesh") == 0) return new GeoCADMesh(table);
#endif

  // Dynamic Constructors
  GeoObject* geo = DynamicObjectLoader::Get()->ConstructDynamicGeometry(table);
  if (geo) return geo;

  // Fail if none loaded!
  std::cout << "Failed to Construct Geometry" << std::endl;
  throw;
  return 0;
}
