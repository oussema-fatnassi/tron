#include "AsteroidGenerator.h"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

namespace TronEngine {

AsteroidGenerator::AsteroidGenerator() {
    // Initialize random engine with a random seed
    std::random_device rd;
    m_Random.seed(rd());
}

AsteroidMesh AsteroidGenerator::GenerateAsteroid(float radius, int complexity) {
    AsteroidMesh mesh;
    
    // Base shape is a sphere with random perturbations
    const int segments = std::max(6, complexity); // At least 6 segments
    const int rings = std::max(4, complexity / 2); // At least 4 rings
    
    std::uniform_real_distribution<float> perturbDist(0.7f, 1.3f);
    
    // Generate vertices for a sphere with perturbations
    std::vector<Vec3> positions;
    
    // Generate vertices
    for (int r = 0; r <= rings; r++) {
        float phi = M_PI * static_cast<float>(r) / rings;
        
        for (int s = 0; s < segments; s++) {
            float theta = 2.0f * M_PI * static_cast<float>(s) / segments;
            
            // Perturbation to make it look rough
            float perturb = perturbDist(m_Random);
            
            // Convert spherical to Cartesian coordinates
            float x = radius * perturb * sin(phi) * cos(theta);
            float y = radius * perturb * sin(phi) * sin(theta);
            float z = radius * perturb * cos(phi);
            
            positions.push_back(Vec3(x, y, z));
        }
    }
    
    // Create vertices with positions
    mesh.vertices.resize(positions.size());
    for (size_t i = 0; i < positions.size(); i++) {
        mesh.vertices[i].position = positions[i];
    }
    
    // Generate indices for triangles
    for (int r = 0; r < rings; r++) {
        for (int s = 0; s < segments; s++) {
            int current = r * segments + s;
            int next = r * segments + (s + 1) % segments;
            int below = (r + 1) * segments + s;
            int belowNext = (r + 1) * segments + (s + 1) % segments;
            
            // Two triangles per quad
            mesh.indices.push_back(current);
            mesh.indices.push_back(below);
            mesh.indices.push_back(next);
            
            mesh.indices.push_back(next);
            mesh.indices.push_back(below);
            mesh.indices.push_back(belowNext);
        }
    }
    
    // Calculate rough normals (not perfect but sufficient for asteroids)
    // We're just using the vertex position as the normal direction for simplicity
    for (size_t i = 0; i < mesh.vertices.size(); i++) {
        mesh.vertices[i].normal = mesh.vertices[i].position.Normalized();
    }
    
    return mesh;
}

std::vector<AsteroidMesh> AsteroidGenerator::GenerateAsteroidField(
    int count, float minRadius, float maxRadius, 
    int minComplexity, int maxComplexity) 
{
    std::vector<AsteroidMesh> asteroids;
    asteroids.reserve(count);
    
    std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);
    std::uniform_int_distribution<int> complexityDist(minComplexity, maxComplexity);
    
    for (int i = 0; i < count; i++) {
        float radius = radiusDist(m_Random);
        int complexity = complexityDist(m_Random);
        
        asteroids.push_back(GenerateAsteroid(radius, complexity));
    }
    
    return asteroids;
}

std::vector<Vec3> AsteroidGenerator::GenerateAsteroidPositions(
    int count, float areaWidth, float areaHeight, float areaDepth) 
{
    std::vector<Vec3> positions;
    positions.reserve(count);
    
    std::uniform_real_distribution<float> xDist(-areaWidth/2, areaWidth/2);
    std::uniform_real_distribution<float> yDist(-areaHeight/2, areaHeight/2);
    std::uniform_real_distribution<float> zDist(-areaDepth/2, areaDepth/2);
    
    for (int i = 0; i < count; i++) {
        positions.push_back(Vec3(
            xDist(m_Random),
            yDist(m_Random),
            zDist(m_Random)
        ));
    }
    
    return positions;
}

} // namespace TronEngine