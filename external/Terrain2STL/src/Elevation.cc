#include "Elevation.hh"

// System Includes
#include <string>
#include <math.h>
#include <fstream>
#include <vector>
#include <iostream>

// namespace std (BAD)
using namespace std;
const float  PI=3.14159265358979f;


string TERRAIN::getTile(float lat, float lng) {
	string file;
	//-------get correct tile-------------------
	if (lat >= 0) {								//Positive is north
		file = "N";
	} else {
		file = "S";
	}
	file.append( to_string( abs( (int)floor(lat) ) ) );
	if (file.length() == 2) file.insert(1, "0");
	if (lng >= 0) {								//Positive is east
		file.append("E");
	} else {
		file.append("W");
	}
	file.append( to_string( abs( (int)floor(lng) ) ) );
	if (file.length() == 5)file.insert(4, "00");
	if (file.length() == 6)file.insert(4, "0");

	file.append(".hgt");
	// file.insert(0, "../hgt_files/");

	return file;
}


int TERRAIN::getTileIndex(float lat, float lng) {
	return (int)floor(lat) * 360 + (int)floor(lng);
}

bool TERRAIN::checkTileBlackList(std::string tileName) {

	// clog << " CANNOT FIND FILE : " << tileName << std::endl;

	std::string listitem;
	ifstream iFile;
	iFile.open("../hgt_files/blacklist.txt"); //Opens file
	while (iFile >> listitem) //While the file is copying into the first and last names
	{
		if (listitem.compare(tileName.c_str()) == 0) {
			clog << tileName << " --> FILE BLACKLISTED " << std::endl;
			return true;
		}
	}

	clog << " Cannot find a file but it is not blacklisted! : " << tileName << std::endl;
	throw;
	return false;
}


vector<float> TERRAIN::getElevations(double lat, double lng,
                                     double xrangelow, double xrangehigh,
                                     double yrangelow, double yrangehigh,
                                     double resolution) {

	// Initialise Tile Loader
	int tileNumber = 0;
	string tileName = "";
	ifstream file;
	file.open("../hgt_files/N44W070.hgt");

	// Choose first tile index
	int t = getTileIndex(lat, lng);
	double r_earth = 6371e3; // metres

	// Initialise temp vars
	int h;
	char number [2];
	int stepSize = 1;
	int baseHeight = 5;
	double waterDrop = 2;
	double vscale = 0.01;

	// Determine the step sizes
	int nx = int((xrangehigh - xrangelow) / resolution);
	int ny = int((yrangehigh - yrangelow) / resolution);
	clog << "NVALS : " << nx << " " << ny << std::endl;
	
	// Make list
	vector<float> outList(nx * ny);

	for (int x = 0; x < nx; x++) {
		for (int y = 0; y < ny; y++) {

			double xpos = xrangelow + resolution * x;
			double ypos = yrangelow + resolution * y;

			double latpos = lat + (ypos / r_earth) * (180 / PI);
			double lngpos = lng + (xpos / r_earth) * (180 / PI) / cos(lat * PI / 180);

			double elevations[2][2];
			double height = 0.0;
			for (int a = 0; a < 2; a++) {
				for (int b = 0; b < 2; b++) {


					double intlat = floor(latpos * 1200 + b) / 1200.0 + 0.000598907;
					double intlng = floor(lngpos * 1200 + a) / 1200.0;
					bool blacklisted = false;

					if (getTileIndex(intlat, intlng) != tileNumber) {
						tileNumber = getTileIndex(intlat, intlng);
						tileName = getTile(intlat, intlng);
						std::string tilePath = "../hgt_files/" + tileName;
						file.close();
						file.open(tilePath.c_str(), ios::in | ios::binary);
						if (!file.good()) {
							blacklisted = checkTileBlackList(tileName);
						}
					}

					if (!blacklisted) {
						int p = (int) (1201 * (intlng - floor(intlng)));		//x or lng component
						p += (int)(1201 * (ceil(intlat) - intlat)) * 1201;	//y or lat component

						file.seekg(p * 2, ios::beg);
						file.read(number, 2);
						h = number[1];
						if (h < 0) {
							h = h + 255;
						}
						h += number[0] * 256;
						if (h == 0) {
							h -= waterDrop / vscale;
						}
						elevations[a][b] = (double)h;
					} else {
						elevations[a][b] = waterDrop / vscale;
					}
				}
			}

			// Interpolate between our 4 longitude points
			double fracLat = latpos - floor(latpos * 1200) / 1200;
			double fracLng = lngpos - floor(lngpos * 1200) / 1200;
			fracLat *= 1200;
			fracLng *= 1200;
			double westLng = elevations[0][0] * (1 - fracLat)  +  elevations[0][1] * fracLat;
			double eastLng = elevations[1][0] * (1 - fracLat)  +  elevations[1][1] * fracLat;

			// Get the height from four way interpolation
			double intHeight = westLng * (1 - fracLng) + eastLng * fracLng;
			intHeight = elevations[0][0];
			// if (intHeight > 0) clog << latpos << " : " << lngpos << " : " << intHeight << std::endl;

			// Save to a list
			outList.at(y * nx + x) = intHeight * vscale + baseHeight;
		}
	}
	file.close();
	return outList;
}



