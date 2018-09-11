#ifndef __GEO_IMPSNeutronBlock__HH__
#define __GEO_IMPSNeutronBlock__HH__

#include "G4Box.hh"

#include "db/DBTable.hh"
#include "geo/GeoSolid.hh"

namespace CRESTA {

/// Simple solid box geometry object
class IMPSNeutronBlock : public GeoSolid {
public:
  /// Create a IMPSNeutronBlock from a table
  IMPSNeutronBlock(DBTable table);

  /// Construct the solid volume for this IMPSNeutronBlock
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace IMPSNeutronBlock
#endif

