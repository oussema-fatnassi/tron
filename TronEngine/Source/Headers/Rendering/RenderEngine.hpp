#pragma once

#include "D3D/D3DContext.hpp"
#include "D3D/D3DRenderer.hpp"
#include "D3D/SwapChain.hpp"
#include "D3D/CommandQueue.hpp"
#include "Resources/ShaderManager.hpp"
#include <iostream>
#include <string>

class RenderEngine {
public:
    RenderEngine(HWND hwnd, int width, int height);
    ~RenderEngine();

    void Initialize();
    void Shutdown();

    // Shader management
    bool LoadShader(const std::string& name, const std::wstring& vsFile, const std::wstring& psFile);
    Shader* GetShader(const std::string& name);

    // Frame rendering
    void BeginFrame();
    void EndFrame();

    // Resource access
    ID3D11DeviceContext* GetDeviceContext() const;
    ID3D11Device* GetDevice() const;
    ID3D11Buffer* GetColorConstantBuffer() const;

private:
    HWND hwnd;
    int width, height;

    D3DContext* context;
    SwapChain* swapChain;
    D3DRenderer* renderer;
    CommandQueue* commandQueue;
    ShaderManager* shaderManager;

    // Default constant buffer for testing
    ID3D11Buffer* colorConstantBuffer;

    void CreateConstantBuffer();
};