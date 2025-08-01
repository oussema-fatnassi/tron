#pragma once
#pragma once
#include "TronEngine.hpp"
#include <iostream>

class SecondScript : public ScriptBase {
private:
	uint32_t playerEntity = -1; // Reference to the player entity

public:
    SecondScript(){}
    SecondScript(uint32_t playerEntity)
    :playerEntity(playerEntity) {}

    void Start() override {
		AddBoxColliderComponent(entity, 1.0f, 1.0f, 1.0f, true); // Add a collider
    }

    void Update(float deltaTime) override 
    {
        
    }

    void OnDestroy() override {
		
    }

	void OnTriggerEnter(uint32_t otherEntity) override {
        if (playerEntity >= 0 && otherEntity == playerEntity)
        {
		    SafeDestroyEntity(entity);
        }
        else if (playerEntity < 0)
        {
			SafeDestroyEntity(entity); // Destroy this entity if playerEntity is not set
        }
	}
};