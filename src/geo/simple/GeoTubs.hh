#ifndef __GEO_TUBS__HH__
#define __GEO_TUBS__HH__

#include "G4Tubs.hh"

#include "db/DBTable.hh"
#include "geo/GeoSolid.hh"

namespace CRESTA {

/// Simple solid tubs geometry object
class GeoTubs : public GeoSolid {
public:
  /// Create a GeoTubs from a table
  GeoTubs(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace CRESTA
#endif

