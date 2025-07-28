#include "../../../Headers/Rendering/Resources/ShaderManager.hpp"
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <iostream>

using Microsoft::WRL::ComPtr;

bool ShaderManager::LoadShader(ID3D11Device* device, const std::string& name, const std::wstring& vsFile, const std::wstring& psFile) {
    if (!device) {
        std::cout << "[ShaderManager] Error: Device is null\n";
        return false;
    }

    Shader shader = {};

    // === Vertex Shader ===
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> errorBlob;
    
    HRESULT hr = D3DCompileFromFile(
        vsFile.c_str(),
        nullptr, 
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", 
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        vsBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to compile vertex shader: " << std::hex << hr << std::dec << "\n";
        if (errorBlob) {
            std::cout << "[ShaderManager] Vertex shader error: " << (char*)errorBlob->GetBufferPointer() << "\n";
        }
        return false;
    }

    hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &shader.vertexShader
    );
    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to create vertex shader object: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    // === Input Layout ===
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = device->CreateInputLayout(
        inputDesc,
        ARRAYSIZE(inputDesc),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &shader.inputLayout
    );
    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to create input layout: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    // === Pixel Shader ===
    ComPtr<ID3DBlob> psBlob;
    hr = D3DCompileFromFile(
        psFile.c_str(),
        nullptr, 
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", 
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        psBlob.GetAddressOf(),
        errorBlob.ReleaseAndGetAddressOf()
    );

    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to compile pixel shader: " << std::hex << hr << std::dec << "\n";
        if (errorBlob) {
            std::cout << "[ShaderManager] Pixel shader error: " << (char*)errorBlob->GetBufferPointer() << "\n";
        }
        return false;
    }

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &shader.pixelShader
    );
    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to create pixel shader object: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    // Store the shader
    shaders[name] = shader;
    std::cout << "[ShaderManager] Shader '" << name << "' loaded successfully\n";
    return true;
}

Shader* ShaderManager::GetShader(const std::string& name) {
    auto it = shaders.find(name);
    return (it != shaders.end()) ? &it->second : nullptr;
}

void ShaderManager::Clear() {
    for (auto& [name, shader] : shaders) {
        if (shader.vertexShader) shader.vertexShader->Release();
        if (shader.pixelShader) shader.pixelShader->Release();
        if (shader.inputLayout) shader.inputLayout->Release();
    }
    shaders.clear();
}