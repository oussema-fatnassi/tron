#pragma once
#include "System.hpp"
#include <iostream>
#include <iomanip>

// <summary>
// DebugSystem is a System that prints debug information at regular intervals.
// It inherits from the System class and overrides the Update method to handle debug output.
// </summary>
// <remarks>
// This system is useful for monitoring the state of the ECS and can be customized to print different
// types of information based on the needs of the application.
// The print interval can be adjusted to control how often debug information is output.
// </remarks>

class DebugSystem : public Tron::System {
private:
    float printTimer = 0.0f;
    float printInterval = 1.0f;

public:
    DebugSystem(float interval = 1.0f);
    void Update(float deltaTime) override;
    void SetPrintInterval(float interval);
};