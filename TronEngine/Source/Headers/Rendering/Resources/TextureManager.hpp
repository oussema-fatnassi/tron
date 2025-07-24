#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>

class TextureManager {
public:
    bool LoadTexture(ID3D11Device* device, const std::string& name, const std::wstring& filepath);
    ID3D11ShaderResourceView* GetTexture(const std::string& name);
    void Clear();

private:
    std::unordered_map<std::string, ID3D11ShaderResourceView*> textures;
};
