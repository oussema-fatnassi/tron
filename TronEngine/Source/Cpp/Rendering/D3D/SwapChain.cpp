#include "../../../Headers/Rendering/D3D/SwapChain.hpp"
#include <dxgi.h>

SwapChain::SwapChain() : swapChain(nullptr), renderTargetView(nullptr) {}

SwapChain::~SwapChain() {
    if (renderTargetView) renderTargetView->Release();
    if (swapChain) swapChain->Release();
}

bool SwapChain::Initialize(D3DContext* context, HWND hwnd, int width, int height) {
    if (!context || !context->GetDevice()) {
        return false;
    }
    
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 1;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = hwnd;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGIDevice* dxgiDevice = nullptr;
    HRESULT hr = context->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    if (FAILED(hr)) return false;

    IDXGIAdapter* adapter = nullptr;
    hr = dxgiDevice->GetAdapter(&adapter);
    if (FAILED(hr)) {
        dxgiDevice->Release();
        return false;
    }

    IDXGIFactory* factory = nullptr;
    hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(hr)) {
        adapter->Release();
        dxgiDevice->Release();
        return false;
    }

    hr = factory->CreateSwapChain(context->GetDevice(), &desc, &swapChain);

    // Clean up intermediate objects
    factory->Release();
    adapter->Release();
    dxgiDevice->Release();

    if (FAILED(hr)) return false;

    // Create render target view
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) return false;

    hr = context->GetDevice()->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();

    return SUCCEEDED(hr);
}

void SwapChain::Present() {
    if (swapChain) {
        swapChain->Present(1, 0); // VSync on
    }
}
