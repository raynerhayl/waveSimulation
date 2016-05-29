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

using namespace std;
using namespace cgra;

School::School(int numBoids, BoundingBox bounds) {
	//bounding_box = bounds;
	for(int i = 0; i < numBoids; i++){

		float x = math::random(-1.0,1.0);
		float y = math::random(-1.0,1.0);
		float z = math::random(-1.0,1.0);
		//velocity = vec3(x,y,z);
		vec3 position = vec3(math::random(bounds.min.x,bounds.max.x),math::random(bounds.min.y,bounds.max.y),math::random(bounds.min.z,bounds.max.z));
		boids.push_back(Boid(position));
		cout << "creating boid "<< i << endl;
	}
}

void School::renderSchool() {
	float zRad = 15;
	applyForce(zRad*zRad, 0.5);
	update(); //update all
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if(draw_bounds)drawBounds();
	//Actually draw the School
	for(int i = 0; i < boids.size(); i++){
		boids[i].draw();
	}

	// Clean up
	glPopMatrix();
}

void School::applyForce(float zoneRadiusSqrd, float thresh){
	for( vector<Boid>::iterator p1 = boids.begin(); p1 != boids.end(); ++p1 ) {
		p1->pullToCentre(vec3(0,0,0));

	    vector<Boid>::iterator p2 = p1;
	    for( ++p2; p2 != boids.end(); ++p2 ) {
			vec3 dir = p1->mPosition - p2->mPosition;
			float distSqrd = lengthSquared(dir);

			if( distSqrd < zoneRadiusSqrd ) { // If the neighbor is within the zone radius...
				float percent = distSqrd/zoneRadiusSqrd;

				if( percent < thresh ) { // ... and is within the threshold limits, separate...
					float F = ( thresh/percent - 1.0f ) * 0.01f;
					dir = normalize(dir) * F;
					p1->mAccel += dir;
					p2->mAccel -= dir;
					cout << "mod accel1" << endl;
				}
				else { // ... else attract
					float threshDelta = 1.0f - thresh;
					float adjustedPercent = ( percent - thresh )/threshDelta;
					float F = ( 1.0 - ( cos( adjustedPercent * math::pi()*2.0f ) * -0.5f + 0.5f ) ) * 0.04f;
					dir = normalize(dir) * F;
					p1->mAccel -= dir;
					p2->mAccel += dir;
					cout << "mod accel2" << endl;
				}
			}else{
					cout << "no change" << endl;
				}
		}
	}
}

float School::lengthSquared(const vec3 &vec){
	return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
}

void School::update(){
	for (int i = 0; i < boids.size(); ++i){
		//cout << i << "-> ";
		boids[i].update();
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