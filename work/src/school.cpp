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
		//struct BoundingBox b(vec3(2,0,0),vec3(0,0,0));

		boids.push_back(Boid(bounds));
		cout << "creating boid "<< i << endl;
	}
}

void School::renderSchool() {
	
	tick();
	
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

void School::tick(){
	for (int i = 0; i < boids.size(); ++i){
		boids[i].tick(boids,i);
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