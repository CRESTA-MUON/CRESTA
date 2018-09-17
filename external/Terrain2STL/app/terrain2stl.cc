#include <iostream>
#include <stdint.h>
#include <fstream>
#include <cmath>
#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include "Vector.hh"
#include "LatLng.hh"
#include "STLWriter.hh"
#include "Elevation.hh"

// namespace std (BAD)
using namespace std;
// namespace TERRAIN
using namespace TERRAIN;

float globalLat = 0;
vector<float> hList;
const float  PI=3.14159265358979f;

int main(int argc, char **argv)			//lat, long, width, height, verticalscale, rot, waterDrop, baseHeight, stepSize
//width and height are in units of steps or maybe degrees??
//rot is in degrees
{

	float lat = atof(argv[1]);	
	float lng = atof(argv[2]);		
	float xrangelow  = atof(argv[3]);
	float xrangehigh = atof(argv[4]);
	float yrangelow  = atof(argv[5]);
	float yrangehigh = atof(argv[6]);
	float resolution = atof(argv[7]);

	float r_earth = 6371e3; // metres
	float dx = xrangelow - 0;
	float dy = yrangelow - 0;
	float latlow = lat  + (dy / r_earth) * (180 / PI);
	float lnglow = lng + (dx / r_earth) * (180 / PI) / cos(lat * PI/180);	

	dx = xrangehigh - 0;
	dy = yrangehigh - 0;
	float lathigh = lat  + (dy / r_earth) * (180 / PI);
	float lnghigh = lng + (dx / r_earth) * (180 / PI) / cos(lat * PI/180);	

	clog << "Long/Lat Center : " << lat     << " : " << lng << std::endl;
	clog << "Long/Lat High : "   << lathigh << " : " << lnghigh << std::endl;
	clog << "Long/Lat Low : "    << latlow  << " : " << lnglow << std::endl;

	int nx = int((xrangehigh - xrangelow)/resolution);
	int ny = int((yrangehigh - yrangelow)/resolution);

	hList.resize(nx*ny,0);

	hList = TERRAIN::getElevations(lat, lng, xrangelow, xrangehigh, yrangelow, yrangehigh, resolution);
	writeSTLfromArray(hList,nx, ny, 3.1415926*lat/180);


	// clog << "STARTING PATRICKS PROJECT" << std::endl;

	// float lat;
	// float lng;
	// float lat2;
	// float lng2;
	// int width;
	// int height;
	// float userscale = 1;
	// float rot = 0;
	// int stepSize = 1;
	// int waterDrop = -2;			//millimeters
	// int baseHeight = 2;			//millimeters

	// //float true_verticalscale = 92.7;	//meters/arcsecond at equator
	// //old vertical scale was 23.2
	// float verticalscale = 92.7;			//true_verticalscale gives models that are too flat to be interesting

	// lat = atof(argv[1]);					//Latitude of NW corner
	// globalLat = 3.1415926*lat/180;
	// lng = atof(argv[2]);					//Longitude of NW corner
	// clog << "'Southwest' coordinate: (" << lat << "N, "<< lng << "E)\n";

	// lat2 = atof(argv[3]);					//Latitude of NW corner
	// lng2 = atof(argv[4]);					//Longitude of NW corner
	// clog << "'Northeast' coordinate: (" << lat2 << "N, "<< lng2 << "E)\n";


	// Want to choose a point and treat that as x0, y0.
	// Then choose an X and Y width, e.g. 

	// Step 1 metre east

	// new_latitude  = latitude  + (dy / r_earth) * (180 / pi);
	// new_longitude = longitude + (dx / r_earth) * (180 / pi) / cos(latitude * pi/180);

	// Make steps of 10m, people choose range in m
	// 

	/*
	var R = 6371e3; // metres
	var φ1 = lat1.toRadians();
	var φ2 = lat2.toRadians();
	var Δφ = (lat2-lat1).toRadians();
	var Δλ = (lon2-lon1).toRadians();

	var a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +
	        Math.cos(φ1) * Math.cos(φ2) *
	        Math.sin(Δλ/2) * Math.sin(Δλ/2);
	var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

	var d = R * c;
	*/

	// Treat the 

	// width = atoi(argv[3]);
	// height = atoi(argv[4]);
	// userscale = atof(argv[5]);
	// rot = atof(argv[6]);
	// rot = rot*PI/180;


	// waterDrop = atoi(argv[7]);
	// baseHeight = atoi(argv[8]);

	// stepSize = atoi(argv[9]);
	// clog << "Step size: " << stepSize << " units\n";

	// float scaleFactor = (userscale/verticalscale) / ((float) stepSize);

	// hList = TERRAIN::getElevations(lat,lng,width,height,scaleFactor,rot,waterDrop,baseHeight,stepSize);

	// int latn = abs(lat2 - lat)*100;
	// int lngn = latn;

	// clog << "Running getElevations!" << lat << " " << lng << " " << lat2 << " " << lng2 << " " << latn << " " << lngn << std::endl;
	// hList.resize((latn+1)*(lngn+1),0);

	// hList = TERRAIN::getElevations(lat,lng,lat2,lng2,latn,lngn);


	// // (float _lat, float _lng, float _lat2, float _lng2, int _latn, int _lngn)
	// //passing global lat as an xscale - only needed for
	// //writeSTLfromArray(hList,width,height,globalLat);
	// //passing zero as latittude to bypass lat compensation
	// writeSTLfromArray(hList,latn,lngn,globalLat);
	return 0;
}
