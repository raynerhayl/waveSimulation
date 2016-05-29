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
	Boid(vec3);
	void draw();
	void update();
	vec3 mPosition;
	vec3 mVelocity;
	vec3 mAccel;

	float mRadius = 5.0f;

	void pullToCentre(const vec3 &);
	
private:
	BoundingBox bounds;
};
