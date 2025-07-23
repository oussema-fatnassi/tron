#pragma once
#include <cstdint>
#include <bitset>

// <summary>
// Types.hpp defines fundamental types used in the ECS (Entity-Component-System) architecture.
// It includes definitions for Entity, ComponentType, ComponentMask, and constants related to components.
// </summary>
// <remarks>
// This file is essential for the ECS framework, providing type definitions that are used throughout the system.
// Entity represents a unique identifier for each entity in the system.
// ComponentType is used to identify different types of components, and ComponentMask is a bitset
// that tracks which components an entity has.
// MAX_COMPONENTS defines the maximum number of components that can be associated with an entity.
// INVALID_ENTITY is a constant used to represent an invalid or non-existent entity.
// </remarks>
namespace Tron {

    using Entity = uint32_t;
    using ComponentType = uint8_t;

    constexpr size_t MAX_COMPONENTS = 32;

    using ComponentMask = std::bitset<MAX_COMPONENTS>;

    constexpr Entity INVALID_ENTITY = 0;
}