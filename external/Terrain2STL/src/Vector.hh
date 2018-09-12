#ifndef __TERRAIN_VECTOR_HH__
#define __TERRAIN_VECTOR_HH__


// namespace TERRAIN
namespace TERRAIN {

// Forward Declarations
class LatLng;

/// Simple vector class
class Vector {
public:
	/// Set constructor
	Vector (float, float, float);
	/// Empty constructor
	Vector (void);

	/// Vector add
	Vector add(const Vector&);
	/// Vector subtract
	Vector subtract(const Vector&);
	/// Vector scale
	Vector multiply(float);

	/// Print Values
	void print();

	/// Convert to latlong vector
	LatLng toSpherical(void);

	float x, y, z;
};

} // - namespace TERRAIN
#endif
