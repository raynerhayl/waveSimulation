#pragma once

#include "cgra_math.hpp"
#include "shady_geometry.hpp"

#include <cstdlib>

using namespace cgra;
using namespace std;

struct BoundingBox {
	vec3 min = vec3();
	vec3 max;
	BoundingBox() {}
	BoundingBox(vec3 _min, vec3 _max) {
		min = _min;
		max = _max;
	}
    bool inside(const vec3 p){
        return !(p.x > max.x || p.x < min.x || p.y > max.y || p.y < min.y || p.z > max.z || p.z < min.z);
    }
    void draw(){
        vec3 centre = (min + max)/2.0;
        vec3 bounds = abs(max-min);
        cgraCube(centre,bounds);
    }
    vec3 origin(){
        return (min + max)/2.0;
    }
};

inline BoundingBox operator* (float lhs, const BoundingBox& rhs)
{
    BoundingBox b = rhs;
    vec3 b_origin = b.origin();
    //translate to origin
    b.min -= b_origin;
    b.max -= b_origin;
    //scale
    b.min *= lhs;
    b.max *= lhs;
    //return to old position
    b.min += b_origin;
    b.max += b_origin;
    return b;
}

inline BoundingBox operator* (const BoundingBox& lhs, float rhs)
{
    BoundingBox b = lhs;
    vec3 b_origin = b.origin();
    //translate to origin
    b.min -= b_origin;
    b.max -= b_origin;
    //scale
    b.min *= rhs;
    b.max *= rhs;
    //return to old position
    b.min += b_origin;
    b.max += b_origin;
    return b;
}

/*Return the non-sqrt length of a vector*/
inline float lengthSquared(const vec3 &vec){
    return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
}


inline float min(float a, float b){
    return (a > b)? b : a;
}

inline float max(float a, float b){
    return (a < b)? b : a;
}
