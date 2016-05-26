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
	void tick();

private:
	cgra::vec3 position;
	cgra::vec3 direction;
	float getRandom();
	cgra::vec3 bounds;

};
