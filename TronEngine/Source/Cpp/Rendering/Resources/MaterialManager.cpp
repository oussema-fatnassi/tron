#include "../../../Headers/Rendering/Resources/MaterialManager.hpp"

bool MaterialManager::CreateMaterial(const std::string& name, Shader* shader, ID3D11ShaderResourceView* texture) {
    materials[name] = { shader, texture };
    return true;
}

Material* MaterialManager::GetMaterial(const std::string& name) {
    auto it = materials.find(name);
    return (it != materials.end()) ? &it->second : nullptr;
}

void MaterialManager::Clear() {
    materials.clear(); // Les shaders et textures sont gérés ailleurs
}
