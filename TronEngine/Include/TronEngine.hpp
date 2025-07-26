#pragma once
// Main include for TRON Engine clients
// This is the ONLY header external projects need
#include "EngineExports.hpp"
#include "ScriptAPI.hpp"

// Version information
#define TRON_ENGINE_VERSION_MAJOR 1
#define TRON_ENGINE_VERSION_MINOR 0
#define TRON_ENGINE_VERSION_PATCH 0
#define TRON_ENGINE_VERSION_STRING "1.0.1"

// Utility macros
// TODO: Check what is used for and if we need to keep it
#define TRON_UNUSED(x) ((void)(x))