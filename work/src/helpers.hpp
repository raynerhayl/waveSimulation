#pragma once

#include "cgra_math.hpp"

using namespace cgra;

struct BoundingBox {
	vec3 min = vec3();
	vec3 max;
	BoundingBox() {}
	BoundingBox(vec3 _min, vec3 _max) {
		min = _min;
		max = _max;
	}
};

inline BoundingBox operator* (float lhs, const BoundingBox& rhs)
{
    BoundingBox b = rhs;
    b.min *= lhs;
    b.max *= lhs;
    return b;
}

inline BoundingBox operator* (const BoundingBox& lhs, float rhs)
{
    BoundingBox b = lhs;
    b.min *= rhs;
    b.max *= rhs;
    return b;
}

/*Return the non-sqrt length of a vector*/
inline float lengthSquared(const vec3 &vec){
    return (vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z);
}