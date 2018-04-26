#ifndef __CAMERA_H__
#define __CAMERA_H__

#pragma once
#include "ray.h"

#define M_PI 3.142f

class Camera
{
public:
    Camera() {}
    Camera(const Vec3& o, const Vec3& llc, const Vec3& h, const Vec3& v)
        : origin(o)
        , lowerLeftCorner(llc)
        , horizontal(h)
        , vertical(v) {}
    Camera(float fov, float aspectRatio) {
        float theta = fov * M_PI / 180.f;
        float halfHeight = std::tan(theta * 0.5f);
        float halfWidth = aspectRatio * halfHeight;
        lowerLeftCorner = Vec3(-halfWidth, -halfHeight, -1.0f);
        horizontal = Vec3(2.0f * halfWidth, 0.0f, 0.0f);
        vertical = Vec3(0.0f, 2 * halfHeight, 0.0f);
        origin = Vec3(0.0f);
    }

    Ray generateRay(float u, float v) const {
        return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical);
    }

    Vec3 origin;
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
};

#endif
