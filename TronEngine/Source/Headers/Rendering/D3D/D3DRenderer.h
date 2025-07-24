#pragma once
#include "D3DContext.h"
#include "SwapChain.h"
#include "CommandQueue.h"
#include "../Resources/ShaderManager.h"
#include "../Resources/MeshManager.h"
#include "../Resources/MaterialManager.h"

class D3DRenderer {
public:
    D3DRenderer();
    ~D3DRenderer();

    void Initialize(D3DContext* context, SwapChain* swapChain, CommandQueue* commandQueue);
    void DrawMesh(ID3D11DeviceContext* context, const MeshData& mesh, const Material& material);
    void Render();

private:
    D3DContext* context;
    SwapChain* swapChain;
    CommandQueue* commandQueue;
};
