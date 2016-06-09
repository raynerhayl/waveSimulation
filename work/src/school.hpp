//---------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

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
#include "octree.hpp"
#include "geometry.hpp"

class School {

public:
	//methods
	School(int, int, BoundingBox);
	void renderSchool();
	
private:
	//fields
	BoundingBox bounding_box;
	bool draw_bounds = true;
	Geometry m_fishGeometry = Geometry("./work/res/assets/fish1.obj");
	Geometry m_sharkGeometry = Geometry("./work/res/assets/shark.obj");

	Octree* m_octree;
	std::vector<Prey> prey;
	std::vector<Predator> predators;

	// Methods
	void drawBounds();
	void update();
	void applyForce(float,float,float);

	void testSchool(BoundingBox);
	void buildTree();
};
