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
std::vector<float> getElevations(float _lat, float _lng, float _lat2, float _lng2, int _latn, int _lngn);

bool checkTileBlackList(std::string tileName);

} // - namespace TERRAIN
#endif
