#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <map>
#include <cctype>
#include <chrono>

#include "cgra_geometry.hpp"
#include "cgra_math.hpp"
#include "opengl.hpp"
#include "school.hpp"
#include "wave.hpp"

using namespace cgra;
using namespace std;

//const int plane_width = 4; // amount of columns
//const int plane_height = 2; // amount of rows
//
//const int total_vertices = (plane_width + 1) * (plane_height + 1);
//vec2 planeVert[total_vertices];
//
//const int numIndPerRow = plane_width * 2 + 2;
//const int numIndDegensReq = (plane_height - 1) * 2;
//const int total_indices = numIndPerRow * plane_height + numIndDegensReq;
//
//int planeInd[total_indices];


Wave::Wave() {
	//memset(planeVert, 0, sizeof(vec2) * total_vertices);
	

	//make_plane(plane_width, plane_height, planeVert, planeInd);
}

Wave::~Wave() {}


void Wave::toggleWireFrame() {
	m_wireFrameOn = !m_wireFrameOn;
}

void Wave::createNormals() {

}

void Wave::createDisplayListPoly() {

}

void Wave::createDisplayListWire() {

}

//void Wave::make_plane(int width, int height, vec2 * vertices, int * indices)
//{
//	width++;
//	height++;
//
//	int size = sizeof(cgra::vec2);
//	// Set up vertices
//	for (int y = 0; y < height; y++)
//	{
//		int base = y * width;
//		for (int x = 0; x < width; x++)
//		{
//			int index = base + x;
//			vec2 *v = vertices + index;
//			v->x = (float)x;
//			v->y = (float)y;
//			cout << index << " " << v->x << " " << v->y << endl;
//
//		}
//	}
//
//	cout << "-------------------------" << endl;
//
//	// Set up indices
//	int i = 0;
//	height--;
//	for (int y = 0; y < height; y++)
//	{
//		int base = y * width;
//
//		//indices[i++] = (uint16)base;
//		for (int x = 0; x < width; x++)
//		{
//			indices[i++] = (int)(base + x);
//			indices[i++] = (int)(base + width + x);
//		}
//		// add a degenerate triangle (except in a last row)
//		if (y < height - 1)
//		{
//			indices[i++] = (int)((y + 1) * width + (width - 1));
//			indices[i++] = (int)((y + 1) * width);
//		}
//	}
//
//	for (int ind = 0; ind < i; ind++)
//		cout << indices[ind] << endl;
//
//}

void Wave::render() {

}



