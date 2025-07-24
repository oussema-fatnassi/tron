#pragma once
#include "D3DContext.hpp"
#include "SwapChain.hpp"
#include "CommandQueue.hpp"
#include "../Resources/ShaderManager.hpp"
#include "../Resources/MeshManager.hpp"
#include "../Resources/MaterialManager.hpp"

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
