#include "../../../Headers/Rendering/Resources/ShaderManager.hpp"
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <iostream>
#include <fstream>
#include <vector>

using Microsoft::WRL::ComPtr;

// Helper function to load compiled shader bytecode from .cso file
std::vector<char> LoadCompiledShader(const std::wstring& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        return {};
    }

    return buffer;
}

bool ShaderManager::LoadShader(ID3D11Device* device, const std::string& name, const std::wstring& vsFile, const std::wstring& psFile) {
    std::cout << "[ShaderManager] Loading shader: " << name << "\n";

    if (!device) {
        std::cout << "[ShaderManager] Error: Device is null\n";
        return false;
    }

    // Try to load pre-compiled bytecode first (.cso files)
    std::wstring vsCsoFile = vsFile;
    std::wstring psCsoFile = psFile;

    // Replace .hlsl extension with .cso
    size_t vsPos = vsCsoFile.find(L".hlsl");
    if (vsPos != std::wstring::npos) {
        vsCsoFile.replace(vsPos, 5, L".cso");
    }

    size_t psPos = psCsoFile.find(L".hlsl");
    if (psPos != std::wstring::npos) {
        psCsoFile.replace(psPos, 5, L".cso");
    }

    std::wcout << L"[ShaderManager] Trying compiled bytecode:\n";
    std::wcout << L"  VS: " << vsCsoFile << L"\n";
    std::wcout << L"  PS: " << psCsoFile << L"\n";

    // Try to load compiled bytecode
    auto vsData = LoadCompiledShader(vsCsoFile);
    auto psData = LoadCompiledShader(psCsoFile);

    if (!vsData.empty() && !psData.empty()) {
        std::cout << "[ShaderManager] Found compiled bytecode, loading from .cso files\n";
        return LoadShaderFromBytecode(device, name, vsData, psData);
    }
    else {
        std::cout << "[ShaderManager] No compiled bytecode found, falling back to runtime compilation\n";
        std::wcout << L"  VS bytecode size: " << vsData.size() << L" bytes\n";
        std::wcout << L"  PS bytecode size: " << psData.size() << L" bytes\n";
        return LoadShaderFromSource(device, name, vsFile, psFile);
    }
}

bool ShaderManager::LoadShaderFromBytecode(ID3D11Device* device, const std::string& name,
    const std::vector<char>& vsData, const std::vector<char>& psData) {
    Shader shader = {};

    // === Create Vertex Shader from bytecode ===
    HRESULT hr = device->CreateVertexShader(
        vsData.data(),
        vsData.size(),
        nullptr,
        &shader.vertexShader
    );
    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to create vertex shader from bytecode: " << std::hex << hr << std::dec << "\n";
        return false;
    }
    std::cout << "[ShaderManager] Vertex shader created from bytecode\n";

    // === Create Input Layout from vertex shader bytecode ===
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = device->CreateInputLayout(
        inputDesc,
        ARRAYSIZE(inputDesc),
        vsData.data(),
        vsData.size(),
        &shader.inputLayout
    );
    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to create input layout from bytecode: " << std::hex << hr << std::dec << "\n";
        return false;
    }
    std::cout << "[ShaderManager] Input layout created from bytecode\n";

    // === Create Pixel Shader from bytecode ===
    hr = device->CreatePixelShader(
        psData.data(),
        psData.size(),
        nullptr,
        &shader.pixelShader
    );
    if (FAILED(hr)) {
        std::cout << "[ShaderManager] Failed to create pixel shader from bytecode: " << std::hex << hr << std::dec << "\n";
        return false;
    }
    std::cout << "[ShaderManager] Pixel shader created from bytecode\n";

    // Store the shader
    shaders[name] = shader;
    std::cout << "[ShaderManager] Shader '" << name << "' loaded successfully from compiled bytecode!\n";
    return true;
}

bool ShaderManager::LoadShaderFromSource(ID3D11Device* device, const std::string& name,
    const std::wstring& vsFile, const std::wstring& psFile) {
    std::cout << "[ShaderManager] Compiling shaders from source files\n";

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
    std::cout << "[ShaderManager] Shader '" << name << "' loaded successfully from source compilation!\n";
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