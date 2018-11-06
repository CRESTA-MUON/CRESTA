#ifndef LATLNG_H
#define LATLNG_H


// namespace TERRAIN
namespace TERRAIN{

// Forward Declarations
class Vector;

/// Latitude Longitude class
class LatLng {
  public:
  	// Constructor
    LatLng (float,float);

    // Print Values
    void print();

    // Convert to cartesian vector
    Vector toCartesian(void);

    float lat,lng;
};

} // - namespace TERRAIN
#endif
