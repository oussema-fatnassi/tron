#pragma once

#include "D3D/D3DContext.hpp"
#include "D3D/D3DRenderer.hpp"
#include "D3D/SwapChain.hpp"
#include "D3D/CommandQueue.hpp"
#include "Resources/ShaderManager.hpp"

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