// //let's go with mercator space for now (Terrain2STL style)
// for (int x = 0; x < _latn; x++) {
// 	for (int y = 0; y < _lngn; y++) {

// 		float u = stepSize * ((float)y / 1200 - (float)_lngn / 1200);
// 		float v = stepSize * ((float)x / 1200);

// 		//get the lat and lng for each point
// 		float lat = _lat + u;
// 		float lng = _lng + v;

// 		std::cout << "Lat Long : " << lat << " " << lng << std::endl;

// 		// Grab the elevations for all points
// 		float elevations[2][2];
// 		for (int a = 0; a < 2; a++) {
// 			for (int b = 0; b < 2; b++) {


// 				// Get the interpolate lat/long
// 				float intlat = floor(lat * 1200 + b) / 1200.0 + 0.000598907;
// 				float intlng = floor(lng * 1200 + a) / 1200.0;

// 				if (getTileIndex(intlat, intlng) != tileNumber) {
// 					tileNumber = getTileIndex(intlat, intlng);
// 					tileName = getTile(intlat, intlng);
// 					std::string tilePath = "../hgt_files/" + tileName;
// 					file.close();
// 					file.open(tilePath.c_str(), ios::in | ios::binary);
// 					if (!file.good()) {
// 						clog << "Cannot find : " << tileName << std::endl;
// 						clog << " --> Downloading now!" << std::endl;
// 						downloadTileHGT(tileName);
// 					}
// 				}

// 				// Get p value?
// 				int p = (int) (1201 * (intlng - floor(intlng)));		//x or lng component
// 				p += (int)(1201 * (ceil(intlat) - intlat)) * 1201;	//y or lat component

// 				// Read the tiles
// 				file.seekg(p * 2, ios::beg);
// 				file.read(number, 2);
// 				h = number[1];
// 				if (h < 0) {
// 					h = h + 255;
// 				}
// 				h += number[0] * 256;

// 				// Add to our elevations
// 				elevations[a][b] = (float)h;
// 			}
// 		}

// 		// Interpolate between our 4 longitude points
// 		float fracLat = lat - floor(lat * 1200) / 1200;
// 		float fracLng = lng - floor(lng * 1200) / 1200;
// 		fracLat *= 1200;
// 		fracLng *= 1200;
// 		float westLng = elevations[0][0] * (1 - fracLat)  +  elevations[0][1] * fracLat;
// 		float eastLng = elevations[1][0] * (1 - fracLat)  +  elevations[1][1] * fracLat;

// 		// Get the height from four way interpolation
// 		float intHeight = westLng * (1 - fracLng) + eastLng * fracLng;
// 		clog << lat << " : " << lng << " : " << intHeight << std::endl;

// 		// Save to a list
// 		outList.at(y * width + x) = intHeight + baseHeight;
// 	}
// }
// file.close();
// clog << "Done getting elevations\n";
// }


