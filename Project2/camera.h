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
    Camera(const Vec3& eye, const Vec3& lookat, const Vec3& up, float fov, float aspectRatio) {
        float theta = fov * M_PI / 180.f;
        float halfHeight = std::tan(theta * 0.5f);
        float halfWidth = aspectRatio * halfHeight;

        // compose the orthogonal view system.
        origin = eye;
        Vec3 w = (eye - lookat).normalized();
        Vec3 u = up.cross(w).normalized();
        Vec3 v = w.cross(u);

        // set up the coord system
        lowerLeftCorner = eye - halfWidth * u - halfHeight * v - w;
        horizontal = 2 * halfWidth * u;
        vertical = 2 * halfHeight * v;
    }

    Ray generateRay(float u, float v) const {
        return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical - origin);
    }

    Vec3 origin;
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
};

#endif
