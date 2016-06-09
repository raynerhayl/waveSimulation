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
#include "school.hpp"
#include "helpers.hpp"
#include "shady_geometry.hpp"

using namespace std;
using namespace cgra;

School::School(int numBoids, BoundingBox bounds) {
	


	for(int i = 0; i < numBoids; i++){

		vec3 position = vec3(math::random(bounds.min.x,bounds.max.x),math::random(bounds.min.y,bounds.max.y),math::random(bounds.min.z,bounds.max.z));
		boids.push_back(Boid(position));
	}
	cout << "created "<< numBoids <<" boids " << endl;
}

void School::renderSchool() {
	float zRad = 50;
	applyForce(zRad*zRad, 0.4, 0.65);
	update(); //update all
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	if(draw_bounds)drawBounds();
	//Actually draw the School
	for(int i = 0; i < boids.size(); i++){
		boids[i].draw();
	}
	glRotatef(45,1,1,1);
	glColor3f(1,1,0);
	cgraLine(zRad);

	// Clean up
	glPopMatrix();
}

void School::applyForce(float zoneRadiusSqrd, float lowThresh, float highThresh){
	float jFactor = 0.1;
	for( vector<Boid>::iterator p1 = boids.begin(); p1 != boids.end(); ++p1 ) {

	    vector<Boid>::iterator p2 = p1;
	    for( ++p2; p2 != boids.end(); ++p2 ) {
			vec3 dir = p1->mPosition - p2->mPosition;
			float distSqrd = lengthSquared(dir);

			if( distSqrd < zoneRadiusSqrd ) { // If the neighbor is within the zone radius...
				float percent = distSqrd/zoneRadiusSqrd;

				if( percent < lowThresh ) { // ... and is within the threshold limits, separate...
					float F = ( lowThresh/percent - 1.0f ) * 0.01f;
					dir = normalize(dir) * F * jFactor;
					p1->mAccel += dir;
					p2->mAccel -= dir;
					//cout << "mod accel1" << endl;
				}else if( percent < highThresh ) { // ... else if it is within the higher threshold limits, align...
					float threshDelta = highThresh - lowThresh;
					float adjustedPercent = ( percent - lowThresh )/threshDelta;
					// float F = ( 0.5f - cos( adjustedPercent * math::pi() * 2.0f ) * 0.5f + 0.5f ) * 0.01f;
					float F = ( 0.5f - cos( adjustedPercent * math::pi() * 2.0f ) * 0.5f + 0.5f ) * 0.01f;
					p1->mAccel += normalize(p2->mVelocity) * F * jFactor;
					p2->mAccel += normalize(p1->mVelocity) * F * jFactor;
				} else { // ... else attract
					float threshDelta = 1.0f - highThresh;
					float adjustedPercent = ( percent -highThresh )/threshDelta;
					//float F = ( 1.0 - ( cos( adjustedPercent * math::pi()*2.0f ) * -0.5f + 0.5f ) ) * 0.04f;
					float F = ( 1.0 - ( cos( adjustedPercent * math::pi()*2.0f ) * -0.5f + 0.5f ) ) * 0.01f;
					dir = normalize(dir) * F * jFactor;
					p1->mAccel -= dir;
					p2->mAccel += dir;
					//cout << "mod accel2" << endl;
				}
			}else{
					//cout << "no change" << endl;
				}
		}
	}
}

void School::update(){

	for(vector<Boid>::iterator b = boids.begin(); b != boids.end(); ++b) {
		
		b->pullToCentre(vec3(0,0,0));
		b->update();
	}
}

void School::drawBounds(){
	glPushMatrix();

	glPopMatrix();
}

void School::addCollider(primitives){

}

// YOUR CODE GOES HERE
// ...