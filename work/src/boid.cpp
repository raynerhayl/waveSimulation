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
#include "shady_geometry.hpp"

using namespace std;
using namespace cgra;


Boid::Boid(vec3 pos) {
	//calculate once 
	mMaxSpeedSqrd = mMaxSpeed * mMaxSpeed;
	mMinSpeedSqrd = mMinSpeed * mMinSpeed;

	mPosition = pos;
	mVelocity = vec3(math::random(-0.01,0.01), math::random(-0.01,0.01), math::random(-0.01,0.01));
}

//TODO use dist^2
void Boid::pullToCentre(const vec3 &centre){
	vec3 dirToCenter = mPosition - centre;
	float distToCenter = length(dirToCenter);
	float maxDistance = 100.0f;
	if(distToCenter > maxDistance){
		float pullStrength = 0.0001f;
		//float pullStrength = 0.01f;
		mVelocity -= normalize(dirToCenter) * ( ( distToCenter - maxDistance ) * pullStrength );
		//mAccel -= normalize(dirToCenter) * ( ( distToCenter - maxDistance ) * pullStrength );
	}
}

void Boid::update(){
	//cout << "accel: " << mAccel << " vel: " << mVelocity << " pos: " << mPosition << endl;
	//limit velocity

	mVelocity += mAccel;

	float velLengthSqrd = lengthSquared(mVelocity);
	if( velLengthSqrd > mMaxSpeed*mMaxSpeed ) {
		mVelocity = normalize(mVelocity);
		mVelocity *= mMaxSpeed;
	} else if( velLengthSqrd < mMinSpeedSqrd ) {
		mVelocity = normalize(mVelocity);
		mVelocity *= mMinSpeed;
	}


	
	mPosition += mVelocity;
	mAccel = vec3(0,0,0);
}

Predator::Predator(vec3 loc) : Boid(loc){
	mMaxSpeed = 2;

}

void Boid::draw() {
	glPushMatrix();{
		glTranslatef(mPosition.x,mPosition.y,mPosition.z);
		float rotation = degrees(acos(dot(normalize(mVelocity), vec3(0,0,1))));
		vec3 orthog = cross(mVelocity, vec3(0,0,1));
		glRotatef(-rotation,orthog.x,orthog.y,orthog.z);
		drawSelf();
	// Clean up
	}glPopMatrix();
}
// Boid::~Boid(){

// }

void Predator::drawSelf() {
	glPushMatrix();{
		glColor3f(1,0.6,0);
		float len = 8;
		cgraCone(len/4, len, 4, 4, false);
		glTranslatef(0,0,4);
		glColor3f(1,0,0);
		cgraLine(3);
	// Clean up
	}glPopMatrix();
}
// Predator::~Predator(){

// }

Prey::Prey(vec3 loc) : Boid(loc){

}
// Prey::~Prey(){

// }

/**/
void Prey::drawSelf() {
	glPushMatrix();{
		glColor3f(1,1,1);
		float len = 2;
		cgraCone(0.5, len, 4, 4, false);
		glTranslatef(0,0,4);
		glColor3f(1,0,0);
		cgraLine(3);
	// Clean up
	}glPopMatrix();
}