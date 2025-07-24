#pragma once
#include <unordered_map>
#include <string>
#include <d3d11.h>

struct MeshData {
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    UINT indexCount;
};

class MeshManager {
public:
    bool LoadMesh(ID3D11Device* device, const std::string& name, const std::string& filepath);
    MeshData* GetMesh(const std::string& name);
    void Clear();

private:
    std::unordered_map<std::string, MeshData> meshes;
};
