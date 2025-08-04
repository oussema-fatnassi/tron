# tron

tron/
├── TronEngine.sln           // Solution file
├── TronEngine/              // Main Engine DLL (YOU work on this)
│   ├── Core/               // Engine core, threading, DLL exports
│   ├── Rendering/          // Teammate #1 works on this
│   └── Game/              // Teammate #2 works on this
├── TestConsole/           // Simple test project
├── TronGame/             // Actual TRON game implementation
└── bin/                  // Output folder (auto-created)
    ├── TronEngine.dll
    ├── TestConsole.exe
    └── TronGame.exe


TODO : 

-Add layer to the collision system in order to optimize
-Make the update functions of the scripts in separates threads

-Make possible to an entity have more than one script attached and iterate for each script
-Same for BoxColliders

-Clean close management - The system shutdown itself after the run method called? - At the end of the game loop destroy the window

-Create an API class Vector3D or struct

- ✅ Solve the flickering problem - BufferedRenderCommandQueue or thread
- ✅ Add to the CustomScripting functions Start et OnDestroy()
- ✅ Add the possibility to Destry the entity from a customScript
- ✅ Correct and improve the deltaTime and the time system
- ✅ If DestroyEntity is used on a custom script in an update function, it will crash if funcs are called after.
- ✅ Add rotations
- ✅ InputSystem


