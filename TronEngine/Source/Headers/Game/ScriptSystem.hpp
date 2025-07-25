#pragma once
#include "System.hpp"

class ScriptSystem : public System {
public:
    ScriptSystem() = default;
    void Update(float deltaTime) override;
};