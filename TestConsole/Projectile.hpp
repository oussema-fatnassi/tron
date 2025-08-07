#pragma once
#include "TronEngine.hpp"
#include <vector>
#include <math.h>

class Projectile : public ScriptBase
{
private:
	float _maxDistance = 100.0f;

	uint32_t _player;

public:
	Projectile(uint32_t player)
		: _player(player) {}

	void Start() override
	{
	}

	void Update(float deltaTime) override
	{
		// If the distance is to long destroy the projectile
		float pX, pY, pZ;
		GetTransformComponent(_player, &pX, &pY, &pZ);

		float x, y, z;
		GetTransformComponent(entity, &x, &y, &z);

		// Calculate the distance
		float distance = sqrtf(powf(x - pX, 2) + powf(y - pY, 2) + powf(z - pZ, 2));
		if (distance > _maxDistance)
		{
			std::cout << "Projectile destroyed due to max distance reached: " << distance << "\n";
			SafeDestroyEntity(entity);
			return;
		}
	}
};