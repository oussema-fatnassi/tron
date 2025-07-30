#include "../../../Headers/Rendering/Resources/MeshManager.hpp"
#include <iostream>

bool MeshManager::LoadMesh(ID3D11Device* device, const std::string& name, const std::string& filepath) {
    // TODO: Implement actual mesh loading from file (.obj, .fbx, etc.)
    // For now, this is a placeholder that creates empty mesh data

    std::cout << "[MeshManager] Loading mesh '" << name << "' from file: " << filepath << "\n";

    MeshData data = {};
    // ... Here you would implement actual file loading
    // This could use libraries like Assimp for .obj, .fbx, etc.

    meshes[name] = data;
    std::cout << "[MeshManager] Mesh '" << name << "' loaded successfully (placeholder)\n";
    return true;
}

bool MeshManager::AddPrimitiveMesh(const std::string& name, const MeshData& meshData) {
    if (HasMesh(name)) {
        std::cout << "[MeshManager] Warning: Mesh '" << name << "' already exists, replacing...\n";
        RemoveMesh(name);
    }

    meshes[name] = meshData;
    std::cout << "[MeshManager] Primitive mesh '" << name << "' added successfully\n";
    return true;
}

MeshData* MeshManager::GetMesh(const std::string& name) {
    auto it = meshes.find(name);
    if (it != meshes.end()) {
        return &it->second;
    }

    std::cout << "[MeshManager] Warning: Mesh '" << name << "' not found\n";
    return nullptr;
}

bool MeshManager::HasMesh(const std::string& name) const {
    return meshes.find(name) != meshes.end();
}

void MeshManager::RemoveMesh(const std::string& name) {
    auto it = meshes.find(name);
    if (it != meshes.end()) {
        // Release D3D11 resources
        it->second.Release();
        meshes.erase(it);
        std::cout << "[MeshManager] Mesh '" << name << "' removed\n";
    }
}

void MeshManager::Clear() {
    for (auto& [name, mesh] : meshes) {
        mesh.Release();
    }
    meshes.clear();
    std::cout << "[MeshManager] All meshes cleared\n";
}