#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "camera.h"
#include "shape.h"
#include "material.h"
#include "stb_image_write.h"
#include "pcg32.h"
#include <fstream>
#include <memory>
#include <string>

Vec3 sampleUniformSphere(pcg32& rng) {
    Vec3 p;
    do {
        // crude rejection sampling
        p = 2.0f * Vec3((float)rng.nextDouble(), (float)rng.nextDouble(), (float)rng.nextDouble()) - Vec3(1.0f);
    } while (p.sqrLength() >= 1.0f);
    return p;
}

Vec3 color(const Ray& r, ShapeList& list, pcg32& rng, int bounce) {
    HitRecord hRec;
    if (list.intersect(r, 0.001f, FLT_MAX, hRec)) {
        Ray scattered;
        Vec3 attenuation;
        if (bounce < 50 && hRec.material->scatter(r, hRec, attenuation, scattered, rng)) {
            return attenuation * color(scattered, list, rng, ++bounce);
        } else { 
            return Vec3(0.f); 
        }
    }

    // shade the backgroud
    Vec3 unitDirVector = r.d.normalized();
    // get an interpolation paramter t between 0-1
    float t = 0.5f * (unitDirVector.y() + 1.0f);
    // interp between blue and white
    return (1.0f - t) * Vec3(1.0f) + t * Vec3(0.5f, 0.7f, 1.0f);
}

void writeToImageFile(const std::string& filename, std::shared_ptr<unsigned char> data, int nx, int ny) {
    int rc = stbi_write_png(filename.c_str(), nx, ny, 8, data.get(), sizeof(unsigned char) * nx * 3);
    if (rc == 0) {
        std::cout << "Error writing to image : " << filename << std::endl;
    }
    else {
        std::cout << "Successfully written to image :" << filename << std::endl;
    }
}

void initRandomScene(pcg32& rng, ShapeList& list, int nSpheres) {
    list.mObjects.resize(nSpheres);
    list.mObjects[0] = std::shared_ptr<Shape>(new Sphere(Vec3(0.f, -1000.f, 0.f), 1000.f, std::shared_ptr<Material>(new Lambertian(Vec3(0.5f)))));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float chooseMat = (float)rng.nextDouble();
            Vec3 center(a + 0.9f * (float)rng.nextDouble(), 0.2f, b + 0.9f * (float)rng.nextDouble());
            if ((center - Vec3(4.0f, 0.2f, 0.f)).length() > 0.9) {
                if (chooseMat < 0.8f) {
                    // diffuse spheres
                    list.mObjects[i++] = std::shared_ptr<Shape>(new Sphere(center,
                        0.2f, std::shared_ptr<Material>(
                            new Lambertian(Vec3((float)(rng.nextDouble() * rng.nextDouble()),
                                                (float)(rng.nextDouble() * rng.nextDouble()),
                                                (float)(rng.nextDouble() * rng.nextDouble())
                            )))));
                } else if (chooseMat < 0.95f) {
                    // metal
                    list.mObjects[i++] = std::shared_ptr<Shape>(new Sphere(center,
                        0.2f, std::shared_ptr<Material>(
                            new Metal(Vec3(
                                0.5f * (1.0f + (float)rng.nextDouble()),
                                0.5f * (1.0f + (float)rng.nextDouble()),
                                0.5f * (1.0f + (float)rng.nextDouble())
                            )))));
                } else {
                    // glass
                    list.mObjects[i++] = std::shared_ptr<Shape>(new Sphere(center,
                        0.2f, std::shared_ptr<Material>(
                            new Dielectric(1.5f))));
                }
            }
        }
    }
    list.mObjects[i++] = std::shared_ptr<Shape>(new Sphere(Vec3(0.f, 1.f, 0.f), 1.f, std::shared_ptr<Material>(new Dielectric(1.5f))));
    list.mObjects[i++] = std::shared_ptr<Shape>(new Sphere(Vec3(-4.f, 1.f, 0.f), 1.f, std::shared_ptr<Material>(new Lambertian(Vec3(0.4f, 0.2f, 0.1f)))));
    list.mObjects[i++] = std::shared_ptr<Shape>(new Sphere(Vec3(4.f, 1.f, 0.f), 1.f, std::shared_ptr<Material>(new Metal(Vec3(0.7f, 0.6f, 0.5f), 0.0f))));
}

int main(int argc, char** argv) {
    std::cout << "Raytracing in One Weekend\n";

    // Set the resolution of x and y
    int nx = 400;
    int ny = 200;
    int ns = 128;

    pcg32 rng;
    rng.seed(42u, 64u);

    // create an output file
    std::ofstream outfile("out.ppm");
    outfile << "P3\n" << nx << " " << ny << "\n255\n";      // header for the pfm file

    bool createRandomScene = true;

    // create a world
    ShapeList list;
    if (!createRandomScene) {
        list.mObjects.resize(4);
        list.mObjects[0] = std::shared_ptr<Shape>(new Sphere(Vec3(0.0f, 0.0f, -1.0f), 0.5f, std::shared_ptr<Material>(new Lambertian(Vec3(0.8f, 0.3f, 0.3f)))));
        list.mObjects[1] = std::shared_ptr<Shape>(new Sphere(Vec3(0.0f, -100.5f, -1.0f), 100.0f, std::shared_ptr<Material>(new Lambertian(Vec3(0.8f, 0.8f, 0.0f)))));
        list.mObjects[2] = std::shared_ptr<Shape>(new Sphere(Vec3(1.0f, 0.0f, -1.0f), 0.5f, std::shared_ptr<Material>(new Metal(Vec3(0.8f, 0.6f, 0.2f), 1.0f))));
        list.mObjects[3] = std::shared_ptr<Shape>(new Sphere(Vec3(-1.0f, 0.0f, -1.0f), 0.5f, std::shared_ptr<Material>(new Dielectric(1.5f))));
    }
    else {
        initRandomScene(rng, list, 500);
    }

    // Create a crude camera
    Vec3 eye(13.f, 2.f, 3.f);
    Vec3 lookat(0.f, 0.f, 0.f);
    float focalDistance = 10.f;
    float aperture = 0.0f;
    Camera camera(eye, lookat, Vec3(0.f, 1.f, 0.f), 20.f, float(nx)/float(ny), aperture,  0.9f * focalDistance);

    // perform the actual raytracing
    std::cout << "Tracing starting...\n";
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            Vec3 col(0.f);
            for (int s = 0; s < ns; s++) {
                float u = (float(i + rng.nextDouble()) / float(nx));
                float v = (float(j + rng.nextDouble()) / float(ny));
                Ray r = camera.generateRay(u, v, rng);
                col += color(r, list, rng, 0);
            }
            // gamma correction
            col /= float(ns);
            col = Vec3(sqrt(col.x()), sqrt(col.y()), sqrt(col.z()));
            int ir = int(255.99 * col.r());
            int ig = int(255.99 * col.g());
            int ib = int(255.99 * col.b());
            outfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    std::cout << "Tracing done\n";
    outfile.close();
    return 0;
}