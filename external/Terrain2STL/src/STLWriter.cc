#include "STLWriter.hh"

// System Includes
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

// Terrain Includes
#include "Vector.hh"

// namespace std (BAD)
using namespace std;
// namespace TERRAIN
using namespace TERRAIN;

int voidCutoff = 0;
char endTag[2] = {0, 0};

struct TERRAIN::triangle {
	Vector a;
	Vector b;
	Vector c;
	Vector normal;
};

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

triangle TERRAIN::createTriangle(const Vector &j, const Vector &k, const Vector &l) {
	triangle t;
	t.a = j;
	t.b = k;
	t.c = l;
	t.normal = normalOf(j, k, l);
	return t;
}

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

void TERRAIN::writeSTLfromArray(const vector<float> &hList, int width, int height, float xScale) {

	uint32_t triangleCount = (width - 1) * (height - 1) * 2;	//number of facets in a void-free surface
	triangleCount += 4 * (width - 1);	//triangle counts for the walls of the model
	triangleCount += 4 * (height - 1);
	triangleCount += 2; 			//base triangles

	float yScale = xScale;

	if (cout.good()) {

		// Write some initial placeholder information
		for (int i = 0; i < 80; i++) {
			cout.write("t", 1);
		}
		cout.write((char *)&triangleCount, 4);

		// Make the model walls on one side along X direction
		for (int c = 1; c < width; c++) {
			if ((int)hList.at(c) > voidCutoff & (int)hList.at(c - 1) > voidCutoff & (int)hList.at(c + width - 1) > voidCutoff ) {
				Vector a(c * xScale, 0, hList.at(c));
				Vector b((c - 1)*xScale, 0, hList.at(c - 1));

				Vector w(c * xScale, 0, 0);				//used in model walls
				Vector z((c - 1)*xScale, 0, 0);

				addTriangle(createTriangle(b, z, a));			//model walls
				addTriangle(createTriangle(w, a, z));
			} else {
				triangleCount -= 3;
			}
		}

		// Make the actual triangles for each of the data points
		for (int y = 1; y < height; y++) {
			for (int x = 1; x < width; x++) {
				if ((int)hList.at(y * width + x) > voidCutoff & (int)hList.at(y * width + x - 1) > voidCutoff & (int)hList.at((y - 1)*width + x) > voidCutoff & (int)hList.at((y - 1)*width + x - 1) > voidCutoff) {
					float ha = hList.at(y * width + x);
					float hb = hList.at((y - 1) * width + x);   // d---a
					float hc = hList.at((y - 1) * width + x - 1); // |   |
					float hd = hList.at(y * width + x - 1);     // c---b

					Vector a = Vector(x * xScale, y, ha);
					Vector b = Vector(x * xScale, y - 1, hb);
					Vector c = Vector((x - 1) * xScale, y - 1, hc);
					Vector d = Vector((x - 1) * xScale, y, hd);

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

		// Make the model walls along the other side in X
		for (int x = 1; x < width; x++) {
			if ((int)hList.at((height - 1)*width + x) > voidCutoff & (int)hList.at((height - 2)*width + x) > voidCutoff & (int)hList.at((height - 1)*width + x - 1) > voidCutoff) {
				Vector a = Vector(x * xScale, height - 1, hList.at((height - 1) * width + x));		//same
				Vector c = Vector((x - 1) * xScale, height - 1, hList.at((height - 1) * width + x - 1));

				Vector w = Vector(x * xScale, height - 1, 0);		//used in model walls
				Vector z = Vector((x - 1) * xScale, height - 1, 0);

				addTriangle(createTriangle(c, a, z));			//model walls
				addTriangle(createTriangle(w, z, a));
			} else {
				triangleCount -= 3;
			}
		}

		// Make the model walls along the y sides
		Vector st = Vector(0, 0, 0);
		Vector sb = Vector(0, 0, 0);
		Vector bt = Vector(0, 0, 0);
		Vector bb = Vector(0, 0, 0);
		for (int y = 1; y < width; y++) {						//adds walls in the y direction for
			if ((int)hList.at(y * width) > voidCutoff & (int)hList.at((y - 1)*width) > voidCutoff) {
				st =  Vector(0, y, hList.at(y * width));			//for x=0 first
				sb =  Vector(0, y - 1, hList.at((y - 1) * width));
				bt =  Vector(0, y, 0);
				bb =  Vector(0, y - 1, 0);

				addTriangle(createTriangle(bb, sb, st));
				addTriangle(createTriangle(st, bt, bb));
			} else {
				triangleCount -= 2;
			}
			if ((int)hList.at(y * width + width - 1) > voidCutoff & (int)hList.at(y * width - 1) > voidCutoff) {
				st =  Vector((width - 1) * xScale, y, hList.at(y * width + width - 1));		//for x=width next
				sb = Vector((width - 1) * xScale, y - 1, hList.at(y * width - 1));
				bt = Vector((width - 1) * xScale, y, 0);
				bb = Vector((width - 1) * xScale, y - 1, 0);

				addTriangle(createTriangle(sb, bb, st));
				addTriangle(createTriangle(bt, st, bb));
			} else {
				triangleCount -= 2;
			}
		}

		// Create the bottom surface of the STL
		Vector origin = Vector(0, 0, 0);					//create bottom surface
		Vector bottomright = Vector((width - 1) * xScale, 0, 0);
		Vector topleft = Vector(0, height - 1, 0);
		Vector topright = Vector((width - 1) * xScale, height - 1, 0);
		addTriangle(createTriangle(origin, topright, bottomright));
		addTriangle(createTriangle(origin, topleft, topright));

		cout.seekp(80);
		cout.write((char *)&triangleCount, 4);
	}
	clog << "Triangle count: " << triangleCount << "\n";
}
