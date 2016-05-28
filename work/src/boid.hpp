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
#include "helpers.hpp"


class Boid {
public:
	Boid(BoundingBox);
	void draw();
	void tick(std::vector<Boid>,int);
	cgra::vec3 getPosition();
	cgra::vec3 getVelocity();
	
private:
	cgra::vec3 position;
	cgra::vec3 velocity;
	float awareness_range = 10;
	float avoidance_range = 2;
	float mass;
	//float getRandom();
	BoundingBox bounds;

	cgra::vec3 limitVelocity(cgra::vec3);
	cgra::vec3 limitToBounds(cgra::vec3);

};
