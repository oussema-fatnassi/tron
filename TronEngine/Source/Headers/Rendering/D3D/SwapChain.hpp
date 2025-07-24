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

private:
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;
};
