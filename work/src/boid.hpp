//---------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


#include "cgra_math.hpp"
#include "opengl.hpp"
#include "helpers.hpp"


class Boid {
public:
	Boid(vec3);

	void update();
	void draw();
	vec3 mPosition;
	vec3 mVelocity;
	vec3 mAccel;
	vec3 getPosition();

	void pullToCentre(const vec3 &);
	
private:
	BoundingBox bounds;
	virtual void drawSelf() = 0;
	virtual float getMaxSpeedSqrd() = 0;
protected:
	float mMaxSpeed = 1;
	float mMinSpeed = 0.01;
};


class Predator : public Boid {
public:
	//methods
	Predator(vec3 pos);
	//fields
	bool mIsHungry = true;
	float mHunger = 10;
private:
	void drawSelf();
	float getMaxSpeedSqrd();
};

class Prey : public Boid {
public:
	//methods
	Prey(vec3 pos);
	//fields
	bool mIsDead = false;
	float mFear = 0;
	float mMass = 1;
private:
	void drawSelf();
	float getMaxSpeedSqrd();
};