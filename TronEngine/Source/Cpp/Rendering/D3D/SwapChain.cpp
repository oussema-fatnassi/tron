#include "../../../Headers/Rendering/D3D/SwapChain.hpp"
#include <dxgi.h>
#include <iostream>

SwapChain::SwapChain()
    : width(0), height(0),
    swapChain(nullptr), renderTargetView(nullptr),
    depthStencilBuffer(nullptr), depthStencilView(nullptr), depthStencilState(nullptr) {
}

SwapChain::~SwapChain() {
    if (depthStencilState) depthStencilState->Release();
    if (depthStencilView) depthStencilView->Release();
    if (depthStencilBuffer) depthStencilBuffer->Release();
    if (renderTargetView) renderTargetView->Release();
    if (swapChain) swapChain->Release();
}

bool SwapChain::Initialize(D3DContext* context, HWND hwnd, int width, int height) {
    if (!context || !context->GetDevice()) {
        std::cout << "[SwapChain] Error: Invalid D3D context\n";
        return false;
    }

    this->width = width;
    this->height = height;

    // === Create Swap Chain ===
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
    if (FAILED(hr)) {
        std::cout << "[SwapChain] Failed to get DXGI device\n";
        return false;
    }

    IDXGIAdapter* adapter = nullptr;
    hr = dxgiDevice->GetAdapter(&adapter);
    if (FAILED(hr)) {
        dxgiDevice->Release();
        std::cout << "[SwapChain] Failed to get DXGI adapter\n";
        return false;
    }

    IDXGIFactory* factory = nullptr;
    hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(hr)) {
        adapter->Release();
        dxgiDevice->Release();
        std::cout << "[SwapChain] Failed to get DXGI factory\n";
        return false;
    }

    hr = factory->CreateSwapChain(context->GetDevice(), &desc, &swapChain);

    // Clean up intermediate objects
    factory->Release();
    adapter->Release();
    dxgiDevice->Release();

    if (FAILED(hr)) {
        std::cout << "[SwapChain] Failed to create swap chain: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    // === Create Render Target View ===
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) {
        std::cout << "[SwapChain] Failed to get back buffer\n";
        return false;
    }

    hr = context->GetDevice()->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();

    if (FAILED(hr)) {
        std::cout << "[SwapChain] Failed to create render target view: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    // === Create Depth-Stencil Buffer and View ===
    if (!CreateDepthStencilBuffer(context)) {
        std::cout << "[SwapChain] Failed to create depth-stencil buffer\n";
        return false;
    }

    // === Create Depth-Stencil State ===
    if (!CreateDepthStencilState(context)) {
        std::cout << "[SwapChain] Failed to create depth-stencil state\n";
        return false;
    }

    std::cout << "[SwapChain] Initialized successfully with depth-stencil support\n";
    return true;
}

bool SwapChain::CreateDepthStencilBuffer(D3DContext* context) {
    // Create depth-stencil buffer
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    HRESULT hr = context->GetDevice()->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) {
        std::cout << "[SwapChain] Failed to create depth-stencil buffer: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    // Create depth-stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    hr = context->GetDevice()->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
    if (FAILED(hr)) {
        std::cout << "[SwapChain] Failed to create depth-stencil view: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    return true;
}

bool SwapChain::CreateDepthStencilState(D3DContext* context) {
    // Create depth-stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    // Depth test parameters
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    depthStencilDesc.StencilEnable = TRUE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    HRESULT hr = context->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    if (FAILED(hr)) {
        std::cout << "[SwapChain] Failed to create depth-stencil state: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    return true;
}

void SwapChain::Present() {
    if (swapChain) {
        swapChain->Present(1, 0); // VSync on
    }
}