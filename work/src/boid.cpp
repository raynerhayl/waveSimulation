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


Boid::Boid(vec3 bounds) {
	bounds = bounds;
	float x = (getRandom()-0.5)*2;
	float y = (getRandom()-0.5)*2;
	float z = (getRandom()-0.5)*2;
	cout << x << " " << y << " " << z << endl;
	direction = normalize(vec3(x,y,z))*0.2;
	cout << direction << endl;
}

float Boid::getRandom(){
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

/**/
void Boid::draw() {
	glPushMatrix();{
		glTranslatef(position.x,position.y,position.z);

		float rotation = degrees(acos(dot(normalize(direction), vec3(0,0,1))));
		vec3 orthog = cross(direction, vec3(0,0,1));
		glRotatef(-rotation,orthog.x,orthog.y,orthog.z);
		//glRotatef();
		glColor3f(0,1,1);
		cgraSphere(0.2 * 1.2);
		glColor3f(1,1,1);
		float len = 4;
		cgraCylinder(0.2, 0.2*0.3, len, 10, 10, false);

	// Clean up
	}glPopMatrix();
}

void Boid::tick(){
	position += direction;
}

// void Boid::drawConstraints(bool ){

// }

// YOUR CODE GOES HERE
// ...