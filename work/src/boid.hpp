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


class Boid {
public:
	Boid(cgra::vec3);
	void draw();
	void tick(std::vector<Boid>);
	cgra::vec3 getPosition();
private:
	cgra::vec3 position;
	cgra::vec3 direction;
	float awareness_range = 20;
	float mass;
	float getRandom();
	cgra::vec3 bounds;

};
