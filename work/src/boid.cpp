//---------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <map>
#include <cctype>
#include <chrono>

#include "cgra_geometry.hpp"
#include "cgra_math.hpp"
#include "opengl.hpp"
#include "boid.hpp"

using namespace std;
using namespace cgra;

Boid::Boid(int numFish, vec3 bounds) {
	
}

/**/
void Boid::draw() {
	glPushMatrix();

	//Actually draw the Boid
	

	// Clean up
	glPopMatrix();
}

// void Boid::drawConstraints(bool ){

// }

// YOUR CODE GOES HERE
// ...