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
-Make the update functions of the scripts in separates threads
-Make possible to an entity have more than one script attached and iterate for each script
-If DestroyEntity is used on a custom script in an update function, it will crash if funcs are called after.
-The system shutdown itself after the run method called
-At the end of the game loop destroy the window
-Memory leak problem in TestConsole due to the method of adding scripts
    Option A: Engine takes ownership and deletes user scripts
    Option B: User manages memory (current approach)
    Option C: Use smart pointers in API

-Verify memory clean when an entity has benn erased
    Entity destruction:
        Entity ID gets recycled (goes to _availableEntities queue)
        Memory stays allocated but marked as "inactive"
    Actually It's Not That Bad!
        The memory stabilizes after the first wave:
            Vectors grow to max size, then stop growing
            Components are properly deleted/recreated
            Only the "bookkeeping" memory stays allocated

    Real issue: If you have one mega-wave of 100,000 enemies, memory stays allocated for 100,000 entities forever, even if you never use that many again.
    For typical games: This is actually acceptable behavior! The memory footprint stabilizes at your "peak usage."
    FOR THE FUTURE IMPLEMENT A MEMORY SHRINK SYSTEM????

- ✅ Add to the CustomScripting functions Start et OnDestroy()
- ✅ Add the possibility to Destry the entity from a customScript
- ✅ Correct and improve the deltaTime and the time system

-Do the system like entity->GetPosition.x or similar
-Add rotations

-InputSystem and close management
