#pragma once
#include <unordered_map>
#include <string>
#include "ShaderManager.h"
#include "TextureManager.h"

struct Material {
    Shader* shader;
    ID3D11ShaderResourceView* texture;
};

class MaterialManager {
public:
    bool CreateMaterial(const std::string& name, Shader* shader, ID3D11ShaderResourceView* texture);
    Material* GetMaterial(const std::string& name);
    void Clear();

private:
    std::unordered_map<std::string, Material> materials;
};
