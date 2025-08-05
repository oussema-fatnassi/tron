#pragma once
#include <string>
#include <vector>

// Forward declarations
struct Transform;

// Color structure for rendering (existing)
struct RenderColor {
    float r, g, b, a;

    RenderColor() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    RenderColor(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {
    }
};

// Transform data for rendering (existing)
struct RenderTransform {
    float position[3];    // x, y, z
    float rotation[3];    // rx, ry, rz (euler angles)
    float scale[3];       // sx, sy, sz

    RenderTransform() {
        position[0] = position[1] = position[2] = 0.0f;
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
        scale[0] = scale[1] = scale[2] = 1.0f;
    }

    RenderTransform(float x, float y, float z) {
        position[0] = x; position[1] = y; position[2] = z;
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
        scale[0] = scale[1] = scale[2] = 1.0f;
    }
};

// NEW: Matrix data for rendering (16 floats in column-major order)
struct RenderMatrix {
    float data[16];
    
    RenderMatrix() {
        // Initialize as identity matrix
        for (int i = 0; i < 16; ++i) data[i] = 0.0f;
        data[0] = data[5] = data[10] = data[15] = 1.0f; // Diagonal = 1
    }
    
    void SetFromMatrix(const class Matrix& matrix); // Implementation needed
};

// NEW: Camera matrices for rendering
struct RenderCameraMatrices {
    RenderMatrix worldMatrix;
    RenderMatrix viewMatrix;
    RenderMatrix projectionMatrix;
    RenderMatrix worldViewProjMatrix;
    
    bool hasValidMatrices;
    
    RenderCameraMatrices() : hasValidMatrices(false) {}
};

// Render command types (existing)
enum class RenderCommandType {
    DRAW_MESH,
    CLEAR_SCREEN,
    SET_VIEWPORT,
    BEGIN_FRAME,
    END_FRAME
};

// Main render command structure (EXTENDED)
struct RenderCommand {
    RenderCommandType type;

    // Mesh rendering data (existing)
    std::string meshName;
    std::string shaderName;
    std::string materialName;
    RenderTransform transform;
    RenderColor color;
    bool visible;
    float alpha;

    // Screen clear data (existing)
    RenderColor clearColor;

    // NEW: Camera matrices
    RenderCameraMatrices cameraMatrices;

    // Default constructor (existing)
    RenderCommand()
        : type(RenderCommandType::DRAW_MESH)
        , visible(true)
        , alpha(1.0f) {
    }

    // Existing convenience constructors...
    static RenderCommand CreateDrawMesh(
        const std::string& mesh,
        const std::string& shader,
        const RenderTransform& trans,
        const RenderColor& col = RenderColor(),
        const std::string& material = ""
    ) {
        RenderCommand cmd;
        cmd.type = RenderCommandType::DRAW_MESH;
        cmd.meshName = mesh;
        cmd.shaderName = shader;
        cmd.materialName = material;
        cmd.transform = trans;
        cmd.color = col;
        cmd.visible = true;
        cmd.alpha = col.a;
        return cmd;
    }

    // NEW: Enhanced constructor with matrices
    static RenderCommand CreateDrawMeshWithMatrices(
        const std::string& mesh,
        const std::string& shader,
        const RenderTransform& trans,
        const RenderCameraMatrices& matrices,
        const RenderColor& col = RenderColor(),
        const std::string& material = ""
    ) {
        RenderCommand cmd = CreateDrawMesh(mesh, shader, trans, col, material);
        cmd.cameraMatrices = matrices;
        return cmd;
    }

    static RenderCommand CreateClearScreen(const RenderColor& col = RenderColor(0.0f, 0.0f, 0.0f, 1.0f)) {
        RenderCommand cmd;
        cmd.type = RenderCommandType::CLEAR_SCREEN;
        cmd.clearColor = col;
        return cmd;
    }
};