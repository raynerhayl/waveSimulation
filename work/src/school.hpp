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


enum primitives {
	cube = 0,
	cylinder = 1
};


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

	std::vector<Prey> prey;
	std::vector<Predator> predators;

	// Methods
	void drawBounds();
	void update();
	void applyForce(float,float,float);


};
