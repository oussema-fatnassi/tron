#include "../../../Headers/Rendering/Resources/ShaderManager.hpp"
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <fstream>

using Microsoft::WRL::ComPtr;

bool ShaderManager::LoadShader(ID3D11Device* device, const std::string& name, const std::wstring& vsFile, const std::wstring& psFile) {
    Shader shader = {};

    // === Vertex Shader ===
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DCompileFromFile(
        vsFile.c_str(),
        nullptr, nullptr,
        "main", "vs_5_0",
        0, 0,
        vsBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return false;
    }

    hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &shader.vertexShader
    );
    if (FAILED(hr)) return false;

    // === Input Layout (exemple simple pos + color) ===
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
    if (FAILED(hr)) return false;

    // === Pixel Shader ===
    ComPtr<ID3DBlob> psBlob;
    hr = D3DCompileFromFile(
        psFile.c_str(),
        nullptr, nullptr,
        "main", "ps_5_0",
        0, 0,
        psBlob.GetAddressOf(),
        errorBlob.ReleaseAndGetAddressOf()
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return false;
    }

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &shader.pixelShader
    );
    if (FAILED(hr)) return false;

    shaders[name] = shader;
    return true;
}

Shader* ShaderManager::GetShader(const std::string& name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return &it->second;
    }
    return nullptr;
}

void ShaderManager::Clear() {
    for (auto& [name, shader] : shaders) {
        if (shader.vertexShader)  shader.vertexShader->Release();
        if (shader.pixelShader)   shader.pixelShader->Release();
        if (shader.inputLayout)   shader.inputLayout->Release();
    }
    shaders.clear();
}

