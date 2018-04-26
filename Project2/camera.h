#ifndef __CAMERA_H__
#define __CAMERA_H__

#pragma once
#include "ray.h"
#include "pcg32.h"

#define M_PI 3.142f

Vec3 sampleUnitDisk(pcg32& rng) {
    Vec3 p;
    do {
        p = 2.f * Vec3((float)rng.nextDouble(), (float)rng.nextDouble(), 0.f) - Vec3(1.f, 1.f, 0.f);
    } while (p.x() * p.x() + p.y() * p.y() >= 1.f);
    return p;
}


class Camera
{
public:
    Camera() {}
    Camera(const Vec3& o, const Vec3& llc, const Vec3& h, const Vec3& v)
        : origin(o)
        , lowerLeftCorner(llc)
        , horizontal(h)
        , vertical(v) {}
    Camera(const Vec3& eye, const Vec3& lookat, const Vec3& up, float fov, float aspectRatio,
        float aperture = 0.0f, float focusDistance = 1.0f) {
        lensRadius = aperture * 0.5f;
        float theta = fov * M_PI / 180.f;
        float halfHeight = std::tan(theta * 0.5f);
        float halfWidth = aspectRatio * halfHeight;

        // compose the orthogonal view system.
        origin = eye;
        w = (eye - lookat).normalized();
        u = up.cross(w).normalized();
        v = w.cross(u);

        // set up the coord system
        lowerLeftCorner = eye - halfWidth * focusDistance * u - halfHeight * focusDistance * v - focusDistance * w;
        horizontal = 2 * halfWidth * focusDistance * u;
        vertical = 2 * halfHeight * focusDistance * v;
    }

    Ray generateRay(float s, float t) const {
        return Ray(origin, lowerLeftCorner + s * horizontal + t * vertical - origin);
    }

    Ray generateRay(float s, float t, pcg32& rng) const {
        Vec3 rd = lensRadius * sampleUnitDisk(rng);
        Vec3 offset = u * rd.x() + v * rd.y();
        return Ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset);
    }

    Vec3 origin;
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;
    float lensRadius;
};

#endif
