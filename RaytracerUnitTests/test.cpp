#include "pch.h"
#include "../Project2/vec3.h"
#include "../Project2/ray.h"

TEST(TestVectorOperations, TestUnaryOperations) {
    // We will test all the unary operations
    Vec3 a(1.0f), b(2.0f);
    EXPECT_EQ(a + b, Vec3(3.0f)) << "Failed Vec3 operator+ Test";
    EXPECT_EQ(a - b, Vec3(-1.0f)) << "Failed Vec3 operator- Test";
    EXPECT_EQ(a * 5.0f, Vec3(5.0f)) << "Failed Vec3 operator* Test";
    EXPECT_EQ(a / 5.0f, Vec3(0.2f)) << "Failed Vec3 opeator/ Test";
}

TEST(TestVec3Util, TestVec3UtilOperations) {
    Vec3 a(0.2f, 1.34f, 94.f);
    Vec3 ua = a.normalized();
    EXPECT_EQ(ua.length(), 1.0f) << "Failed normalized() vector test";
    EXPECT_EQ(a.normalize().length(), 1.0f) << "Failed normalize() vector test";
}

TEST(TestVec3Operations, TestVec3OperationsFunctions) {
    Vec3 x(1.0f, 0.0f, 0.0f), y(0.0f, 1.0f, 0.0f), z(0.0f, 0.0f, 1.0f);
    EXPECT_EQ(x.dot(x), 1.0f) << "Failed dot product test";
    EXPECT_EQ(x.dot(y), 0.0f) << "Failed dot product with orthogonal test";
    EXPECT_EQ(x.cross(y).dot(x), 0.0f) << "Failed cross product test";
    EXPECT_EQ(x.cross(z), Vec3(0.0f, -1.0f, 0.0f)) << "Failed cross product test";
}

TEST(TestRay, TestRayOperations) {
    Ray r(Vec3(0.0), Vec3(1.0f, 0.0f, 0.0f));
    EXPECT_EQ(r(5.0f), Vec3(5.0f, 0.0f, 0.0)) << "Ray () operator failed";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}