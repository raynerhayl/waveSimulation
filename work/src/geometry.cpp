//---------------------------------------------------------------------------
//
// Copyright (c) 2016 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>

#include "cgra_math.hpp"
#include "geometry.hpp"
#include "opengl.hpp"

using namespace std;
using namespace cgra;


Geometry::Geometry(string filename) {
	m_filename = filename;
	readOBJ(filename);
	if (m_triangles.size() > 0) {
		createDisplayListPoly();
		createDisplayListWire();
	}
}


Geometry::~Geometry() { }


void Geometry::readOBJ(string filename) {

	// Make sure our geometry information is cleared
	m_points.clear();
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();

	// Load dummy points because OBJ indexing starts at 1 not 0
	m_points.push_back(vec3(0,0,0));
	m_uvs.push_back(vec2(0,0));
	m_normals.push_back(vec3(0,0,1));


	ifstream objFile(filename);

	if(!objFile.is_open()) {
		cerr << "Error reading " << filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file " << filename << endl;
	
	// good() means that failbit, badbit and eofbit are all not set
	while(objFile.good()) {

		// Pull out line from file
		string line;
		std::getline(objFile, line);
		istringstream objLine(line);

		// Pull out mode from line
		string mode;
		objLine >> mode;

		// Reading like this means whitespace at the start of the line is fine
		// attempting to read from an empty string/line will set the failbit
		if (!objLine.fail()) {

			if (mode == "v") {
				vec3 v;
				objLine >> v.x >> v.y >> v.z;
				m_points.push_back(v);

			} else if(mode == "vn") {
				vec3 vn;
				objLine >> vn.x >> vn.y >> vn.z;
				m_normals.push_back(vn);

			} else if(mode == "vt") {
				vec2 vt;
				objLine >> vt.x >> vt.y;
				m_uvs.push_back(vt);

			} else if(mode == "f") {

				vector<vertex> verts;
				while (objLine.good()){
					vertex v;

					//-------------------------------------------------------------
					// [Assignment 1] :
					// Modify the following to parse the bunny.obj. It has no uv
					// coordinates so each vertex for each face is in the format
					// v//vn instead of the usual v/vt/vn.
					//
					// Modify the following to parse the dragon.obj. It has no
					// normals or uv coordinates so the format for each vertex is
					// v instead of v/vt/vn or v//vn.
					//
					// Hint : Check if there is more than one uv or normal in
					// the uv or normal vector and then parse appropriately.
					//-------------------------------------------------------------

					// Assignment code (assumes you have all of v/vt/vn for each vertex)
					if (m_normals.size() <=1 && m_uvs.size() <= 1) {
						objLine >> v.p;		// Scan in normal index
						verts.push_back(v);
						objLine >> v.p;		// Scan in normal index
						verts.push_back(v);
						objLine >> v.p;		// Scan in normal index
						verts.push_back(v);
					}
					else {
						if (m_uvs.size() <= 1) {
							objLine >> v.p;		// Scan in position index
							objLine.ignore(2);	// Ignore the '/' character
							objLine >> v.n;		// Scan in normal index
						}
						else {
							objLine >> v.p;		// Scan in position index
							objLine.ignore(1);	// Ignore the '/' character
							objLine >> v.t;		// Scan in uv (texture coord) index
							objLine.ignore(1);	// Ignore the '/' character
							objLine >> v.n;		// Scan in normal index
						}
						verts.push_back(v);
					}
				}

				// IFF we have 3 verticies, construct a triangle
				if(verts.size() >= 3){
					triangle tri;
					tri.v[0] = verts[0];
					tri.v[1] = verts[1];
					tri.v[2] = verts[2];
					m_triangles.push_back(tri);

				}
			}
		}
	}

	cout << "Reading OBJ file is DONE." << endl;
	cout << m_points.size()-1 << " points" << endl;
	cout << m_uvs.size()-1 << " uv coords" << endl;
	cout << m_normals.size()-1 << " normals" << endl;
	cout << m_triangles.size() << " faces" << endl;


	// If we didn't have any normals, create them
	if (m_normals.size() <= 1) createNormals();
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to populate the normals for 
// the model currently loaded. Compute per face normals
// first and get that working before moving onto calculating
// per vertex normals.
//-------------------------------------------------------------
void Geometry::createNormals() {

	
	//Calculates face normals
	for (int i = 0; i < m_triangles.size(); i++) {
		vec3 u = m_points[m_triangles[i].v[1].p] - m_points[m_triangles[i].v[0].p]; //edge 1
		vec3 v = m_points[m_triangles[i].v[2].p] - m_points[m_triangles[i].v[0].p]; //edge 2
		vec3 normal = normalize(cross(u, v)); //cross product of two edges, normalized
		m_normals.push_back(normal); //push onto list
		m_triangles[i].v[0].n = m_normals.size()- 1; //add index of surface normal to each vertex struct
		m_triangles[i].v[1].n = m_normals.size() - 1;
		m_triangles[i].v[2].n = m_normals.size() - 1;
	}

	/*As the vector m_normals now contains surface normals in the same order as the triangle list, 
	we can now iterate through the vector of points and add the surface normal of any triangle containing that point
	NOTE: This was a real dumb way of doing it and it took 30 minutes. */
	/*for (int i = 1; i < m_points.size(); i++) {
		vec3 vertnormal = vec3(0,0,0);
		for (int j = 0; j < m_triangles.size(); j++) { //iterate through all triangles
			for (int k = 0; k < 3; k++) {
				if (m_triangles[j].v[k].p == i) { //if point is in triangle
					vertnormal += m_normals[j+1]; //add surface normal to vert normal
					m_triangles[j].v[k].n = m_normals.size(); //index of normal to be added to list
				}
			}
		}
		vertnormal = normalize(vertnormal);
		m_normals.push_back(vertnormal);
	}*/
	
	vector<vec3> m_vertnormals (m_points.size(), vec3(0, 0, 0));
	for (int i = 0; i < m_triangles.size(); i++) {
		//add surface normal to each point in m_vertnormals
		for (int j = 0; j < 3; j++) {
			m_vertnormals[m_triangles[i].v[j].p] += m_normals[i + 1];
			m_triangles[i].v[j].n = m_triangles[i].v[j].p;
		}
	}
	for (int i = 1; i < m_vertnormals.size(); i++) {
		m_vertnormals[i] = normalize(m_vertnormals[i]);
	}
	m_normals = m_vertnormals;
	m_vertnormals.clear();
	
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to create display list
// of the obj file to show it as wireframe model
//-------------------------------------------------------------
void Geometry::createDisplayListPoly() {
	// Delete old list if there is one
	if (m_displayListPoly) glDeleteLists(m_displayListPoly, 1);

	// Create a new list
	cout << "Creating Poly Geometry" << endl;
	m_displayListPoly = glGenLists(1);
	glNewList(m_displayListPoly, GL_COMPILE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// YOUR CODE GOES HERE
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < m_triangles.size(); i++) {
		triangle tri = m_triangles[i];
		vec3 v1 = m_points[tri.v[0].p];
		vec3 n1 = m_normals[tri.v[0].n];
		vec3 v2 = m_points[tri.v[1].p];
		vec3 n2 = m_normals[tri.v[1].n];
		vec3 v3 = m_points[tri.v[2].p];
		vec3 n3 = m_normals[tri.v[2].n];
		vec2 t1 = m_uvs[tri.v[0].t];
		vec2 t2 = m_uvs[tri.v[1].t];
		vec2 t3 = m_uvs[tri.v[2].t];
		t1 *= 8;
		t2 *= 8;
		t3 *= 8;
		glNormal3f(n1.x, n1.y, n1.z);
		glTexCoord2f(t1.x,t1.y);
		glVertex3f(v1.x, v1.y, v1.z);
		glNormal3f(n2.x, n2.y, n2.z);
		glTexCoord2f(t2.x,t2.y);
		glVertex3f(v2.x, v2.y, v2.z);
		glNormal3f(n3.x, n3.y, n3.z);
		glTexCoord2f(t3.x,t3.y);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();
	glEndList();
	cout << "Finished creating Poly Geometry" << endl;
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to create display list
// of the obj file to show it as polygon model
//-------------------------------------------------------------
void Geometry::createDisplayListWire() {
	// Delete old list if there is one
	if (m_displayListWire) glDeleteLists(m_displayListWire, 1);

	// Create a new list
	cout << "Creating Wire Geometry" << endl;
	m_displayListWire = glGenLists(1);
	glNewList(m_displayListWire, GL_COMPILE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// YOUR CODE GOES HERE
	// ...
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < m_triangles.size(); i++) {
		triangle tri = m_triangles[i];
		vec3 v1 = m_points[tri.v[0].p];
		vec3 n1 = m_normals[tri.v[0].n];
		vec3 v2 = m_points[tri.v[1].p];
		vec3 n2 = m_normals[tri.v[1].n];
		vec3 v3 = m_points[tri.v[2].p];
		vec3 n3 = m_normals[tri.v[2].n];
		glNormal3f(n1.x, n1.y, n1.z);
		glVertex3f(v1.x, v1.y, v1.z);
		glNormal3f(n2.x, n2.y, n2.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glNormal3f(n3.x, n3.y, n3.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();
	glEndList();
	cout << "Finished creating Wire Geometry" << endl;
}


void Geometry::renderGeometry() {
		glShadeModel(GL_SMOOTH);
		 glCallList(m_displayListPoly);
}


void Geometry::toggleWireFrame() {
	m_wireFrameOn = !m_wireFrameOn;
}