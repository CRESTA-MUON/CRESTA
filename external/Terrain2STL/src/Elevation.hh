#ifndef __TERRAIN_ELEVATION_HH__
#define __TERRAIN_ELEVATION_HH__

// System Includes
#include <vector>
#include <string>

// namespace TERRAIN
namespace TERRAIN {

/// Get the tile for a given lat long
std::string getTile(float, float);

/// returns the 'index' of the tile that contains a certain coordinate
/// quicker than getTile()
int getTileIndex(float lat, float lng);

/// width and heigth are in 'pixels'
std::vector<float> getElevations(double lat, double lng,
                                     double xrangelow, double xrangehigh,
                                     double yrangelow, double yrangehigh,
                                     double resolution);

bool checkTileBlackList(std::string tileName, std::string folder, std::string recomm);

} // - namespace TERRAIN
#endif
