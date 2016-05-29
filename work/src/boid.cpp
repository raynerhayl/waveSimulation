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
	mPosition = pos;
	mVelocity = vec3(math::random(-0.1,0.1), math::random(-0.1,0.1), math::random(-0.1,0.1));
}


/**/
void Boid::draw() {
	glPushMatrix();{
		glTranslatef(mPosition.x,mPosition.y,mPosition.z);
		float rotation = degrees(acos(dot(normalize(mVelocity), vec3(0,0,1))));
		vec3 orthog = cross(mVelocity, vec3(0,0,1));
		glRotatef(-rotation,orthog.x,orthog.y,orthog.z);
		glColor3f(1,1,1);
		float len = 2;
		cgraCone(0.5, len, 4, 4, false);
		glTranslatef(0,0,4);
		glColor3f(1,0,0);
		cgraLine(3);
	// Clean up
	}glPopMatrix();
}

//TODO use dist^2
void Boid::pullToCentre(const vec3 &centre){
	vec3 dirToCenter = mPosition - centre;
	float distToCenter = length(dirToCenter);
	float maxDistance = 300.0f;
	if(distToCenter > maxDistance){
		//float pullStrength = 0.0001f;
		float pullStrength = 0.01f;
		mVelocity -= normalize(dirToCenter) * ( ( distToCenter - maxDistance ) * pullStrength );
	}
}

void Boid::update(){
	//cout << "accel: " << mAccel << " vel: " << mVelocity << " pos: " << mPosition << endl;
	mVelocity += mAccel;
	mPosition += mVelocity;
	mAccel = vec3(0,0,0);
}