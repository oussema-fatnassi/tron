#pragma once
#include "TronEngine.hpp"

class PlayerScript : public ScriptBase {
public:
    std::string playerName;
    bool hasStoppedAtTarget = false;

    PlayerScript(const std::string& name = "Player")
        : playerName(name), hasStoppedAtTarget(false) {
    }

    void Update(float deltaTime) override {
        // Check position and stop at x=500
        if (!hasStoppedAtTarget) {
            float x, y, z;
            if (GetTransformComponent(entity, &x, &y, &z)) {
                if (x >= 500.0f) {
                    std::cout << "  -> REACHED 500! Stopping at x=" << x << std::endl;
                    RemoveVelocityComponent(entity);
                    hasStoppedAtTarget = true;
                }
            }
        }
    }
};