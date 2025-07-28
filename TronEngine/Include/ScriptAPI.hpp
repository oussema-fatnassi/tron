#pragma once

#ifdef ENGINE_EXPORTS
#define SCRIPT_API __declspec(dllexport)
#else
#define SCRIPT_API __declspec(dllimport)
#endif

// Base script class that users inherit from
struct SCRIPT_API ScriptBase {
    uint32_t entity = 0;

    // Lifecycle methods
    virtual void Start() {}                    // Called once when script is added
    virtual void Update(float deltaTime) = 0;  // Called every frame
    virtual void OnDestroy() {}                // Called when Entity is removed/destroyed

    virtual ~ScriptBase() = default;
};