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
#include "octree_node.hpp"


class School {

public:
	//methods
	School(int, int, BoundingBox);
	void renderSchool();
	void addCollider(primitives);
	
private:
	//fields
	BoundingBox bounding_box;
	bool draw_bounds = true;

	OctreeNode* m_octree;
	std::vector<Prey> prey;
	std::vector<Predator> predators;

	// Methods
	void drawBounds();
	void update();
	void applyForce(float,float,float);

	void testSchool(BoundingBox);
};
