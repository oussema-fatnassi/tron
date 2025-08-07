#pragma once
#include "TronEngine.hpp"

class Target : public ScriptBase
{
private:
public:
    Target() {}

    void Start() override {}

    void Update(float deltaTime) override {}

    void OnDestroy() override {}

    void OnTriggerEnter(uint32_t otherEntity) override
    {
        DestroyEntity(entity);
    }
};