#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#pragma once
#include "ray.h"
#include "shape.h"
#include "pcg32.h"

extern Vec3 sampleUniformSphere(pcg32& rng);

Vec3 reflect(const Vec3& n, const Vec3& v) {
    return v - 2 * n.dot(v) * n;
}

bool refract(const Vec3& v, const Vec3& n, float ni_over_nt, Vec3& refracted) {
    Vec3 uv = v.normalized();
    float dt = uv.dot(n);
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    } else {
        return false;
    }
}

float schlick(float cosine, float refIdx) {
    float r0 = (1.f - refIdx) / (1.f + refIdx);
    r0 = r0 * r0;
    return r0 + (1.f - r0) * std::pow((1.f - cosine), 5);
}

class Material
{
public:
    virtual bool scatter(const Ray& ray, const HitRecord& hitRecord, Vec3& attenuation, Ray& scattered, pcg32& rng) const = 0;
};

class Lambertian : public Material
{
public:
    Lambertian(const Vec3& a) : albedo(a) {}
    bool scatter(const Ray& ray, const HitRecord& hitRecord, Vec3& attenuation, Ray& scattered, pcg32& rng) const {
        Vec3 target = hitRecord.position + hitRecord.normal + sampleUniformSphere(rng);
        scattered = Ray(hitRecord.position, target - hitRecord.position);
        attenuation = albedo;
        return true;
    }
    Vec3 albedo;
};

class Metal : public Material
{
public:
    Metal(const Vec3& a, const float f = 0.f) : albedo(a) {
        if (f < 1.0f) fuzziness = f;
        else fuzziness = 1.0f;
    }
    bool scatter(const Ray& ray, const HitRecord& hitRecord, Vec3& attenuation, Ray& scattered, pcg32& rng) const {
        Vec3 reflected = reflect(hitRecord.normal, ray.d.normalized());
        scattered = Ray(hitRecord.position, reflected + fuzziness * sampleUniformSphere(rng));
        attenuation = albedo;
        return scattered.d.dot(hitRecord.normal) > 0.0f;
    }
    Vec3 albedo;
    float fuzziness;
};

class Dielectric : public Material
{
public:
    Dielectric(const float _eta) : eta(_eta) {}
    virtual bool scatter(const Ray& ray, const HitRecord& hitRecord, Vec3& attenuation, Ray& scattered, pcg32& rng) const {
        Vec3 outwardNormal;
        Vec3 reflected = reflect(hitRecord.normal, ray.d.normalized());
        float ni_over_nt;
        attenuation = Vec3(1.f);
        Vec3 refracted;
        float reflectionProb;
        float cosine;
        if (ray.d.dot(hitRecord.normal) > 0.f) {
            outwardNormal = -hitRecord.normal;
            ni_over_nt = eta;
            cosine = eta * ray.d.dot(hitRecord.normal) / ray.d.length();
        } else {
            outwardNormal = hitRecord.normal;
            ni_over_nt = 1.0f / eta;
            cosine = -ray.d.dot(hitRecord.normal) / ray.d.length();
        }
        if (refract(ray.d, outwardNormal, ni_over_nt, refracted)) {
            reflectionProb = schlick(cosine, eta);
        } else {
            scattered = Ray(hitRecord.position, reflected);
            reflectionProb = 1.0f;
        }

        if ((float)rng.nextDouble() < reflectionProb) {
            scattered = Ray(hitRecord.position, reflected);
        } else {
            scattered = Ray(hitRecord.position, refracted);
        }
        return true;
    }
    float eta;
};

#endif
