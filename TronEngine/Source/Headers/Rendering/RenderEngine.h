#pragma once

#include "D3D/D3DContext.h"
#include "D3D/D3DRenderer.h"
#include "D3D/SwapChain.h"
#include "D3D/CommandQueue.h"
#include "Resources/ShaderManager.h"

class RenderEngine {
public:
    RenderEngine(HWND hwnd, int width, int height);
    ~RenderEngine();

    void Initialize();
    void RenderFrame();
    void Shutdown();

private:
    HWND hwnd;
    int width, height;

    D3DContext* context;
    SwapChain* swapChain;
    D3DRenderer* renderer;
    CommandQueue* commandQueue;
    ShaderManager* shaderManager;
};
