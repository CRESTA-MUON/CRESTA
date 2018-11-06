//STL.cpp
//makes an stl file from a big array
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "Vector.hh"
#include "STLWriter.hh"

using namespace std;
using namespace TERRAIN;

struct TERRAIN::triangle {
	Vector a;
	Vector b;
	Vector c;
	Vector normal;
};


int voidCutoff = 0;
char endTag[2] = {0, 0};

//Determines the normal vector of a triangle from three vertices
Vector TERRAIN::normalOf(const Vector &p1, const Vector &p2, const Vector &p3) {
	Vector u(0, 0, 0);
	Vector v(0, 0, 0);
	Vector r(0, 0, 0);
	u.x = p2.x - p1.x;
	u.y = p2.y - p1.y;
	u.z = p2.z - p1.z;
	v.x = p3.x - p1.x;
	v.y = p3.y - p1.y;
	v.z = p3.z - p1.z;
	r.x = u.y * v.z - u.z * v.y;
	r.y = u.z * v.x - u.x * v.z;
	r.z = u.x * v.y - u.y * v.x;
	return r;
}

//Creates a triangle and its normal vector from three vertices
triangle TERRAIN::createTriangle(const Vector &j, const Vector &k, const Vector &l) {
	triangle t;
	t.a = j;
	t.b = k;
	t.c = l;
	t.normal = normalOf(j, k, l);
	return t;
}

//Writes a triangle into the STL file
void TERRAIN::addTriangle(triangle t) {
	//normal vector1
	cout.write((char *)&t.normal.x, sizeof(float));
	cout.write((char *)&t.normal.y, sizeof(float));
	cout.write((char *)&t.normal.z, sizeof(float));

	//vertices
	cout.write((char *)&t.a.x, sizeof(float));
	cout.write((char *)&t.a.y, sizeof(float));
	cout.write((char *)&t.a.z, sizeof(float));

	cout.write((char *)&t.b.x, sizeof(float));
	cout.write((char *)&t.b.y, sizeof(float));
	cout.write((char *)&t.b.z, sizeof(float));

	cout.write((char *)&t.c.x, sizeof(float));
	cout.write((char *)&t.c.y, sizeof(float));
	cout.write((char *)&t.c.z, sizeof(float));
	cout.write(endTag, 2);
}

