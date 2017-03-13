#pragma once
#include <vector>
#include "Math/vec3.hpp"
#include "Math/Transform.hpp"

struct Face {
    int a, b, c, d;
};

struct Model final {
    std::vector<Vector3> vertices;
    std::vector<Face>    faces;
    
    static Model Sphere(int lats = 6, int lons = 8);
    static const Model& sphereInstance();
    
};
