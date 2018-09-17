#ifndef __STLWriter_HH__
#define __STLWriter_HH__

// System Includes
#include <vector>

// namespace TERRAIN
namespace TERRAIN {

// Forward Declarations
struct triangle;
class Vector;

/// Determines the normal vector of a triangle from three vertices
Vector normalOf(const Vector&, const Vector&, const Vector&);

/// Creates a triangle and its normal vector from three vertices
triangle createTriangle(const Vector&, const Vector&, const Vector&);

/// Writes a triangle into the STL file
void addTriangle(triangle t);

/// Takes a height array of variable length and turns it into an STL file
void writeSTLfromArray(std::vector<float>&, int, int, float);

} // - namespace TERRAIN
#endif
