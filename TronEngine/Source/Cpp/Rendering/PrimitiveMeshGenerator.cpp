#include "../../Headers/Rendering/PrimitiveMeshGenerator.hpp"
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool PrimitiveMeshGenerator::GenerateAllPrimitives(ID3D11Device* device, MeshManager* meshManager) {
    std::cout << "[PrimitiveMeshGenerator] Generating all primitive meshes...\n";

    bool success = true;
    success &= GenerateQuad(device, meshManager);
    success &= GenerateCube(device, meshManager);
    success &= GenerateCylinder(device, meshManager);
    success &= GeneratePlane(device, meshManager);
    success &= GenerateTriangle(device, meshManager);
    success &= GenerateSphere(device, meshManager);

    if (success) {
        std::cout << "[PrimitiveMeshGenerator] All primitive meshes generated successfully!\n";
    }
    else {
        std::cout << "[PrimitiveMeshGenerator] Some primitive meshes failed to generate!\n";
    }

    return success;
}

bool PrimitiveMeshGenerator::GenerateQuad(ID3D11Device* device, MeshManager* meshManager) {
    std::vector<Vertex> vertices = {
        // Front face quad (Z = 0)
        { {-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} }, // Bottom Left
        { {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} }, // Top Left
        { { 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} }, // Top Right
        { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f} }  // Bottom Right
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    MeshData meshData = {};
    if (CreateMeshBuffers(device, vertices, indices, meshData)) {
        return meshManager->AddPrimitiveMesh("primitive_quad", meshData);
    }
    return false;
}

bool PrimitiveMeshGenerator::GenerateCube(ID3D11Device* device, MeshManager* meshManager) {
    std::vector<Vertex> vertices = {
        // Front face - Red to Yellow gradient
        { {-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f} }, // 0 - Red (bottom left)
        { {-0.5f,  0.5f,  0.5f}, {1.0f, 0.5f, 0.0f} }, // 1 - Orange (top left)
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f} }, // 2 - Yellow (top right)
        { { 0.5f, -0.5f,  0.5f}, {1.0f, 0.5f, 0.0f} }, // 3 - Orange (bottom right)
        
        // Back face - Blue to Purple gradient
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f} }, // 4 - Blue (bottom left)
        { {-0.5f,  0.5f, -0.5f}, {0.5f, 0.0f, 1.0f} }, // 5 - Purple (top left)
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f} }, // 6 - Magenta (top right)
        { { 0.5f, -0.5f, -0.5f}, {0.5f, 0.0f, 1.0f} }, // 7 - Purple (bottom right)
        
        // Left face - Green gradient
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.5f, 0.0f} }, // 8 - Dark Green
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} }, // 9 - Bright Green
        { {-0.5f,  0.5f,  0.5f}, {0.5f, 1.0f, 0.5f} }, // 10 - Light Green
        { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.8f, 0.0f} }, // 11 - Medium Green
        
        // Right face - Cyan to White gradient
        { { 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f} }, // 12 - Cyan
        { { 0.5f,  0.5f,  0.5f}, {0.5f, 1.0f, 1.0f} }, // 13 - Light Cyan
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f} }, // 14 - White
        { { 0.5f, -0.5f, -0.5f}, {0.8f, 0.8f, 1.0f} }, // 15 - Light Blue
        
        // Top face - Rainbow gradient
        { {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 1.0f} }, // 16 - Magenta
        { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 1.0f} }, // 17 - Cyan
        { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.0f} }, // 18 - Yellow
        { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.5f, 0.0f} }, // 19 - Orange
        
        // Bottom face - Dark rainbow gradient
        { {-0.5f, -0.5f, -0.5f}, {0.5f, 0.0f, 0.5f} }, // 20 - Dark Magenta
        { {-0.5f, -0.5f,  0.5f}, {0.8f, 0.0f, 0.0f} }, // 21 - Dark Red
        { { 0.5f, -0.5f,  0.5f}, {0.0f, 0.5f, 0.8f} }, // 22 - Dark Blue
        { { 0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.0f} }  // 23 - Dark Yellow
    };
    
    // Indices remain the same
    std::vector<uint32_t> indices = {
        // Front face
        0, 1, 2,  0, 2, 3,
        // Back face
        4, 6, 5,  4, 7, 6,
        // Left face
        8, 9, 10,  8, 10, 11,
        // Right face
        12, 14, 13,  12, 15, 14,
        // Top face
        16, 17, 18,  16, 18, 19,
        // Bottom face
        20, 22, 21,  20, 23, 22
    };
    
    MeshData meshData = {};
    if (CreateMeshBuffers(device, vertices, indices, meshData)) {
        return meshManager->AddPrimitiveMesh("primitive_cube", meshData);
    }
    return false;
}

bool PrimitiveMeshGenerator::GenerateTriangle(ID3D11Device* device, MeshManager* meshManager) {
    std::vector<Vertex> vertices = {
        { { 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} }, // Top
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} }, // Bottom Left
        { { 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} }  // Bottom Right
    };

    std::vector<uint32_t> indices = { 0, 1, 2 };

    MeshData meshData = {};
    if (CreateMeshBuffers(device, vertices, indices, meshData)) {
        return meshManager->AddPrimitiveMesh("primitive_triangle", meshData);
    }
    return false;
}

