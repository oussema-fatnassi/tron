#pragma once
#include "Component.hpp"
#include <string>
#include <d3d11.h>

// Enumeration for primitive mesh types
enum class PrimitiveMeshType {
    QUAD,
    CUBE,
    SPHERE,
    CYLINDER,
    PLANE,
    TRIANGLE,
    CUSTOM,
	RAINBOW_CUBE
};

// <summary>
// MeshRenderer component that holds mesh and material information for rendering entities.
// It can use either primitive meshes or custom loaded meshes.
// </summary>
// <remarks>
// This component is essential for the rendering system, allowing entities to be rendered
// with specific meshes and materials. It integrates with the existing rendering pipeline
// and supports both built-in primitive shapes and custom mesh loading.
// The component stores references to mesh data, shader information, and material properties.
// </remarks>
struct MeshRenderer : Component {
    // Mesh information
    PrimitiveMeshType primitiveType;
    std::string customMeshName;      // Name of custom mesh (if using CUSTOM type)
    std::string meshName;            // Final resolved mesh name for rendering

    // Material/Shader information
    std::string shaderName;          // Name of shader to use
    std::string materialName;        // Name of material to use (optional)

    // Rendering properties
    bool isVisible;                  // Whether this mesh should be rendered
    float alpha;                     // Transparency value (0.0 = transparent, 1.0 = opaque)

    // Color override (if no material is specified)
    float color[4];                  // RGBA color values

    // Constructor for primitive meshes
    MeshRenderer(PrimitiveMeshType type = PrimitiveMeshType::CUBE,
        const std::string& shader = "default")
        : primitiveType(type)
        , customMeshName("")
        , shaderName(shader)
        , materialName("")
        , isVisible(true)
        , alpha(1.0f)
    {
        // Set default white color
        color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;

        // Set mesh name based on primitive type
        SetMeshNameFromPrimitive();
    }

    // Constructor for custom meshes
    MeshRenderer(const std::string& customMesh, const std::string& shader = "default")
        : primitiveType(PrimitiveMeshType::CUSTOM)
        , customMeshName(customMesh)
        , meshName(customMesh)
        , shaderName(shader)
        , materialName("")
        , isVisible(true)
        , alpha(1.0f)
    {
        // Set default white color
        color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
    }

    // Set color (convenience method)
    void SetColor(float r, float g, float b, float a = 1.0f) {
        color[0] = r;
        color[1] = g;
        color[2] = b;
        color[3] = a;
        alpha = a;
    }

    // Set material
    void SetMaterial(const std::string& material) {
        materialName = material;
    }

    // Set shader
    void SetShader(const std::string& shader) {
        shaderName = shader;
    }

    // Get the final mesh name to use for rendering
    const std::string& GetMeshName() const {
        return meshName;
    }

private:
    void SetMeshNameFromPrimitive() {
        switch (primitiveType) {
        case PrimitiveMeshType::QUAD:
            meshName = "primitive_quad";
            break;
        case PrimitiveMeshType::CUBE:
            meshName = "primitive_cube";
            break;
        case PrimitiveMeshType::SPHERE:
            meshName = "primitive_sphere";
            break;
        case PrimitiveMeshType::CYLINDER:
            meshName = "primitive_cylinder";
            break;
        case PrimitiveMeshType::PLANE:
            meshName = "primitive_plane";
            break;
        case PrimitiveMeshType::TRIANGLE:
            meshName = "primitive_triangle";
            break;
        case PrimitiveMeshType::CUSTOM:
            meshName = customMeshName;
            break;
        case PrimitiveMeshType::RAINBOW_CUBE:
            meshName = "primitive_rainbow_cube";
            break;
        }
    }
};