// vector<float> TERRAIN::getElevations(float _lat, float _lng, int width, int height, float vscale, float rot, int waterDrop, int baseHeight, int stepSize) {
// 	int tileNumber = 0;
// 	string tileName = "";
// 	ifstream file;
// 	vector<float> outList(width * height);

// 	file.open("../hgt_files/N44W070.hgt");

// 	clog << "Getting elevations" << std::endl;
// 	int h;
// 	char number [2];

// 	int t = getTileIndex(_lat, _lng);

// 	//let's go with mercator space for now (Terrain2STL style)
// 	for (int x = 0; x < width; x++) {
// 		for (int y = 0; y < height; y++) {
// 			// std::cout << "Getting : " << x << " " << y << std::endl;

// 			float u = (float)y / 1200 - (float)height / 1200;
// 			float v = (float)x / 1200;

// 			u *= stepSize;
// 			v *= stepSize;

// 			//get the lat and lng for each point
// 			float lat = _lat + u * cos(rot) + v * sin(rot);
// 			float lng = _lng + v * cos(rot) - u * sin(rot);

// 			std::cout << "Lat Long : " << lat << " " << lng << std::endl;


// 			//interpolate - maybe skip for rot0=0?

// 			float elevations[2][2];
// 			for (int a = 0; a < 2; a++) {		//x coord for Interpolation
// 				for (int b = 0; b < 2; b++) { //

// 					//interesting bit of code here.
// 					//floor(...)/1200 as no distortion but cannot handle tile edges
// 					//x+float() handles edges well but with distortion
// 					//lat+(float)b/1200
// 					float intlat = floor(lat * 1200 + b) / 1200.0 + 0.000598907;	//magic number??
// 					float intlng = floor(lng * 1200 + a) / 1200.0;
// 					//floor(lng*1200+a)/1200;

// 					if (getTileIndex(intlat, intlng) != tileNumber) {
// 						tileNumber = getTileIndex(intlat, intlng);
// 						tileName = getTile(intlat, intlng);
// 						std::string tilePath = "../hgt_files/" + tileName;
// 						file.close();
// 						file.open(tilePath.c_str(), ios::in | ios::binary);
// 						if (!file.good()) {
// 							clog << "Cannot find : " << tileName << std::endl;
// 							clog << " --> Downloading now!" << std::endl;
// 							downloadTileHGT(tileName);
// 						}
// 					}

// 					int p = (int) (1201 * (intlng - floor(intlng)));		//x or lng component
// 					p += (int)(1201 * (ceil(intlat) - intlat)) * 1201;	//y or lat component

// 					file.seekg(p * 2, ios::beg);
// 					file.read(number, 2);
// 					h = number[1];
// 					if (h < 0) {
// 						h = h + 255;
// 					}
// 					h += number[0] * 256;
// 					if (h == 0) {
// 						h -= waterDrop / vscale;
// 					}
// 					elevations[a][b] = (float)h;
// 				}
// 			}
// 			float fracLat = lat - floor(lat * 1200) / 1200;
// 			float fracLng = lng - floor(lng * 1200) / 1200;
// 			fracLat *= 1200;
// 			fracLng *= 1200;
// 			float westLng = elevations[0][0] * (1 - fracLat)  +  elevations[0][1] * fracLat;
// 			float eastLng = elevations[1][0] * (1 - fracLat)  +  elevations[1][1] * fracLat;

// 			//not sure about this....
// 			float intHeight = westLng * (1 - fracLng) + eastLng * fracLng;

// 			 // if (intHeight > 0) {
// 				// std::cout << "INTHEIGHT : " << intHeight << std::endl;
// 				// throw;
// 			// }
// 			//if(t!=getTileIndex(lat,lng)){
// 			//	intHeight=0;
// 			//}
// 			//float intHeight = (elevations[0][0]+elevations[0][1]+elevations[1][0]+elevations[1][1])/4;
// 			outList.at(y * width + x) = intHeight * vscale + baseHeight;
// 		}
// 	}
// 	file.close();
// 	clog << "Done getting elevations\n";
// 	return outList;
// }





