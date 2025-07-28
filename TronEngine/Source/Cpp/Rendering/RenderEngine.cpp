#include "../../Headers/Rendering/RenderEngine.hpp"
#include "../../Headers/Rendering/Resources/ShaderManager.hpp"
#include "../../Headers/Rendering/FullscreenQuad.hpp"

RenderEngine::RenderEngine(HWND hwnd, int width, int height)
    : hwnd(hwnd), width(width), height(height),
    context(nullptr), swapChain(nullptr), renderer(nullptr),
    commandQueue(nullptr), shaderManager(nullptr),
    fullscreenQuad(nullptr), colorConstantBuffer(nullptr) {
}

RenderEngine::~RenderEngine() {
    Shutdown();
}

void RenderEngine::Initialize() {
    // Initialize D3D Context
    context = new D3DContext();
    if (!context->Initialize(hwnd, width, height)) {
        std::cout << "[RenderEngine] Failed to initialize D3D context\n";
        return;
    }

    // Initialize Swap Chain (now includes depth-stencil buffer)
    swapChain = new SwapChain();
    if (!swapChain->Initialize(context, hwnd, width, height)) {
        std::cout << "[RenderEngine] Failed to initialize swap chain\n";
        return;
    }

    // Initialize Command Queue
    commandQueue = new CommandQueue();

    // Initialize Shader Manager and load shaders
    shaderManager = new ShaderManager();
    if (!shaderManager->LoadShader(context->GetDevice(), "default", L"VertexShader.hlsl", L"PixelShader.hlsl")) {
        std::cout << "[RenderEngine] Failed to load shaders\n";
    }

    // Initialize Renderer
    renderer = new D3DRenderer();
    renderer->Initialize(context, swapChain, commandQueue);

    // Create constant buffer
    CreateConstantBuffer();

    // Initialize fullscreen quad
    fullscreenQuad = new FullscreenQuad();
    if (!fullscreenQuad->Initialize(context->GetDevice())) {
        std::cout << "[RenderEngine] Failed to initialize fullscreen quad\n";
        delete fullscreenQuad;
        fullscreenQuad = nullptr;
    }
}

void RenderEngine::CreateConstantBuffer() {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(float) * 4; // float4 color
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    float colorData[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Green

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = colorData;

    HRESULT hr = context->GetDevice()->CreateBuffer(&bufferDesc, &initData, &colorConstantBuffer);
    if (FAILED(hr)) {
        std::cout << "[RenderEngine] Failed to create constant buffer: " << std::hex << hr << std::dec << "\n";
        colorConstantBuffer = nullptr;
    }
}

void RenderEngine::RenderFrame() {
    ID3D11DeviceContext* ctx = context->GetContext();
    ID3D11RenderTargetView* rtv = swapChain->GetBackBufferRTV();
    ID3D11DepthStencilView* dsv = swapChain->GetDepthStencilView();

    if (!ctx || !rtv || !dsv) {
        return;
    }

    // Clear render target and depth-stencil buffer
    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black background
    ctx->ClearRenderTargetView(rtv, clearColor);
    ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Set render targets (both color and depth-stencil)
    ctx->OMSetRenderTargets(1, &rtv, dsv);

    // Set viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    ctx->RSSetViewports(1, &viewport);

    // Get shader and render
    Shader* shader = shaderManager ? shaderManager->GetShader("default") : nullptr;

    if (!shader) {
        // Fallback: clear to magenta if shader loading failed
        float clearColor[] = { 0.8f, 0.2f, 0.8f, 1.0f };
        ctx->ClearRenderTargetView(rtv, clearColor);
    }
    else {
        // Set up shaders and render fullscreen quad
        ctx->IASetInputLayout(shader->inputLayout);
        ctx->VSSetShader(shader->vertexShader, nullptr, 0);
        ctx->PSSetShader(shader->pixelShader, nullptr, 0);

        if (fullscreenQuad) {
            fullscreenQuad->Render(ctx, colorConstantBuffer);
        }
        else {
            // Fallback: clear to yellow if quad failed
            float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
            ctx->ClearRenderTargetView(rtv, clearColor);
        }
    }

    swapChain->Present();
}

void RenderEngine::Shutdown() {
    if (fullscreenQuad) {
        delete fullscreenQuad;
        fullscreenQuad = nullptr;
    }

    if (colorConstantBuffer) {
        colorConstantBuffer->Release();
        colorConstantBuffer = nullptr;
    }

    delete renderer;
    delete commandQueue;
    delete swapChain;
    delete shaderManager;
    delete context;

    renderer = nullptr;
    commandQueue = nullptr;
    swapChain = nullptr;
    shaderManager = nullptr;
    context = nullptr;
}