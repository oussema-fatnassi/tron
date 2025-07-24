#include "../../../Headers/Rendering/Resources/MeshManager.hpp"

bool MeshManager::LoadMesh(ID3D11Device* device, const std::string& name, const std::string& filepath) {
    // TODO : Charger un fichier .obj ou .mesh custom (placeholder ici)
    MeshData data = {};
    // ... Remplir vertexBuffer et indexBuffer
    meshes[name] = data;
    return true;
}

MeshData* MeshManager::GetMesh(const std::string& name) {
    auto it = meshes.find(name);
    return (it != meshes.end()) ? &it->second : nullptr;
}

void MeshManager::Clear() {
    for (auto& [_, mesh] : meshes) {
        if (mesh.vertexBuffer) mesh.vertexBuffer->Release();
        if (mesh.indexBuffer) mesh.indexBuffer->Release();
    }
    meshes.clear();
}
