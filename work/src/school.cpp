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

School::School(int numPrey, int numPredators, BoundingBox bounds) {
	


	for(int i = 0; i < numPrey; i++){

		vec3 position = vec3(math::random(bounds.min.x,bounds.max.x),math::random(bounds.min.y,bounds.max.y),math::random(bounds.min.z,bounds.max.z));
		prey.push_back(Prey(position));
	}

	for(int i = 0; i < numPredators; i++){
		vec3 position = vec3(math::random(bounds.min.x,bounds.max.x),math::random(bounds.min.y,bounds.max.y),math::random(bounds.min.z,bounds.max.z));
		predators.push_back(Predator(position));
	}


	cout << "created "<< numPrey <<" prey " << endl;
}

void School::renderSchool() {
	float zRad = 50;
	applyForce(zRad*zRad, 0.4, 0.65);
	update(); //update all
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	if(draw_bounds)drawBounds();
	//Actually draw the School
	for(int i = 0; i < prey.size(); i++){
		prey[i].draw();
	}
	for(int i = 0; i < predators.size(); i++){
		predators[i].draw();
	}
	glRotatef(45,1,1,1);
	glColor3f(1,1,0);
	cgraLine(zRad);

	// Clean up
	glPopMatrix();
}

void School::applyForce(float zoneRadiusSqrd, float lowThresh, float highThresh){
	float jFactor = 0.1;
	for( vector<Prey>::iterator p1 = prey.begin(); p1 != prey.end(); ++p1 ) {

	    vector<Prey>::iterator p2 = p1;
	    for( ++p2; p2 != prey.end(); ++p2 ) {
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
			}
		}
		float eatDistSqrd = 10.0f;
		float predatorZoneRadiusSqrd = zoneRadiusSqrd * 3.0f;
		for( vector<Predator>::iterator predator = predators.begin(); predator != predators.end(); ++predator ) {
			vec3 dir = p1->mPosition - predator->mPosition;
			float distSqrd = lengthSquared(dir);
			if( distSqrd < predatorZoneRadiusSqrd ){
				if( distSqrd > eatDistSqrd ){
					float F = ( predatorZoneRadiusSqrd/distSqrd - 1.0f ) * 0.1f;
					p1->mFear += F * 0.1f;
					//cout << "fear add" <<F * 0.1f << endl;
					dir = normalize(dir) * F;
					p1->mAccel+= dir;
					predator->mAccel += dir;
				} else {
					p1->mIsDead = true;
					predator->mIsHungry = false;
					predator->mHunger -= p1->mMass;
				}
			} else { //TODO make more elegand, decrement
				p1->mFear = 0;
			}
		}
	}
}

/*Update the positions of the boids and remove dead prey*/
void School::update(){
	vector<Prey>::iterator pr = prey.begin();
	while(pr != prey.end()) {
		if (pr->mIsDead) {
			prey.erase(pr++);
			cout << "eaten!" << endl;
		} else {
			pr->pullToCentre(vec3(0,0,0));
			pr->update();
			++pr;
		}
	}

	for(vector<Predator>::iterator pred = predators.begin(); pred != predators.end(); ++pred) {
		
		pred->pullToCentre(vec3(0,0,0));
		pred->update();
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