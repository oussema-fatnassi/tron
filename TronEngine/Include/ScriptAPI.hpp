#pragma once

#ifdef ENGINE_EXPORTS
#define SCRIPT_API __declspec(dllexport)
#else
#define SCRIPT_API __declspec(dllimport)
#endif

// Base script class that users inherit from
struct SCRIPT_API ScriptBase {
    uint32_t entityId = 0;

    virtual void Update(float deltaTime) = 0;
    virtual ~ScriptBase() = default;
};