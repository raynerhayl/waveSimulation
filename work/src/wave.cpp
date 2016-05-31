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

	createVertices();
	createTriangles();

	//make_plane(plane_width, plane_height, planeVert, planeInd);
}

Wave::~Wave() {}


void Wave::toggleWireFrame() {
	m_wireFrameOn = !m_wireFrameOn;
}

void Wave::createNormals() {

}

void Wave::createVertices() {
	double x_space = width / squareDensity;
	double z_space = height / squareDensity;

	for (int x = 0; x <= squareDensity; x++) {
	for (int z = 0; z <= squareDensity; z++) {
			m_points.push_back(vec3(x*x_space-width/2, 0, z*z_space-height/2));
		}
	}

	m_normals.push_back(vec3(0, 1, 0));

}

void Wave::createTriangles() {
	int numSquares = squareDensity * squareDensity;
	int off = 0;

	for (int square = 0; square < numSquares; square++) {

		if (square > 0 && (int)square % (int)squareDensity == 0) {
			off = off + 1;
		}

		triangle triOdd = triangle();

		triOdd.v[0].p = off + square;
		triOdd.v[0].n = 0;
		triOdd.v[1].p = off + square + 1;
		triOdd.v[1].n = 0;
		triOdd.v[2].p = off + square + squareDensity + 1;
		triOdd.v[2].n = 0;

		m_triangles.push_back(triOdd);
		triangle triEven = triangle();

		triEven.v[0].p = off + square + 1;
		triEven.v[0].n = 0;
		triEven.v[1].p = off + square + squareDensity + 1;
		triEven.v[1].n = 0;
		triEven.v[2].p = off + square + squareDensity + 2;
		triEven.v[2].n = 0;

		vec3 v = vec3(0, 0, 0);
		v = cgra::cross(v, v);

		m_triangles.push_back(triEven);

	}
}

void Wave::createDisplayListPoly() {
	// Delete old list if there is one
	if (m_displayListPoly) glDeleteLists(m_displayListPoly, 1);

	// Create a new list
	//cout << "Creating Poly Geometry" << endl;
	m_displayListPoly = glGenLists(1);
	glNewList(m_displayListPoly, GL_COMPILE);
	glBegin(GL_TRIANGLES);


	for (int tri = 0; tri < m_triangles.size(); tri++) {
		for (int vtri = 0; vtri < 3; vtri++) {
			glNormal3f(m_normals[m_triangles[tri].v[vtri].n].x, m_normals[m_triangles[tri].v[vtri].n].y, m_normals[m_triangles[tri].v[vtri].n].z);
			glVertex3f(m_points[m_triangles[tri].v[vtri].p].x, m_points[m_triangles[tri].v[vtri].p].y, m_points[m_triangles[tri].v[vtri].p].z);
		}
	}

	glEnd();
	glEndList();
}

void Wave::createDisplayListWire() {
	// Delete old list if there is one
	if (m_displayListWire) glDeleteLists(m_displayListWire, 1);

	// Create a new list
	//cout << "Creating Wire Geometry" << endl;
	m_displayListWire = glGenLists(1);
	glNewList(m_displayListWire, GL_COMPILE);

	glBegin(GL_TRIANGLES);

	for (int tri = 0; tri < m_triangles.size(); tri++) {
		for (int vtri = 0; vtri < 3; vtri++) {
			glNormal3f(m_normals[m_triangles[tri].v[vtri].n].x, m_normals[m_triangles[tri].v[vtri].n].y, m_normals[m_triangles[tri].v[vtri].n].z);
			glVertex3f(m_points[m_triangles[tri].v[vtri].p].x, m_points[m_triangles[tri].v[vtri].p].y, m_points[m_triangles[tri].v[vtri].p].z);
		}
	}

	glEnd();

	glEndList();
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
	//m_normals.clear();
	//createNormals();

	if (m_wireFrameOn) {
		createDisplayListWire();
		//-------------------------------------------------------------
		// [Assignment 1] :
		// When moving on to displaying your obj, comment out the
		// wire_cow function & uncomment the glCallList function
		//-------------------------------------------------------------

		glShadeModel(GL_SMOOTH);
		//wire_cow();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glCallList(m_displayListWire);

	}
	else {
		createDisplayListPoly();
		//-------------------------------------------------------------
		// [Assignment 1] :
		// When moving on to displaying your obj, comment out the
		// cow function & uncomment the glCallList function
		//-------------------------------------------------------------

		glShadeModel(GL_SMOOTH);
		//cow();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glCallList(m_displayListPoly);

	}
}



