#pragma once
#include "TronEngine.hpp"
#include <vector>
#include <math.h>

class Projectile : public ScriptBase
{
private:
	float _maxDistance = 100.0f;

	uint32_t _player;
	std::vector<uint32_t> *_projectilesRef;

public:
	Projectile(uint32_t player, std::vector<uint32_t> *projectiles)
		: _player(player), _projectilesRef(projectiles) {}

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
			DestroyEntity(entity);
			return;
		}
	}
	void OnDestroy() override
	{
		std::cout << "Projectile destroyed: " << entity << "\n";
		// Remove from the projectile list
		auto it = std::find(_projectilesRef->begin(), _projectilesRef->end(), entity);
		if (it != _projectilesRef->end())
		{
			_projectilesRef->erase(it);
			std::cout << "Projectile removed from list\n";
		}
		else
		{
			std::cout << "Projectile not found in list\n";
		}
	}
	void OnTriggerEnter(uint32_t otherEntity) override
	{
		DestroyEntity(entity);
	}
};