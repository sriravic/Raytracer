#ifndef __VEC3_H__
#define __VEC3_H__

#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

class Vec3
{
public:
    // Constructors
    Vec3() { v[0] = 0.f; v[1] = 0.f; v[2] = 0.f; }
    Vec3(float val) { v[0] = val; v[1] = val; v[2] = val; }
    Vec3(float x, float y, float z) { v[0] = x; v[1] = y; v[2] = z; }
    Vec3(const Vec3& V) { v[0] = V.v[0]; v[1] = V.v[1]; v[2] = V.v[2]; }

    // access operators
    float x() const { return v[0]; }
    float y() const { return v[1]; }
    float z() const { return v[2]; }
    float r() const { return v[0]; }
    float g() const { return v[1]; }
    float b() const { return v[2]; }
    float operator[] (int i) const { assert(i >= 0 && i <= 2); return v[i]; }

    // unary operators
    Vec3 operator- () const { return Vec3(-v[0], -v[1], -v[2]); }
    Vec3 operator+ (const Vec3& V) const { return Vec3(v[0] + V.v[0], v[1] + V.v[1], v[2] + V.v[2]); }
    Vec3 operator- (const Vec3& V) const { return Vec3(v[0] - V.v[0], v[1] - V.v[1], v[2] - V.v[2]); }
    Vec3 operator* (const float k) const { return Vec3(v[0] * k, v[1] * k, v[2] * k); }
    Vec3 operator* (const Vec3& V) const { return Vec3(v[0] * V.v[0], v[1] * V.v[1], v[2] * V.v[2]); }
    Vec3 operator/ (const float k) const { assert(k != 0.f); float invK = 1.f / k; return *this * invK; }
    friend Vec3 operator* (const float t, const Vec3& V) { return V * t; }

    // binary operators
    Vec3& operator+= (const Vec3& V) { v[0] += V.v[0]; v[1] += V.v[1]; v[2] += V.v[2]; return *this; }
    Vec3& operator-= (const Vec3& V) { v[0] -= V.v[0]; v[1] -= V.v[1]; v[2] -= V.v[2]; return *this; }
    Vec3& operator*= (const float k) { v[0] *= k; v[1] *= k; v[2] *= k; return *this; }
    Vec3& operator/= (const float k) { assert(k != 0.f); float invK = 1.0f / k; v[0] *= invK; v[1] *= invK; v[2] *= invK; return *this; }
    bool operator== (const Vec3& V) const { return v[0] == V.v[0] && v[1] == V.v[1] && v[2] == V.v[2]; }
    bool operator== (const float k) const { return v[0] == k && v[1] == k && v[2] == k; }

    // common operations
    float length() const { return std::sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }
    float sqrLength() const { return v[0] * v[0] + v[1] + v[1] + v[2] * v[2]; }
    float dot(const Vec3& V) const { return v[0] * V.v[0] + v[1] * V.v[1] + v[2] * V.v[2]; }
    Vec3 cross(const Vec3& V) const { return Vec3(v[1] * V.v[2] - v[2] * V.v[1], 
                                                  v[2] * V.v[0] - v[0] * V.v[2], 
                                                  v[0] * V.v[1] - v[1] * V.v[0]); }
    Vec3 normalized() const { return *this / this->length(); }
    Vec3& normalize() { *this /= length(); return *this; }

    // output operators
    friend std::ostream& operator >> (std::ostream& out, const Vec3& V) {
        out << "[" << V.v[0] << "," << V.v[1] << "," << V.v[2] << "]\n";
        return out;
    }

    float v[3];
};


#endif
