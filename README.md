# TronEngine

> *"Le paradoxe de la perfection, c'est qu'on ne l'atteint jamais..."* - The paradox of perfection is that we never achieve it...

A modern 3D game engine built with C++ and DirectX 11, featuring a comprehensive Entity-Component-System (ECS) architecture designed for real-time rendering and interactive experiences.

## 🎯 Project Overview

TronEngine is a 3D game engine developed as part of a learning project to understand game engine architecture and 3D graphics programming. The engine implements modern game development patterns and provides a complete framework for creating 3D applications and games.

### Key Features

- **🏗️ Entity-Component-System (ECS) Architecture** - Flexible and scalable game object management
- **🎨 DirectX 11 Rendering Pipeline** - Hardware-accelerated 3D graphics with custom shaders
- **🎮 Input Management** - Keyboard and mouse input handling with first-person controls
- **⚡ Physics System** - Collision detection with spatial grid optimization
- **📍 Raycast System** - Precision ray casting for object interaction and line-of-sight testing
- **🎯 Camera System** - First-person camera with matrix transformations
- **📜 Scripting System** - Component-based scripting for game logic
- **🔄 Multi-threaded Rendering** - Separate render thread for optimal performance

## 🚀 Getting Started

### Prerequisites

- **Visual Studio 2022** (v143 toolset)
- **Windows 10/11**
- **DirectX 11 SDK** (included with Windows SDK)
- **C++20 Standard** support

### Building the Project

1. **Clone the repository**
   ```bash
   git clone https://github.com/username/TronEngine.git
   cd TronEngine
   ```

2. **Open the solution**
   ```bash
   # Open in Visual Studio
   start TronEngine.sln
   ```

