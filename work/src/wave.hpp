#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


#include "cgra_math.hpp"
#include "opengl.hpp"
#include "boid.hpp"


struct vertex {
	int p = 0; // index for point in m_points
	int t = 0; // index for uv in m_uvs
	int n = 0; // index for normal in m_normals
};

struct triangle {
	vertex v[3]; //requires 3 verticies
};

class Wave {
private:

	// Feilds for storing raw obj information
	std::string m_filename;
	std::vector<cgra::vec3> m_points;	// Point list
	std::vector<cgra::vec2> m_uvs;		// Texture Coordinate list
	std::vector<cgra::vec3> m_normals;	// Normal list
	std::vector<triangle> m_triangles;	// Triangle/Face list

	double squareDensity = 150;
	double width = 1000;
	double height = 1000;

	bool m_wireFrameOn = false;


	// IDs for the display list to render
	GLuint m_displayListPoly = 0; // DisplayList for Polygon
	GLuint m_displayListWire = 0; // DisplayList for Wireframe

	void createNormals();

	void createVertices();
	void createTriangles();

	void createDisplayListPoly();
	void createDisplayListWire();

public:
	Wave();
	~Wave();

	void render();
	void toggleWireFrame();

};
