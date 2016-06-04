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
public:
	Octree(const vec3& origin, const vec3& halfDimension) {
		root = new OctreeNode(origin,halfDimension);
	}

	~Octree(){
		delete root;
	}

	void insert(Boid * elem){
		root->insert(elem);
	}

	void clear(){
		if(!root->isLeafNode())root->deleteChildren();
		root->data = NULL;
	}

	/*Check and reshuffle all nodes*/
	void check(){
		vector<Boid *> shuffleList;
		root->clean(shuffleList);
		//TODO make root down a size
		if(shuffleList.size() > 0) cout << "shuffleList.size = " << shuffleList.size() << endl;
	}

	void draw(){
		root->draw();
	}

	void getBoidsInsideCube(const cgra::vec3& min, const cgra::vec3& max, std::vector<Boid*>& results){
		root->getBoidsInsideCube(min,max,results);
	}
};
