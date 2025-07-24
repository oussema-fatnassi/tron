#include "../../Headers/Rendering/RenderEngine.hpp"
#include "../../Headers/Rendering/Resources/ShaderManager.hpp"

RenderEngine::RenderEngine(HWND hwnd, int width, int height)
    : hwnd(hwnd), width(width), height(height),
    context(nullptr), swapChain(nullptr), renderer(nullptr),
    commandQueue(nullptr), shaderManager(nullptr) {
}

RenderEngine::~RenderEngine() {
    Shutdown();
}

void RenderEngine::Initialize() {
    context = new D3DContext();
    context->Initialize(hwnd, width, height);

    swapChain = new SwapChain();
    swapChain->Initialize(context, hwnd, width, height);

    commandQueue = new CommandQueue();

    // Cr�ation du ShaderManager
    shaderManager = new ShaderManager();
    if (!shaderManager->LoadShader(context->GetDevice(), "default",
        L"Source/shaders/PixelShader.hlsl",
        L"Source/shaders/PixelShader.hlsl")) {
        OutputDebugStringA("�chec du chargement du shader par d�faut.\n");
    }

    renderer = new D3DRenderer();
    renderer->Initialize(context, swapChain, commandQueue);
}

void RenderEngine::RenderFrame() {
    commandQueue->Flush(); // R�cup�re commandes depuis la GameThread (non montr� ici)
    renderer->Render();
    swapChain->Present();
}

void RenderEngine::Shutdown() {
    delete renderer;
    delete commandQueue;
    delete swapChain;
    delete shaderManager;
    delete context;
}
