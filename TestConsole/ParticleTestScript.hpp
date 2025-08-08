#pragma once
#include "TronEngine.hpp"
#include <iostream>
#include <vector>
#include <random>

struct SimpleParticle {
    uint32_t entityId;
    float vx, vy, vz;
    float life;
    float maxLife;
    bool active;
};

class ParticleTestScript : public ScriptBase {
private:
    std::vector<SimpleParticle> particles;
    std::string scriptName;
    float emissionTimer;
    float emissionRate; // particles per second
    std::size_t maxParticles;
    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> velocityDist;
    std::uniform_real_distribution<float> lifeDist;

public:
    ParticleTestScript(const std::string& name = "ParticleTest") 
        : scriptName(name), emissionTimer(0.0f), emissionRate(10.0f), maxParticles(50),
          velocityDist(-2.0f, 2.0f), lifeDist(1.0f, 3.0f) {
        particles.reserve(maxParticles);
        std::cout << "[" << scriptName << "] Particle emitter created with max " << maxParticles << " particles\n";
    }

    void Start() override {
        std::cout << "[" << scriptName << "] START() - Particle emitter starting\n";
        
        // Add visual representation (emitter sphere)
        //AddTransformComponent(entity, 2.0f, 2.0f, 50.0f);
        //AddMeshRendererComponent(entity, PRIMITIVE_SPHERE, "RainbowShader");
        //SetTransformUniformScale(entity, 0.2f); // Small emitter sphere
    }

    void Update(float deltaTime) override {
        // Update emission timer
        emissionTimer += deltaTime;
        
        // Emit particles at the specified rate
        float emissionInterval = 1.0f / emissionRate;
        while (emissionTimer >= emissionInterval && GetActiveParticleCount() < maxParticles) {
            EmitParticle();
            emissionTimer -= emissionInterval;
        }
        
        // Update all particles
        UpdateParticles(deltaTime);
        
        // Log particle count periodically
        static float logTimer = 0.0f;
        logTimer += deltaTime;
        if (logTimer >= 2.0f) {
            LogParticleStats();
            logTimer = 0.0f;
        }
    }

    void EmitParticle() {
        // Find an inactive particle slot
        for (auto& particle : particles) {
            if (!particle.active) {
                CreateParticleEntity(particle);
                return;
            }
        }
        
        // If no slot found, create new particle (if under limit)
        if (particles.size() < maxParticles) {
            SimpleParticle newParticle;
            CreateParticleEntity(newParticle);
            particles.push_back(newParticle);
        }
    }

    void CreateParticleEntity(SimpleParticle& particle) {
        // Get emitter position
        float emitterX, emitterY, emitterZ;
        if (GetTransformComponent(entity, &emitterX, &emitterY, &emitterZ)) {
            // Create new entity for this particle
            particle.entityId = CreateEntity();
            
            // Position with slight random offset
            float startX = emitterX + velocityDist(randomEngine) * 0.1f;
            float startY = emitterY + velocityDist(randomEngine) * 0.1f;
            float startZ = emitterZ + velocityDist(randomEngine) * 0.1f;
            
            AddTransformComponent(particle.entityId, startX, startY, startZ);
            AddMeshRendererComponent(particle.entityId, PRIMITIVE_CUBE, "blue");
            SetMeshRendererColor(particle.entityId, 1.0f, 0.5f, 0.0f, 1.0f); // Orange particles
            SetTransformUniformScale(particle.entityId, 0.05f); // Very small particles
            
            // Set particle properties
            particle.vx = velocityDist(randomEngine);
            particle.vy = velocityDist(randomEngine) + 1.0f; // Slight upward bias
            particle.vz = velocityDist(randomEngine);
            particle.life = lifeDist(randomEngine);
            particle.maxLife = particle.life;
            particle.active = true;
        }
    }

    void UpdateParticles(float deltaTime) {
        for (auto& particle : particles) {
            if (!particle.active) continue;
            
            // Update lifetime
            particle.life -= deltaTime;
            
            if (particle.life <= 0.0f) {
                // Particle died - destroy entity and deactivate
                SafeDestroyEntity(particle.entityId);
                particle.active = false;
                continue;
            }
            
            // Update position based on velocity
            float currentX, currentY, currentZ;
            if (GetTransformComponent(particle.entityId, &currentX, &currentY, &currentZ)) {
                float newX = currentX + particle.vx * deltaTime;
                float newY = currentY + particle.vy * deltaTime;
                float newZ = currentZ + particle.vz * deltaTime;
                SetTransformPosition(particle.entityId, newX, newY, newZ);
                
                // Fade particle based on remaining life
                float alpha = particle.life / particle.maxLife;
                SetMeshRendererColor(particle.entityId, 1.0f, 0.5f, 0.0f, alpha);
            }
        }
    }

    size_t GetActiveParticleCount() const {
        size_t count = 0;
        for (const auto& particle : particles) {
            if (particle.active) count++;
        }
        return count;
    }

    void LogParticleStats() {
        size_t activeCount = GetActiveParticleCount();
        std::cout << "[" << scriptName << "] Active particles: " << activeCount 
                  << "/" << maxParticles << " (rate: " << emissionRate << "/sec)\n";
    }

    void SetEmissionRate(float rate) {
        emissionRate = rate;
        std::cout << "[" << scriptName << "] Emission rate set to " << rate << " particles/sec\n";
    }

    void OnDestroy() override {
        // Clean up all particle entities
        for (auto& particle : particles) {
            if (particle.active) {
                DestroyEntity(particle.entityId);
            }
        }
        std::cout << "[" << scriptName << "] Particle system destroyed\n";
    }
};