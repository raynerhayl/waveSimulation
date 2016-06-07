#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "octree_node.hpp"

using namespace std;

/*Octree node wrapper*/
class Octree{
private:
	OctreeNode* root;
	vec3 m_origin;
	vec3 m_halfDimension;
public:
	Octree(const vec3& origin, const vec3& halfDimension) {
		m_origin = origin;
		m_halfDimension = halfDimension;
		root = new OctreeNode(m_origin,m_halfDimension);
	}

	~Octree(){
		delete root;
	}

	void insert(Boid * elem){
		root->insert(elem);
	}

	void clear(){
		delete root;
		root = new OctreeNode(m_origin,m_halfDimension);
	}

	/*Check and reshuffle all nodes*/
	void check(){
		vector<Boid *> shuffleList;
		//root->clean(shuffleList);
		for(int i = 0; i < shuffleList.size(); i++){
			if(root->inBounds(shuffleList[i]->mPosition)){
				root->insert(shuffleList[i]);
			}
		}
		//TODO make root down a size
		if(shuffleList.size() > 0) cout << "shuffleList.size = " << shuffleList.size() << endl;
	}

	void draw(){
		glColor3f(1,1,1);
		root->draw();
	}

	void getBoidsInsideCube(const cgra::vec3& min, const cgra::vec3& max, std::vector<Boid*>& results){
		root->getBoidsInArea(min,max,results);
	}
};
