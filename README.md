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
Correct and improve the deltaTime and the time system
Add more than Update to the custom scripts
Do the system like entity->GetPosition.x or similar
Add rotations
Close logic and InputSystem





FUTURE TODO : 
REWORK THE COMPONENT SYSTEM TO SIMPLIFY THE CUSTOM SCRIPT PART (not needed for the moment)
