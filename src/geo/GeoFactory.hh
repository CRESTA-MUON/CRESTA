#ifndef __COSMIC_GeoFactory_HH__
#define __COSMIC_GeoFactory_HH__

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoObject.hh"

// namespace COSMIC
namespace COSMIC {

// namespace GeoFactory
namespace GeoFactory {
GeoObject* ConstructGeometry(DBTable table);
} // - namespace GeoFactory
} // - namespace COSMIC
#endif

