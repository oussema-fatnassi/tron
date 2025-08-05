#pragma once
#include <d3d11.h>
#include "D3DContext.hpp"

class SwapChain {
public:
    SwapChain();
    ~SwapChain();

    bool Initialize(D3DContext* context, HWND hwnd, int width, int height);
    void Present();

    ID3D11RenderTargetView* GetBackBufferRTV() const { return renderTargetView; }
    ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView; }
    ID3D11DepthStencilState* GetDepthStencilState() const { return depthStencilState; }

private:
    int width, height;

    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

    // Depth-stencil resources
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11DepthStencilState* depthStencilState;

    bool CreateDepthStencilBuffer(D3DContext* context);
    bool CreateDepthStencilState(D3DContext* context);
};