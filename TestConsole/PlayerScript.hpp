// PlayerScript.hpp - Enhanced with simple lifecycle
#pragma once
#include "TronEngine.hpp"
#include "SecondScript.hpp"
#include <iostream>
#include <string>

class PlayerScript : public ScriptBase {
private:
    std::string playerName;
    bool hasStoppedAtTarget = false;
    float targetX = 250.0f;
    int updateCount = 0;

	uint32_t* secondEntity = nullptr;
	SecondScript* secondScript = new SecondScript();


public:
    PlayerScript(uint32_t* secondEntity, const std::string& name = "Player")
        : secondEntity(secondEntity),playerName(name), hasStoppedAtTarget(false) {
        std::cout << "[" << playerName << "] Script instance created" << std::endl;
    }

    void Start() override {
        std::cout << "\n[" << playerName << "] START() called on entity " << entity << std::endl;

        // Get initial state
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z)) {
            std::cout << "[" << playerName << "] Starting position: ("
                << x << ", " << y << ", " << z << ")" << std::endl;
        }

        float vx, vy, vz;
        if (GetVelocityComponent(entity, &vx, &vy, &vz)) {
            std::cout << "[" << playerName << "] Starting velocity: ("
                << vx << ", " << vy << ", " << vz << ")" << std::endl;
            if (vx > 0) {
                std::cout << "[" << playerName << "] ETA to target: ~"
                    << (targetX / vx) << " seconds" << std::endl;
            }
        }

        std::cout << "[" << playerName << "] Initialization complete!\n" << std::endl;
    }

    void Update(float deltaTime) override {
        updateCount++;

        if (IsKeyDown(90))
        {
			std::cout << "[" << playerName << "] Z key down" << entity << std::endl;
        }
        /*if (IsKeyPressed(90))
        {
			std::cout << "[" << playerName << "] Z key pressed" << entity << std::endl;
        }*/
        if (IsKeyUp(90))
        {
            std::cout << "[" << playerName << "] Z key released" << entity << std::endl;
        }

        /*float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z)) {
            if (x >= targetX) {
                std::cout << "[" << playerName << "] TARGET REACHED at x="
                    << x << " after " << updateCount << " updates!" << std::endl;

                RemoveVelocityComponent(entity);
                AddVelocityComponent(entity, -100.0f, 0.0f, 0.0f);
				AddCustomScript(*secondEntity, secondScript);
                hasStoppedAtTarget = true;

                std::cout << "[" << playerName << "] Movement stopped!" << std::endl;
            }
            if (x < 0)
            {
				RemoveScript(*secondEntity);
				DestroyEntity(entity);
            }
        }*/
    }

    void OnDestroy() override {
        std::cout << "\n[" << playerName << "] OnDestroy() called on entity " << entity << std::endl;

        // Final report
        float x, y, z;
        if (GetTransformComponent(entity, &x, &y, &z)) {
            std::cout << "[" << playerName << "] Final position: x=" << x << std::endl;
        }

        std::cout << "[" << playerName << "] Final stats:" << std::endl;
        std::cout << "[" << playerName << "]   - Total updates: " << updateCount << std::endl;
        std::cout << "[" << playerName << "]   - Target reached: "
            << (hasStoppedAtTarget ? "YES" : "NO") << std::endl;

        std::cout << "[" << playerName << "]  Script cleanup complete!\n" << std::endl;
    }

    // Public interface
    const std::string& GetName() const { return playerName; }
    bool HasReachedTarget() const { return hasStoppedAtTarget; }
    int GetUpdateCount() const { return updateCount; }
};