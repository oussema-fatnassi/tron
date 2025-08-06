#include "Rendering/ParticleSystem.hpp"

ParticleSystem::ParticleSystem(std::size_t maxParticles)
    : maxParticles_(maxParticles) {
    particles_.reserve(maxParticles_);
}

void ParticleSystem::Update(float deltaTime) {
    for (auto& p : particles_) {
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
        p.z += p.vz * deltaTime;
        p.life -= deltaTime;
    }
    // Remove dead particles
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        particles_.end());
}

void ParticleSystem::Emit(const Particle& particle) {
    if (particles_.size() < maxParticles_)
        particles_.push_back(particle);
}

// Render() implementation depends on your rendering pipeline