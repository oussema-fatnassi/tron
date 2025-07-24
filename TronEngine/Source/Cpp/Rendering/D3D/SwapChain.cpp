#include "../../../Headers/Rendering/D3D/SwapChain.hpp"
#include <dxgi.h>

SwapChain::SwapChain() : swapChain(nullptr), renderTargetView(nullptr) {}

SwapChain::~SwapChain() {
    if (renderTargetView) renderTargetView->Release();
    if (swapChain) swapChain->Release();
}

bool SwapChain::Initialize(D3DContext* context, HWND hwnd, int width, int height) {
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 1;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = hwnd;
    desc.SampleDesc.Count = 1;
    desc.Windowed = TRUE;

    IDXGIDevice* dxgiDevice = nullptr;
    context->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

    IDXGIAdapter* adapter = nullptr;
    dxgiDevice->GetAdapter(&adapter);

    IDXGIFactory* factory = nullptr;
    adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);

    factory->CreateSwapChain(context->GetDevice(), &desc, &swapChain);

    ID3D11Texture2D* backBuffer = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    context->GetDevice()->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();

    return true;
}

void SwapChain::Present() {
    swapChain->Present(1, 0);
}
