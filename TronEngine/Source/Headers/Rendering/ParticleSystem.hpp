#pragma once
#include <vector>
#include <memory>
#include <cstdint>

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    // Add color, size, etc. as needed
};

class ParticleSystem {
public:
    ParticleSystem(std::size_t maxParticles);
    void Update(float deltaTime);
    void Render(); // Integrate with your rendering teammate
    void Emit(const Particle& particle);

private:
    std::vector<Particle> particles_;
    std::size_t maxParticles_;
};