#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include "../Rendering/Resources/MeshManager.hpp"

// Vertex structure that matches your shader input layout
struct Vertex {
    float position[3];  // Position (x, y, z)
    float color[3];     // Color (r, g, b)
};

// <summary>
// PrimitiveMeshGenerator class responsible for creating built-in primitive meshes.
// It generates vertex and index data for common 3D shapes and integrates with MeshManager.
// </summary>
// <remarks>
// This class provides functionality to generate standard 3D primitives like cubes, spheres,
// cylinders, etc. All generated meshes use the same vertex format as your existing shaders.
// The meshes are automatically registered with the MeshManager for use by MeshRenderer components.
// </remarks>
class PrimitiveMeshGenerator {
public:
    // Generate all primitive meshes and register them with the MeshManager
    static bool GenerateAllPrimitives(ID3D11Device* device, MeshManager* meshManager);

    // Individual primitive generation methods
    static bool GenerateQuad(ID3D11Device* device, MeshManager* meshManager);
    static bool GenerateCube(ID3D11Device* device, MeshManager* meshManager);
    static bool GenerateSphere(ID3D11Device* device, MeshManager* meshManager, int segments = 16);
    static bool GenerateCylinder(ID3D11Device* device, MeshManager* meshManager, int segments = 16);
    static bool GeneratePlane(ID3D11Device* device, MeshManager* meshManager);
    static bool GenerateTriangle(ID3D11Device* device, MeshManager* meshManager);

private:
    // Helper method to create D3D11 buffers from vertex/index data
    static bool CreateMeshBuffers(ID3D11Device* device,
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        MeshData& outMeshData);

    // Helper method to generate sphere vertices
    static void GenerateSphereVertices(std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices,
        int segments);

    // Helper method to generate cylinder vertices
    static void GenerateCylinderVertices(std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices,
        int segments);
};