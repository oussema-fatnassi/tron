#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>

struct Shader {
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
};

class ShaderManager {
public:
    bool LoadShader(ID3D11Device* device, const std::string& name, const std::wstring& vsFile, const std::wstring& psFile);
    Shader* GetShader(const std::string& name);
    void Clear();

private:
    std::unordered_map<std::string, Shader> shaders;
};
