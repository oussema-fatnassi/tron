#pragma once
#pragma once
#include "TronEngine.hpp"
#include <iostream>

class SecondScript : public ScriptBase {
private:

public:
    SecondScript(){}

    void Start() override {
        std::cout << " START() called on entity " << entity << " from secondScript" << std::endl;
		AddTransformComponent(entity, 0.0f, 0.0f, 0.0f);
		AddVelocityComponent(entity, 0.0f, -10.0f, 0.0f);
    }

    void Update(float deltaTime) override 
    {
        std::cout << "aaaaaa";
    }

    void OnDestroy() override {
		std::cout << " OnDestroy() called on entity " << entity << " from secondScript" << std::endl;
		RemoveVelocityComponent(entity);
    }
};