bool PrimitiveMeshGenerator::GeneratePlane(ID3D11Device* device, MeshManager* meshManager) {
    // Plane is essentially a quad lying flat (Y = 0)
    std::vector<Vertex> vertices = {
        { {-1.0f, 0.0f, -1.0f}, {0.5f, 0.5f, 0.5f} }, // Bottom Left
        { {-1.0f, 0.0f,  1.0f}, {0.5f, 0.5f, 0.5f} }, // Top Left
        { { 1.0f, 0.0f,  1.0f}, {0.5f, 0.5f, 0.5f} }, // Top Right
        { { 1.0f, 0.0f, -1.0f}, {0.5f, 0.5f, 0.5f} }  // Bottom Right
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    MeshData meshData = {};
    if (CreateMeshBuffers(device, vertices, indices, meshData)) {
        return meshManager->AddPrimitiveMesh("primitive_plane", meshData);
    }
    return false;
}

bool PrimitiveMeshGenerator::GenerateSphere(ID3D11Device* device, MeshManager* meshManager, int segments) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    GenerateSphereVertices(vertices, indices, segments);

    MeshData meshData = {};
    if (CreateMeshBuffers(device, vertices, indices, meshData)) {
        return meshManager->AddPrimitiveMesh("primitive_sphere", meshData);
    }
    return false;
}

bool PrimitiveMeshGenerator::GenerateCylinder(ID3D11Device* device, MeshManager* meshManager, int segments) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    GenerateCylinderVertices(vertices, indices, segments);

    MeshData meshData = {};
    if (CreateMeshBuffers(device, vertices, indices, meshData)) {
        return meshManager->AddPrimitiveMesh("primitive_cylinder", meshData);
    }
    return false;
}

void PrimitiveMeshGenerator::GenerateSphereVertices(std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices,
    int segments) {
    const float radius = 0.5f;

    // Generate vertices
    for (int i = 0; i <= segments; ++i) {
        float theta = static_cast<float>(i) * M_PI / segments; // 0 to PI

        for (int j = 0; j <= segments; ++j) {
            float phi = static_cast<float>(j) * 2.0f * M_PI / segments; // 0 to 2*PI

            float x = radius * sin(theta) * cos(phi);
            float y = radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);

            // Color based on position for variety
            float r = (x + radius) / (2.0f * radius);
            float g = (y + radius) / (2.0f * radius);
            float b = (z + radius) / (2.0f * radius);

            vertices.push_back({ {x, y, z}, {r, g, b} });
        }
    }

    // Generate indices
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;

            // First triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Second triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

void PrimitiveMeshGenerator::GenerateCylinderVertices(std::vector<Vertex>& vertices,
    std::vector<uint32_t>& indices,
    int segments) {
    const float radius = 0.5f;
    const float height = 1.0f;
    const float halfHeight = height * 0.5f;

    // Generate side vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = static_cast<float>(i) * 2.0f * M_PI / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Top vertex
        vertices.push_back({ {x, halfHeight, z}, {1.0f, 0.0f, 0.0f} });
        // Bottom vertex
        vertices.push_back({ {x, -halfHeight, z}, {0.0f, 0.0f, 1.0f} });
    }

    // Generate side indices
    for (int i = 0; i < segments; ++i) {
        int topCurrent = i * 2;
        int bottomCurrent = i * 2 + 1;
        int topNext = ((i + 1) % segments) * 2;
        int bottomNext = ((i + 1) % segments) * 2 + 1;

        // First triangle
        indices.push_back(topCurrent);
        indices.push_back(bottomCurrent);
        indices.push_back(topNext);

        // Second triangle
        indices.push_back(topNext);
        indices.push_back(bottomCurrent);
        indices.push_back(bottomNext);
    }

    // Add center vertices for caps
    int topCenter = vertices.size();
    vertices.push_back({ {0.0f, halfHeight, 0.0f}, {0.0f, 1.0f, 0.0f} });

    int bottomCenter = vertices.size();
    vertices.push_back({ {0.0f, -halfHeight, 0.0f}, {0.0f, 1.0f, 0.0f} });

    // Generate cap indices
    for (int i = 0; i < segments; ++i) {
        int topCurrent = i * 2;
        int bottomCurrent = i * 2 + 1;
        int topNext = ((i + 1) % segments) * 2;
        int bottomNext = ((i + 1) % segments) * 2 + 1;

        // Top cap
        indices.push_back(topCenter);
        indices.push_back(topNext);
        indices.push_back(topCurrent);

        // Bottom cap
        indices.push_back(bottomCenter);
        indices.push_back(bottomCurrent);
        indices.push_back(bottomNext);
    }
}

bool PrimitiveMeshGenerator::CreateMeshBuffers(ID3D11Device* device,
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    MeshData& outMeshData) {
    // Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &outMeshData.vertexBuffer);
    if (FAILED(hr)) {
        std::cout << "[PrimitiveMeshGenerator] Failed to create vertex buffer: " << std::hex << hr << std::dec << "\n";
        return false;
    }

    // Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t));
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    hr = device->CreateBuffer(&indexBufferDesc, &indexData, &outMeshData.indexBuffer);
    if (FAILED(hr)) {
        std::cout << "[PrimitiveMeshGenerator] Failed to create index buffer: " << std::hex << hr << std::dec << "\n";
        outMeshData.vertexBuffer->Release();
        outMeshData.vertexBuffer = nullptr;
        return false;
    }

    outMeshData.indexCount = static_cast<UINT>(indices.size());
    return true;
}
