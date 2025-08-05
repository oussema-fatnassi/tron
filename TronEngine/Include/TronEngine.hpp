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

// ADD: Timing configuration constants
#define TRON_GAME_TARGET_FPS 120.0
#define TRON_RENDER_TARGET_FPS 60.0
#define TRON_GAME_TARGET_DELTA (1.0 / TRON_GAME_TARGET_FPS)    // 8.33ms
#define TRON_RENDER_TARGET_DELTA (1.0 / TRON_RENDER_TARGET_FPS) // 16.67ms

// Utility macros
// TODO: Check what is used for and if we need to keep it
#define TRON_UNUSED(x) ((void)(x))

// Camera Entity Integration - NEW API
bool SetCameraEntity(uint32_t entity);
uint32_t GetCameraEntity();
void PrintCameraMatrices();