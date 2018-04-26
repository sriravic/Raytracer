#ifndef __SHAPE_H__
#define __SHAPE_H__

#pragma once
#include "vec3.h"
#include "ray.h"
#include <vector>
#include <memory>

class Material;

struct HitRecord
{
    float t;
    Vec3 position;
    Vec3 normal;
    std::shared_ptr<Material> material;
};

// Abstract base class for all intersectable shapes
class Shape
{
public:
    virtual bool intersect(const Ray& r, const float minT, const float maxT, HitRecord& record) const = 0;
};

class Sphere : public Shape
{
public:
    Sphere() : radius(0.f) {}
    Sphere(const Vec3& c, float r) : center(c), radius(r) {}
    Sphere(const Vec3& c, float r, std::shared_ptr<Material> mat) : center(c), radius(r), material(mat) {}
    bool intersect(const Ray& ray, const float minT, const float maxT, HitRecord& record) const {
        Vec3 oc = ray.o - center;
        float a = ray.d.dot(ray.d);
        float b = 2.0f * ray.d.dot(oc);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0.0f) return false;
        else {
            float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
            if (t1 >= 0.0f && t1 >= minT && t1 <= maxT) { record.t = t1; }
            else {
                float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);
                if(t2 >= 0.0f && t2 >= minT && t2 <= maxT)
                    record.t = t2;
                else return false;
            }
            record.position = ray(record.t);
            record.normal = (record.position - center).normalized();
            record.material = material;
            return true;
        }
    }
    Vec3 center;
    float radius;
    std::shared_ptr<Material> material;
};

class ShapeList : public Shape
{
public:
    ShapeList() {}
    ShapeList(const std::vector<std::shared_ptr<Shape>>& objects) {
        mObjects = objects;
    }
    bool intersect(const Ray& r, const float minT, const float maxT, HitRecord& record) const {
        HitRecord tempRec;
        float minDistance = maxT;
        bool hitAnything = false;
        for(auto& o : mObjects)
            if (o->intersect(r, minT, minDistance, tempRec)) {
                minDistance = tempRec.t;
                hitAnything = true;
                record = tempRec;
            }
        return hitAnything;
    }
    std::vector<std::shared_ptr<Shape>> mObjects;
};

#endif
