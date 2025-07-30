#pragma once
#include <string>
#include <vector>

// Forward declarations
struct Transform;

// Color structure for rendering
struct RenderColor {
    float r, g, b, a;

    RenderColor() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    RenderColor(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {
    }
};

// Transform data for rendering (copy of essential transform info)
struct RenderTransform {
    float position[3];    // x, y, z
    float rotation[3];    // rx, ry, rz (euler angles for now)
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

// Render command types
enum class RenderCommandType {
    DRAW_MESH,
    CLEAR_SCREEN,
    SET_VIEWPORT,
    BEGIN_FRAME,
    END_FRAME
};

// Main render command structure
struct RenderCommand {
    RenderCommandType type;

    // Mesh rendering data (used for DRAW_MESH)
    std::string meshName;
    std::string shaderName;
    std::string materialName;  // Optional
    RenderTransform transform;
    RenderColor color;
    bool visible;
    float alpha;

    // Screen clear data (used for CLEAR_SCREEN)
    RenderColor clearColor;

    // Default constructor
    RenderCommand()
        : type(RenderCommandType::DRAW_MESH)
        , visible(true)
        , alpha(1.0f) {
    }

    // Convenience constructor for mesh rendering
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

    // Convenience constructor for screen clear
    static RenderCommand CreateClearScreen(const RenderColor& col = RenderColor(0.0f, 0.0f, 0.0f, 1.0f)) {
        RenderCommand cmd;
        cmd.type = RenderCommandType::CLEAR_SCREEN;
        cmd.clearColor = col;
        return cmd;
    }
};