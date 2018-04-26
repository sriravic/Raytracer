#ifndef __RAY_H__
#define __RAY_H__

#pragma once
#include "vec3.h"

class Ray
{
public:
    Ray() {}
    Ray(const Vec3& origin, const Vec3& direction) : o(origin), d(direction) {}
    Vec3 operator() (const float t) const { return o + t * d; }
    Vec3 o;
    Vec3 d;
};

#endif
