#include "../../../Include/EngineExports.hpp"
#include "../../Headers/Game/EntityManager.hpp"
#include "../../Headers/Game/ComponentManager.hpp"
#include "../../../Game/AsteroidGenerator.h"
#include <iostream>

extern "C" {

void* CreateAsteroidGenerator() {
    try {
        return new TronEngine::AsteroidGenerator();
    }
    catch (...) {
        return nullptr;
    }
}

void DestroyAsteroidGenerator(void* generator) {
    if (generator) {
        delete static_cast<TronEngine::AsteroidGenerator*>(generator);
    }
}

uint32_t GenerateSingleAsteroid(void* generator, float radius, int complexity, float x, float y, float z, const char* shaderName) {
    if (!generator) return 0;
    
    try {
        auto* asteroidGen = static_cast<TronEngine::AsteroidGenerator*>(generator);
        
        // Generate the asteroid mesh data
        auto asteroidMesh = asteroidGen->GenerateAsteroid(radius, complexity);
        
        // Create an entity for this asteroid
        uint32_t entity = CreateEntity();
        if (entity == 0) return 0;
        
        // Add transform component at the specified position
        AddTransformComponent(entity, x, y, z);
        
        // For now, we'll use a sphere primitive as a placeholder
        // In the future, you could create a custom mesh from the asteroidMesh data
        AddMeshRendererComponent(entity, PRIMITIVE_SPHERE, shaderName ? shaderName : "RainbowShader");
        
        // Vary the scale based on radius
        SetTransformUniformScale(entity, radius / 2.0f);

        float x, y, z;
        GetTransformScale(entity, &x, &y, &z);
        AddBoxColliderComponent(entity, x, y, z, true);
        
        // Add some random rotation
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
        
        SetTransformRotationDegrees(entity, rotDist(gen), rotDist(gen), rotDist(gen));
        
        return entity;
    }
    catch (...) {
        return 0;
    }
}

void GenerateAsteroidField(void* generator, int count, float minRadius, float maxRadius, 
                          int minComplexity, int maxComplexity, float areaWidth, float areaHeight, float areaDepth, 
                          const char* shaderName) {
    if (!generator) return;
    
    try {
        auto* asteroidGen = static_cast<TronEngine::AsteroidGenerator*>(generator);
        
        // Generate positions for the asteroid field
        auto positions = asteroidGen->GenerateAsteroidPositions(count, areaWidth, areaHeight, areaDepth);
        
        // Generate asteroids with varying parameters
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);
        std::uniform_int_distribution<int> complexityDist(minComplexity, maxComplexity);
        
        for (int i = 0; i < count && i < static_cast<int>(positions.size()); i++) {
            float radius = radiusDist(gen);
            int complexity = complexityDist(gen);
            
            GenerateSingleAsteroid(generator, radius, complexity, 
                                 positions[i].x, positions[i].y, positions[i].z, 
                                 shaderName);
        }
        
        std::cout << "Generated " << count << " asteroids in field\n";
    }
    catch (...) {
        std::cout << "Error generating asteroid field\n";
    }
}

void TestAsteroidGenerator() {
    std::cout << "=== ASTEROID GENERATOR API TEST ===\n";
    
    // Create generator
    void* generator = CreateAsteroidGenerator();
    if (!generator) {
        std::cout << "✗ Failed to create asteroid generator\n";
        return;
    }
    std::cout << "✓ Asteroid generator created\n";
    
    // Test single asteroid generation
    uint32_t asteroid1 = GenerateSingleAsteroid(generator, 2.0f, 8, 0.0f, 0.0f, -10.0f, "RainbowShader");
    if (asteroid1 != 0) {
        std::cout << "✓ Single asteroid generated (Entity ID: " << asteroid1 << ")\n";
    } else {
        std::cout << "✗ Failed to generate single asteroid\n";
    }
    
    // Test asteroid field generation
    GenerateAsteroidField(generator, 5, 3.0f, 5.0f, 5, 50, 20.0f, 20.0f, 20.0f, "RainbowShader");
    std::cout << "✓ Asteroid field generation completed\n";
    
    // Cleanup
    DestroyAsteroidGenerator(generator);
    std::cout << "✓ Asteroid generator destroyed\n";
    std::cout << "=== ASTEROID GENERATOR API TEST COMPLETED ===\n\n";
}

} // extern "C"