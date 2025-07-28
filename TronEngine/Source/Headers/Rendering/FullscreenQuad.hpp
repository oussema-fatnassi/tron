#pragma once
#include <d3d11.h>
#include <iostream>

class FullscreenQuad {
public:
    FullscreenQuad();
    ~FullscreenQuad();

    bool Initialize(ID3D11Device* device);
    void Render(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer = nullptr);
    void Shutdown();

private:
    ID3D11Buffer* vertexBuffer;
    UINT vertexCount;

    void CreateVertexBuffer(ID3D11Device* device);
};