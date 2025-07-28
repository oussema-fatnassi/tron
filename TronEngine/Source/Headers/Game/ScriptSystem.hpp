#pragma once
#include "System.hpp"

class ScriptSystem : public System {
public:
    ScriptSystem() = default;
	~ScriptSystem() override = default;
	void OnEntityAdded(Entity entity) override;
	void OnEntityRemoved(Entity entity) override;
    void Update(float deltaTime) override;
};