//Takes a height array of variable length and turns it into an STL file
void TERRAIN::writeSTLfromArray(vector<float> &hList, int width, int height, float resolution) {
	uint32_t triangleCount = (width - 1) * (height - 1) * 2;	//number of facets in a void-free surface
	triangleCount += 4 * (width - 1);	//triangle counts for the walls of the model
	triangleCount += 4 * (height - 1);
	triangleCount += 2; 			//base triangles
	// xScale = cos(xScale);
	float xScale = resolution;
	float yScale = resolution;

	float verticaloffset = 0.0;
	float minvertical = *std::min_element(std::begin(hList), std::end(hList));
	verticaloffset = - minvertical + 10;

	// Anneal the map if voids are present
	for (int i = 1; i < hList.size(); i++){
		if (hList[i] <= voidCutoff){
			

			// Work outwards in X.


		}
	}


	if (cout.good()) {
		for (int i = 0; i < 80; i++) {
			cout.write("t", 1);
		}
		//write a placeholder number
		cout.write((char *)&triangleCount, 4);


		// Model Walls 1
		// -------------------------------------
		for (int x = 1; x < width; x++) {

			float xa =  x * xScale;
			float xb = (x - 1) * xScale;
			float xw =  x * xScale;
			float xz = (x - 1) * xScale;

			float ya = 0.0;
			float yb = 0.0;
			float yw = 0.0;
			float yz = 0.0;

			float za = hList.at(x) + verticaloffset;
			float zb = hList.at(x - 1) + verticaloffset;
			float zw = 0.0;
			float zz = 0.0;

			if (za > voidCutoff && zb > voidCutoff) {

				Vector a(xa, ya, za);
				Vector b(xb, yb, zb);
				Vector w(xw, yw, zw);
				Vector z(xz, yz, zz);

				addTriangle(createTriangle(b, z, a));
				addTriangle(createTriangle(w, a, z));

			} else {
				triangleCount -= 3;
			}

		}


		// Main Points
		// -------------------------------------
		for (int y = 1; y < height; y++) {
			for (int x = 1; x < width; x++) {

				float xa = x * xScale;
				float xb = x * xScale;
				float xc = (x - 1) * xScale;
				float xd = (x - 1) * xScale;

				float ya = (y) * yScale;
				float yb = (y - 1) * yScale;
				float yc = (y - 1) * yScale;
				float yd = (y) * yScale;

				float ha = hList.at(y * width + x) + verticaloffset;
				float hb = hList.at((y - 1) * width + x) + verticaloffset;     // d---a
				float hc = hList.at((y - 1) * width + x - 1) + verticaloffset; // |   |
				float hd = hList.at(y * width + x - 1) + verticaloffset;       // c---b

				if (ha > voidCutoff &&
				        hb > voidCutoff &&
				        hc > voidCutoff &&
				        hd > voidCutoff) {

					Vector a = Vector(xa, ya, ha);
					Vector b = Vector(xb, yb, hb);
					Vector c = Vector(xc, yc, hc);
					Vector d = Vector(xd, yd, hd);

					if (abs(hd - hb) < abs(ha - hc)) {
						addTriangle(createTriangle(a, d, b));
						addTriangle(createTriangle(c, b, d));
					} else {
						addTriangle(createTriangle(a, d, c));
						addTriangle(createTriangle(a, c, b));
					}

				} else {
					triangleCount -= 2;
				}
			}
		}

		// Model walls 2
		// -----------------------------
		for (int x = 1; x < width; x++) {

			float xa =  x * xScale;
			float xc = (x - 1) * xScale;
			float xw =  x * xScale;
			float xz = (x - 1) * xScale;

			float ya = (height - 1) * yScale;
			float yc = (height - 1) * yScale;
			float yw = (height - 1) * yScale;
			float yz = (height - 1) * yScale;

			float za = hList.at((height - 1) * width + x) + verticaloffset;
			float zc = hList.at((height - 1) * width + x - 1) + verticaloffset;
			float zw = 0.0;
			float zz = 0.0;

			if (za > voidCutoff && zc > voidCutoff) {

				Vector a(xa, ya, za);
				Vector c(xc, yc, zc);
				Vector w(xw, yw, zw);
				Vector z(xz, yz, zz);

				addTriangle(createTriangle(c, a, z));
				addTriangle(createTriangle(w, z, a));

			} else {
				triangleCount -= 3;
			}

		}

		// Add Walls Left(3)
		// -----------------------------
		for (int y = 1; y < width; y++) {

			float xa = 0.0;
			float xb = 0.0;
			float xw = 0.0;
			float xz = 0.0;

			float ya = (y) * yScale;
			float yb = (y - 1) * yScale;
			float yw = (y) * yScale;
			float yz = (y - 1) * yScale;

			float za = hList.at(y * width) + verticaloffset;
			float zb = hList.at((y - 1) * width) + verticaloffset;
			float zw = 0.0;
			float zz = 0.0;


			if (za > voidCutoff && zb > voidCutoff) {

				Vector a = Vector(xa, ya, za);
				Vector b = Vector(xb, yb, zb);
				Vector w = Vector(xw, yw, zw);
				Vector z = Vector(xz, yz, zz);

				addTriangle(createTriangle(z, b, a));
				addTriangle(createTriangle(a, w, z));

			} else {
				triangleCount -= 2;
			}
		}

		// Add Walls Right (4)
		// -----------------------------
		for (int y = 1; y < width; y++) {

			float xa = (width - 1) * xScale;
			float xb = (width - 1) * xScale;
			float xw = (width - 1) * xScale;
			float xz = (width - 1) * xScale;

			float ya = (y) * yScale;
			float yb = (y - 1) * yScale;
			float yw = (y) * yScale;
			float yz = (y - 1) * yScale;

			float za = hList.at(y * width + width - 1) + verticaloffset;
			float zb = hList.at(y * width - 1) + verticaloffset;
			float zw = 0.0;
			float zz = 0.0;

			if (za > voidCutoff && zb > voidCutoff) {

				Vector a = Vector(xa, ya, za);
				Vector b = Vector(xb, yb, zb);
				Vector w = Vector(xw, yw, zw);
				Vector z = Vector(xz, yz, zz);

				addTriangle(createTriangle(b, z, a));
				addTriangle(createTriangle(w, a, z));

			} else {
				triangleCount -= 2;
			}
		}

		// Add Bottom Surface
		// -----------------------------
		float xa = 0.0;
		float xb = (width - 1) * xScale;
		float xw = 0.0;
		float xz = (width - 1) * xScale;

		float ya = 0.0;
		float yb = 0.0;
		float yw = (height - 1) * yScale;
		float yz = (height - 1) * yScale;

		float za = 0.0;
		float zb = 0.0;
		float zw = 0.0;
		float zz = 0.0;

		Vector a = Vector(xa, ya, za);	
		Vector b = Vector(xb, yb, zb);		
		Vector w = Vector(xw, yw, zw);		
		Vector z = Vector(xz, yz, zz);		

		addTriangle(createTriangle(a, z, b));
		addTriangle(createTriangle(a, w, z));

		cout.seekp(80);
		cout.write((char *)&triangleCount, 4);
	}
	clog << "Triangle count: " << triangleCount << "\n";
}
