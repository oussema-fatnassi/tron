#pragma once
#include "TronEngine.hpp"

class PlayerScript : public ScriptBase {
public:
    std::string playerName;
    bool hasStoppedAtTarget = false;
    // Remove entityId - it's now inherited from ScriptBase

    PlayerScript(const std::string& name = "Player")
        : playerName(name), hasStoppedAtTarget(false) {
    }

    void Update(float deltaTime) override {
        //std::cout << "[" << playerName << "] Entity " << entityId << " updating" << std::endl;

        if (!hasStoppedAtTarget) {
            float x, y, z;
            if (GetTransformComponent(entityId, &x, &y, &z)) {
                if (x >= 500.0f) {
                    std::cout << "Entity " << entityId << "  -> REACHED 500! Stopping at x=" << x << std::endl;
                    RemoveVelocityComponent(entityId);
                    hasStoppedAtTarget = true;
                }
            }
        }
    }
};