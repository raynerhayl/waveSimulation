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
	School(int,cgra::vec3);
	void renderSchool();
	void addCollider(primitives);
	
private:
	//fields
	cgra::vec3 bounding_box;
	bool draw_bounds = true;

	std::vector<Boid> boids;

	// Methods
	void drawBounds();
	void tick();

};
