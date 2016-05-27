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
	direction = normalize(vec3(x,y,z))*0.05;
	position = vec3(2,2,2);
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
		//cgraSphere(0.2 * 1.2);
		glColor3f(1,1,1);
		float len = 1;
		//cgraCylinder(0.2, 0.2*0.3, len, 10, 10, false);
		cgraCone(0.2, len, 4, 4, false);

	// Clean up
	}glPopMatrix();
}

void Boid::tick(vector<Boid> boids){
	//calculate centre of mass (TODO not including this one)
	vec3 centreOfMass;
	for(int i = 0; i < boids.size(); i++){
		// if(boids[i] != this){
		centreOfMass += boids[i].getPosition();
	}
	centreOfMass = centreOfMass/(boids.size()-0);
	centreOfMass = (centreOfMass - position) / 100;

	//keep away from other boids

	direction = normalize(centreOfMass + direction)/5;

	position += direction;
}

vec3 Boid::getPosition(){
	return position;
}
// void Boid::drawConstraints(bool ){

// }

// YOUR CODE GOES HERE
// ...