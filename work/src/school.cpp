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

using namespace std;
using namespace cgra;

School::School(int numFish, vec3 bounds) {
	bounding_box = bounds;
}

void School::renderSchool() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if(draw_bounds)drawBounds();
	//Actually draw the School
	for(int i = 0; i < m_boids.size(); i++){
		
	}

	// Clean up
	glPopMatrix();
}


void School::drawBounds(){
	glPushMatrix();

	glPopMatrix();
}

void School::addCollider(primitives){

}


// void School::cube(vec3 pos, vec3 size){
// 	glPushMatrix();{
// 		glTranslatef();


// 	}glPopMatrix();
// }

// YOUR CODE GOES HERE
// ...