3. **Build the engine**
   - Set the build configuration to `Debug` or `Release`
   - Build the `TronEngine` project first (it's a dependency)
   - Build the `TestConsole` project for the demo application

4. **Run the demo**
   - The TestConsole project demonstrates the engine capabilities
   - Executable will be in `bin/Debug/` or `bin/Release/`

### Project Structure

```
TronEngine/
├── TronEngine/                 # Main engine library
│   ├── Source/
│   │   ├── Headers/           # Engine headers
│   │   │   ├── Core/         # Core engine systems
│   │   │   ├── Game/         # ECS components and systems
│   │   │   ├── Math/         # Mathematical utilities
│   │   │   └── Rendering/    # Rendering pipeline
│   │   └── Cpp/              # Implementation files
│   └── Include/              # Public API headers
├── TestConsole/              # Demo application
│   ├── TestConsole.cpp       # Main demo program
│   ├── *Script.hpp           # Example game scripts
│   └── *.hpp                 # Test components
└── bin/                      # Output directory
```

## 🎮 Demo Application

The TestConsole project showcases the engine's capabilities with an interactive 3D scene:

### Controls

- **ZQSD** - Move forward/left/backward/right
- **Mouse** - Look around (first-person camera)
- **Space** - Move up
- **Shift** - Move down
- **1-5** - Run various raycast tests
- **Mouse Click** - Object picking
- **P** - Print camera position
- **ESC** - Exit

### Features Demonstrated

- **First-Person Camera** - Smooth camera movement with mouse look
- **Raycast Testing** - Interactive object detection and line-of-sight tests
- **Physics System** - Box colliders with spatial grid optimization
- **Rendering** - Multiple colored objects with different shaders
- **Target Practice** - Interactive targets that can be hit with raycasts

## 🏗️ Engine Architecture

### Entity-Component-System (ECS)

The engine uses a modern ECS architecture for optimal performance and flexibility:

```cpp
// Create an entity
uint32_t player = CreateEntity();

// Add components
AddTransformComponent(player, 0.0f, 0.0f, 0.0f);
AddMeshRendererComponent(player, PRIMITIVE_CUBE, "basic_shader");
AddBoxColliderComponent(player, 1.0f, 1.0f, 1.0f, false);

// Add custom script
PlayerScript* script = new PlayerScript();
AddCustomScript(player, script);
```

### Core Systems

- **EntityManager** - Manages entity lifecycle and component masks
- **ComponentManager** - Handles component storage and retrieval  
- **SystemManager** - Updates all registered systems each frame
- **World** - Central coordinator for the ECS

### Components

- **Transform** - Position, rotation, and scale
- **MeshRenderer** - Visual representation with materials
- **BoxCollider** - Physics collision detection
- **Script** - Custom behavior scripting
- **Velocity** - Movement and rotation speeds

### Systems

- **MovementSystem** - Handles entity movement based on velocity
- **MeshRenderSystem** - Renders all visible objects
- **PhysicsSystem** - Collision detection and response
- **RaycastSystem** - Ray-object intersection testing
- **ScriptSystem** - Executes custom game logic

## 🎨 Rendering Pipeline

### DirectX 11 Features

- **Vertex and Pixel Shaders** - Custom HLSL shaders
- **Constant Buffers** - Efficient parameter passing to GPU
- **Matrix Transformations** - World, view, and projection matrices
- **Primitive Rendering** - Cubes, spheres, triangles, and custom meshes
- **Multi-threaded Command Queue** - Separate render thread

### Supported Primitives

```cpp
#define PRIMITIVE_QUAD 0
#define PRIMITIVE_CUBE 1  
#define PRIMITIVE_SPHERE 2
#define PRIMITIVE_CYLINDER 3
#define PRIMITIVE_PLANE 4
#define PRIMITIVE_TRIANGLE 5
#define PRIMITIVE_CUSTOM 6
```

## ⚡ Physics System

### Features

- **Spatial Grid** - Broad-phase collision detection optimization
- **AABB Collision Detection** - Axis-aligned bounding box testing
- **Trigger Support** - Non-solid collision zones
- **Performance Metrics** - Built-in profiling and statistics

### Usage

```cpp
// Add a physics collider
AddBoxColliderComponent(entity, width, height, depth, isTrigger);

// Configure physics system
SetPhysicsGridCellSize(5.0f);
SetPhysicsDebugOutput(true);

// Query performance
uint32_t collisionChecks, triggerEvents;
GetPhysicsMetrics(&collisionChecks, &triggerEvents);
```

## 📍 Raycast System

### Capabilities

- **Line-of-sight Testing** - Check visibility between points
- **Object Picking** - Mouse-based object selection
- **Camera Raycasts** - Cast rays from camera position
- **Performance Optimized** - Uses spatial grid for fast intersection tests

### API Example

```cpp
// Basic raycast
uint32_t hitEntity;
float hitX, hitY, hitZ, distance;
bool hit = Raycast(startX, startY, startZ, dirX, dirY, dirZ,
                   &hitEntity, &hitX, &hitY, &hitZ, &distance);

// Camera-based raycast
bool cameraHit = RaycastFromCamera(0.0f, 0.0f, -1.0f,
                                   &hitEntity, &hitX, &hitY, &hitZ, &distance);

// Line of sight test
bool canSee = LineOfSight(x1, y1, z1, x2, y2, z2);
```

## 🔧 Scripting System

Create custom game logic by inheriting from `ScriptBase`:

```cpp
class PlayerScript : public ScriptBase {
    float movementSpeed = 5.0f;
    
public:
    void Start() override {
        // Initialize the script
        AddTransformComponent(entity, 0.0f, 0.0f, 0.0f);
    }
    
    void Update(float deltaTime) override {
        // Process input and update logic
        if (IsKeyDown('W')) {
            // Move forward
        }
    }
    
    void OnDestroy() override {
        // Cleanup
    }
};
```

## 📊 Performance

### Threading Architecture

- **Main Thread** - Game logic, input, and physics
- **Render Thread** - Graphics rendering and GPU commands
- **Target Framerate** - 120 FPS main loop, 60 FPS rendering

### Optimization Features

- **Spatial Grid Collision Detection** - O(n) instead of O(n²) collision checks
- **Frustum Culling** - Only render visible objects
- **Efficient Memory Management** - Component pooling and object reuse
- **Command-based Rendering** - Decoupled game logic from rendering

## 🤝 Contributors

OROITZ LAGO RAMOS
OUSSEMA FATNASSI
GUILLAUME BOMBEN