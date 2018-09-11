#ifndef __DetectorFactory_HH__
#define __DetectorFactory_HH__

#include <iostream>

namespace COSMIC {

// Forward Declarations
class DBTable;
class VDetector;

/// Detector Factory used to create SD from tables
namespace DetectorFactory {

/// Function to create detector objects from tables
VDetector* CreateSD(DBTable tbl);

} // - namespace DetectorFactory
} // - namespace COSMIC
#endif
