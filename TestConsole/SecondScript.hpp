#pragma once
#pragma once
#include "TronEngine.hpp"
#include <iostream>

class SecondScript : public ScriptBase {
private:
	uint32_t playerEntity = -1; // Reference to the player entity
	int frameCount = 0; // Frame counter for debugging

public:
    SecondScript(){}
    SecondScript(uint32_t playerEntity)
    :playerEntity(playerEntity) {}

    void Start() override {
		AddBoxColliderComponent(entity, 1.0f, 1.0f, 1.0f, true); // Add a collider
		//Add a random velocity to this entity
		AddVelocityComponent(entity,
			static_cast<float>(rand() % 10 - 5), // Random X velocity between -5 and 5
			static_cast<float>(rand() % 10 - 5), // Random Y velocity between -5 and 5
			0.0f
		);
    }

    void Update(float deltaTime) override 
    {
		 //Add a low probability of destroying this entity each frame
		//if (frameCount >= 50 && rand() % 100 < 5)
  //      {
		//	SafeDestroyEntity(entity);
  //      }
		//frameCount++; // Increment frame count for this script
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