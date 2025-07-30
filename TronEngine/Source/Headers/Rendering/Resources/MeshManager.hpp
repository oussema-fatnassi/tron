#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>

struct MeshData {
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    UINT indexCount;

    // Constructor
    MeshData() : vertexBuffer(nullptr), indexBuffer(nullptr), indexCount(0) {}

    // Destructor helper
    void Release() {
        if (vertexBuffer) {
            vertexBuffer->Release();
            vertexBuffer = nullptr;
        }
        if (indexBuffer) {
            indexBuffer->Release();
            indexBuffer = nullptr;
        }
        indexCount = 0;
    }
};

class MeshManager {
public:
    MeshManager() = default;
    ~MeshManager() { Clear(); }

    // Load mesh from file (existing functionality)
    bool LoadMesh(ID3D11Device* device, const std::string& name, const std::string& filepath);

    // Add primitive mesh (new functionality for generated meshes)
    bool AddPrimitiveMesh(const std::string& name, const MeshData& meshData);

    // Get mesh by name
    MeshData* GetMesh(const std::string& name);

    // Check if mesh exists
    bool HasMesh(const std::string& name) const;

    // Remove specific mesh
    void RemoveMesh(const std::string& name);

    // Clear all meshes
    void Clear();

    // Get mesh count
    size_t GetMeshCount() const { return meshes.size(); }

private:
    std::unordered_map<std::string, MeshData> meshes;
};