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

	vec3 colours [5] = {vec3(52/255.0,86/255.0,156/255.0),vec3(250/255.0,129/255.0,19/255.0),vec3(27/255.0,143/255.0,89/255.0),vec3(115/255.0,4/255.0,166/255.0),vec3(250/255.0,155/255.0,25/255.0)};

	// Methods
	void drawBounds();
	void update();
	void applyForce(float,float,float);

	void testSchool(BoundingBox);
	void buildTree();
};
