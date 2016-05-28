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


Boid::Boid(BoundingBox bbox) {
	bounds = bbox;
	float x = (getRandom()-0.5)*2;
	float y = (getRandom()-0.5)*2;
	float z = (getRandom()-0.5)*2;
	//cout << x << " " << y << " " << z << endl;
	velocity = vec3(x,y,z);
	position = vec3((getRandom()-0.5)*bounds.max.x,(getRandom()-0.5)*bounds.max.y,(getRandom()-0.5)*bounds.max.z);
	//cout << velocity << endl;
}

float Boid::getRandom(){
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

/**/
void Boid::draw() {
	glPushMatrix();{
		glTranslatef(position.x,position.y,position.z);

		float rotation = degrees(acos(dot(normalize(velocity), vec3(0,0,1))));
		vec3 orthog = cross(velocity, vec3(0,0,1));
		glRotatef(-rotation,orthog.x,orthog.y,orthog.z);
		//glRotatef();
		glColor3f(0,1,1);
		//cgraSphere(0.2 * 1.2);
		glColor3f(1,1,1);
		float len = 2;
		//cgraCylinder(0.2, 0.2*0.3, len, 10, 10, false);
		cgraCone(0.5, len, 4, 4, false);

	// Clean up
	}glPopMatrix();
}

void Boid::tick(vector<Boid> boids, int myIndex){
	//calculate centre of mass (TODO not including this one)
	vec3 centreOfMass;
	vec3 avoidancePos;
	vec3 velocityMatch;
	for(int i = 0; i < boids.size(); i++){
		if(myIndex != i){
			centreOfMass += boids[i].getPosition();
			//keep away from other boids
			if(length(boids[i].getPosition() - position) < avoidance_range){
				cout << "within range" << length(boids[i].getPosition() - position) << endl;
				avoidancePos += (position - boids[i].getPosition())/50;

			}
			velocityMatch += boids[i].getVelocity();
		}
	}
	centreOfMass = centreOfMass/(boids.size()-1);
	centreOfMass = (centreOfMass - position) / 100;

	velocityMatch = velocityMatch / (boids.size()-1);
	velocityMatch = (velocityMatch - velocity)/8;
	cout << centreOfMass << avoidancePos << velocityMatch << limitToBounds(position) << endl;
	
	//update direction
	vec3 calculatedVelocity = centreOfMass + (avoidancePos) + velocityMatch + limitToBounds(position);

	velocity = limitVelocity(velocity + calculatedVelocity);
	position += velocity;
}

vec3 Boid::limitVelocity(vec3 velVec){
	float limit = 0.5;
	vec3 limited = velVec;
	if(length(velVec) > limit){
		limited = (limited / length(limited)) * limit;
	}
	return limited;
}

vec3 Boid::limitToBounds(vec3 pos){
	//cout << bounds.xMax << " " << bounds.xMin << endl;
	vec3 vel;
	float amt = 10;
	if(pos.x > bounds.max.x){
		vel.x -= amt;
	} else if (pos.x < bounds.min.x){
		vel.x += amt;
	}
	if(pos.y > bounds.max.y){
		vel.y -= amt;
	} else if (pos.y < bounds.min.y){
		vel.y += amt;
	}
	if(pos.z > bounds.max.z){
		vel.z -= amt;
	} else if (pos.z < bounds.min.z){
		vel.z += amt;
	}
	return vel;
}

vec3 Boid::getVelocity(){
	return velocity;
}

vec3 Boid::getPosition(){
	return position;
}
// void Boid::drawConstraints(bool ){

// }

// YOUR CODE GOES HERE
// ...