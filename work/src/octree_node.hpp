//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "cgra_math.hpp"
#include "boid.hpp"
#include "helpers.hpp"
#include "shady_geometry.hpp"

using namespace std;

class OctreeNode {

public:
	//fields
	vec3 origin;
	vec3 halfDimension;
	OctreeNode *children[8];
	Boid *data;

	//methods
	OctreeNode(const vec3& origin, const vec3& halfDimension) {
		this->origin = origin;
		this->halfDimension = halfDimension;
		data = NULL;
		for(int i = 0; i != 8; i++){
			children[i] = NULL;
		}
	}

	OctreeNode(const OctreeNode& copy) {
		//TODO
	}

	void deleteChildren(){
		for(int i=0; i !=8; i++){
			if(children[i] != NULL){
				delete children[i];
				children[i]	= NULL;
			} 
		}
	}

	~OctreeNode(){
		if(!isLeafNode()){
			deleteChildren();
		}
	}

	bool dataInBounds() {
		if (data == NULL){
			return true;
		} 
		vec3 p = data->getPosition();
		//cout << p << endl;
		return inBounds(p);
	}

	bool inBounds(const vec3& p){
		vec3 max = origin + halfDimension;
		vec3 min = origin - halfDimension;
		return !(p.x > max.x || p.x < min.x || p.y > max.y || p.y < min.y || p.z > max.z || p.z < min.z);
	}

	int clean(vector<Boid*>& toShuffle){
		int childDataCount = 0;
		if(isLeafNode()){
			if(data != NULL){
				if(dataInBounds()) return 1;
				else {
					toShuffle.push_back(data);
					data = NULL;
				}
			}
		} else {
			for (int i = 0; i != 8; ++i){
				int dataCount = children[i]->clean(toShuffle);
				if(dataCount == 0){
					children[i] -> deleteChildren();
				}
				childDataCount+= dataCount;
			}
			for(int i = toShuffle.size()-1; i != -1; i--){
				if(inBounds(toShuffle[i]->mPosition)){
					if(insert(toShuffle[i])){
						toShuffle.erase(toShuffle.begin()+i); //remove from list
						cout << "shsize: " << toShuffle.size() << endl;
						childDataCount++;
					} else {
						cout << "didnt fit, continued up" << endl;
					}//add to this
				}
			}
		}
		//cout << "#ch: " << childDataCount << " #orph: "<< toShuffle.size() << endl;
		return childDataCount;
	}

	bool insert(Boid* boid){
		cout << "inserting" << endl;
		if(inBounds(boid->getPosition())){
			if(isLeafNode()){
				cout << "Is leaf" << endl;
				if(data == NULL){
					cout << "Inserted into data" << endl;
					data = boid;
					return true;
				} else {
					//split and put boid and data in new octants
					for(int i = 0; i != 8; i++){
						float x = origin.x + ((i&1 ? 1 : -1) * halfDimension.x/2);
						float y = origin.y + ((i&2 ? 1 : -1) * halfDimension.y/2);
						float z = origin.z + ((i&4 ? 1 : -1) * halfDimension.z/2);
						children[i] = new OctreeNode(vec3(x,y,z),halfDimension/2.0);
					}
					//insert the new one
					int oct = getOctant(boid->getPosition());
					children[oct]->insert(boid);
					//insert the old one
					oct = getOctant(data->getPosition());
					return children[oct]->insert(data);
				}
			} else {
				//put it in the appropriate octant
				int oct = getOctant(boid->mPosition);
				return children[oct]->insert(boid);
			}
		}
		return false;
	}

	void getBoidsInsideCube(const cgra::vec3& min, const cgra::vec3& max, std::vector<Boid*>& results) {
		if(isLeafNode()){
			//cout << "getBoidsInCube -> leaf" << endl;
			if(data != NULL){
				vec3 pos = data->getPosition();
				if(pos.x>max.x || pos.y>max.y || pos.z>max.z) return;
				if(pos.x<min.x || pos.y<min.y || pos.z<min.z) return;
				results.push_back(data);
				//cout << "adding to results " << results.size() << endl;
			}
		} else {
			for(int i = 0; i != 8; i++){
				vec3 childMin = children[i]->origin - children[i]->halfDimension;
				vec3 childMax = children[i]->origin + children[i]->halfDimension;
				//dont bother if no overlap
				if(min.x > childMax.x || min.y > childMax.y || min.z > childMax.z) continue;
				if(max.x < childMin.x || max.y < childMin.y || max.z < childMin.z) continue;
				//if overlaps
				children[i]->getBoidsInsideCube(min,max,results);
			}
		}
	}

	bool isLeafNode() const {
		//will always split into 8
		return children[0] == NULL;
	}

	int getOctant(const vec3& point) const {
		int position = 0;
		// x 1, y 2, z 4
		if(point.x > origin.x)	position |= 1;
		if(point.y > origin.y)	position |= 2;
		if(point.z > origin.z)	position |= 4;
		return position;
	}

	void draw(){
		if(!isLeafNode()){
			for(int i=0; i !=8; i++){
				float red = (i&1 ? 1 : 0);
				float green = (i&2 ? 1 : 0);
				float blue = (i&4 ? 1 : 0);
				glColor3f(red,green,blue);
				cgraCube(children[i]->origin,children[i]->halfDimension*2);
				children[i]->draw();
			}
		} else if (data != NULL) {
			glPushMatrix();{
				glTranslatef(data->getPosition().x,data->getPosition().y,data->getPosition().z);
				//glColor3f(1,1,1);
				cgraSphere(2,3,3);
				//cout << "drawing data" << endl;
			}glPopMatrix();
		}
	}

};
