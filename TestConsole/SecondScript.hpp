#pragma once
#pragma once
#include "TronEngine.hpp"
#include <iostream>

class SecondScript : public ScriptBase {
private:

public:
    SecondScript(){}

    void Start() override {
		AddBoxColliderComponent(entity, 1.0f, 1.0f, 1.0f, true); // Add a collider
    }

    void Update(float deltaTime) override 
    {
        
    }

    void OnDestroy() override {
		
    }

	void OnTriggerEnter(uint32_t otherEntity) override {
		SafeDestroyEntity(entity);
	}
};