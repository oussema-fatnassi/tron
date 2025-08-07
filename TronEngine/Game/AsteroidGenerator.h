#pragma once

#include <vector>
#include <random>
#include <string>
#include <cmath>

#ifdef ENGINE_EXPORTS
#define ASTEROID_API __declspec(dllexport)
#else
#define ASTEROID_API __declspec(dllimport)
#endif

namespace TronEngine {

// Simple 3D vector class to avoid dependencies
struct ASTEROID_API Vec3 {
    float x, y, z;
    
    Vec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
    
    float Length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    Vec3 Normalized() const {
        float len = Length();
        if (len < 0.0001f) return Vec3(0, 0, 0);
        return Vec3(x / len, y / len, z / len);
    }
};

// Vertex data for mesh
struct ASTEROID_API AsteroidVertex {
    Vec3 position;
    Vec3 normal;
};

// Simple mesh structure
struct ASTEROID_API AsteroidMesh {
    std::vector<AsteroidVertex> vertices;
    std::vector<uint32_t> indices;
};

class ASTEROID_API AsteroidGenerator {
public:
    AsteroidGenerator();
    
    // Generate a single asteroid mesh with given parameters
    AsteroidMesh GenerateAsteroid(float radius, int complexity);
    
    // Generate multiple asteroids with different parameters
    std::vector<AsteroidMesh> GenerateAsteroidField(
        int count, 
        float minRadius, float maxRadius,
        int minComplexity, int maxComplexity
    );
    
    // Generate positions for an asteroid field within a region
    std::vector<Vec3> GenerateAsteroidPositions(
        int count,
        float areaWidth, float areaHeight, float areaDepth
    );

private:
    std::mt19937 m_Random;
};

} // namespace TronEngine