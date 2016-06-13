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
	}
}

void Boid::update(){
	mVelocity += mAccel;

	float velLengthSqrd = lengthSquared(mVelocity);
	float maxSpeedSqrd = getMaxSpeedSqrd();

	//limit velocity
	if( velLengthSqrd > maxSpeedSqrd ) {
		mVelocity = normalize(mVelocity);
		mVelocity *= mMaxSpeed;
	} else if( velLengthSqrd < mMinSpeed * mMinSpeed ) {
		mVelocity = normalize(mVelocity);
		mVelocity *= mMinSpeed;
	}

	mPosition += mVelocity;
	mAccel = vec3(0,0,0);
}

cgra::vec3 Boid::getPosition(){
	return mPosition;
}

Predator::Predator(vec3 loc) : Boid(loc){
	mMaxSpeed = 1.2;
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

void Predator::drawSelf() {
	glPushMatrix();{
		glColor3f(1,1,1);
		float len = 8;
		glScalef(2,2,2);
		m_geometry->renderGeometry();
	// Clean up
	}glPopMatrix();
}

float Predator::getMaxSpeedSqrd(){
	return mMaxSpeed * mMaxSpeed;
}

Prey::Prey(vec3 loc) : Boid(loc){

}

float Prey::getMaxSpeedSqrd(){
	float speed = min(mMaxSpeed + (((int)(mFear*100) % 10000)*.001), 1.2f);
	return speed * speed;
}

/**/
void Prey::drawSelf() {
	glPushMatrix();{
		glColor3f(m_colour.x,m_colour.y,m_colour.z);
		m_geometry->renderGeometry();
	// Clean up
	}glPopMatrix();
}