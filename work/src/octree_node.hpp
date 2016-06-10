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
	//maximum number of points for a leaf
	static const int MAX_SIZE = 8;
private:
	Boid * data[MAX_SIZE];
	int m_dataSize = 0;
		//fields
	vec3 m_origin;
	vec3 m_halfDimension;
	OctreeNode *children[8];
public:


	//methods
	OctreeNode(const vec3& origin, const vec3& halfDimension) {
		this->m_origin = origin;
		this->m_halfDimension = halfDimension;
		//data = NULL;
		for(int i = 0; i != 8; i++){
			children[i] = NULL;
			data[i] = NULL;
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
		if (m_dataSize == 0){ //TODO should this return true?
			return true;
		}
		bool allIn = true;
		for(int i = 0; i != m_dataSize; i++){
			vec3 p = data[i]->getPosition();
			if(!inBounds(p)) allIn = false;
		}
		return allIn;
	}

	bool inBounds(const vec3& p){
		vec3 max = m_origin + m_halfDimension;
		vec3 min = m_origin - m_halfDimension;
		return !(p.x > max.x || p.x < min.x || p.y > max.y || p.y < min.y || p.z > max.z || p.z < min.z);
	}

	bool insert(Boid* boid){
		//cout << "inserting" << endl;
		if(inBounds(boid->getPosition())){
			if(isLeafNode()){
				//cout << "Is leaf" << endl;
				if(m_dataSize != MAX_SIZE){
					//cout << "Inserted into data" << endl;
					data[m_dataSize] = boid;
					m_dataSize++;
					return true;
				} else {
					//split and put boid and data in new octants
					for(int i = 0; i != 8; i++){
						float x = m_origin.x + ((i&1 ? 1 : -1) * m_halfDimension.x/2);
						float y = m_origin.y + ((i&2 ? 1 : -1) * m_halfDimension.y/2);
						float z = m_origin.z + ((i&4 ? 1 : -1) * m_halfDimension.z/2);
						children[i] = new OctreeNode(vec3(x,y,z),m_halfDimension/2.0);
					}
					//insert the new one
					int oct = getOctant(boid->getPosition());
					children[oct]->insert(boid);
					//insert the old ones
					for(int i = 0; i != m_dataSize; i++){
						oct = getOctant(data[i]->getPosition());
						children[oct]->insert(data[i]);
						data[i] = NULL;
					}
					m_dataSize = 0;
					return true;
				}
			} else {
				//put it in the appropriate octant
				int oct = getOctant(boid->mPosition);
				return children[oct]->insert(boid);
			}
		}
		return false;
	}

	void getBoidsInArea(const cgra::vec3& min, const cgra::vec3& max, std::vector<Boid*>& results) {
		if(isLeafNode()){
			if(m_dataSize != 0){
				for(int i = 0; i != m_dataSize; i++){
					vec3 pos = data[i]->getPosition();
					if(pos.x>max.x || pos.y>max.y || pos.z>max.z) continue;
					if(pos.x<min.x || pos.y<min.y || pos.z<min.z) continue;
					results.push_back(data[i]);
				}
			}
		} else {
			for(int i = 0; i != 8; i++){
				vec3 childMin = children[i]->m_origin - children[i]->m_halfDimension;
				vec3 childMax = children[i]->m_origin + children[i]->m_halfDimension;
				//dont bother if no overlap
				if(min.x > childMax.x || min.y > childMax.y || min.z > childMax.z) continue;
				if(max.x < childMin.x || max.y < childMin.y || max.z < childMin.z) continue;
				//if overlaps
				children[i]->getBoidsInArea(min,max,results);
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
		if(point.x > m_origin.x)	position |= 1;
		if(point.y > m_origin.y)	position |= 2;
		if(point.z > m_origin.z)	position |= 4;
		return position;
	}

	void draw(){
		if(!isLeafNode()){
			for(int i=0; i !=8; i++){
				float red = (i&1 ? 1 : 0);
				float green = (i&2 ? 1 : 0);
				float blue = (i&4 ? 1 : 0);
				glColor3f(red,green,blue);
				cgraCube(children[i]->m_origin,children[i]->m_halfDimension*2);
				children[i]->draw();
			}
		} else if (m_dataSize != 0) {
			for(int i = 0; i != m_dataSize; i++){
				glPushMatrix();{
					glTranslatef(data[i]->getPosition().x,data[i]->getPosition().y,data[i]->getPosition().z);
					cgraSphere(2,3,3);
				}glPopMatrix();
			}
		}
	}

};
