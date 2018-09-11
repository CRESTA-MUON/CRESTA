#ifndef __CRESTA_GeoFactory_HH__
#define __CRESTA_GeoFactory_HH__

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoObject.hh"

// namespace CRESTA
namespace CRESTA {

// namespace GeoFactory
namespace GeoFactory {
GeoObject* ConstructGeometry(DBTable table);
} // - namespace GeoFactory
} // - namespace CRESTA
#endif

