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

-Add to the CustomScripting functions Start et OnDestroy()
-Correct and improve the deltaTime and the time system
-Add more than Update to the custom scripts
-Do the system like entity->GetPosition.x or similar
-Add rotations
-Close logic and InputSystem





FUTURE TODO : 
REWORK THE COMPONENT SYSTEM TO SIMPLIFY THE CUSTOM SCRIPT PART (not needed for the moment)
