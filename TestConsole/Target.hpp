#pragma once
#include "TronEngine.hpp"
#include "ParticleTestScript.hpp"

class Target : public ScriptBase
{
private:
    uint32_t _particles = 0;
    float _particleTimer = 0.0f;
    float _maxParticleTImer = 5.0f;
    bool _hasSpawnParticle = false;
public:
    Target() {}

    void Start() override {
        _particles = CreateEntity();
    }

    void Update(float deltaTime) override {
        if (!_hasSpawnParticle) { return; }
        _particleTimer += deltaTime;
        if (_particleTimer >= _maxParticleTImer)
        {
            SafeDestroyEntity(_particles);
            SafeDestroyEntity(entity);
        }
    }

    void OnDestroy() override {}

    void OnTriggerEnter(uint32_t otherEntity) override
    {
        SafeDestroyEntity(otherEntity);
        SpawnParticle();
        _hasSpawnParticle = true;
        RemoveMeshRendererComponent(entity);
        RemoveBoxColliderComponent(entity);
    }

    void SpawnParticle()
    {
        float pX, pY, pZ;
        GetTransformComponent(entity, &pX, &pY, &pZ);
        AddTransformComponent(_particles, pX, pY, pZ);
        ParticleTestScript* particleScript = new ParticleTestScript();
        AddCustomScript(_particles, particleScript);
    }
};