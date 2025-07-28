#include "../../Headers/Rendering/FullscreenQuad.hpp"

FullscreenQuad::FullscreenQuad() 
    : vertexBuffer(nullptr), vertexCount(0) {
}

FullscreenQuad::~FullscreenQuad() {
    Shutdown();
}

bool FullscreenQuad::Initialize(ID3D11Device* device) {
    if (!device) {
        return false;
    }

    CreateVertexBuffer(device);
    return vertexBuffer != nullptr;
}

void FullscreenQuad::CreateVertexBuffer(ID3D11Device* device) {
    // Fullscreen quad vertices in NDC space (-1 to 1)
    struct Vertex {
        float pos[3];    // Position
        float color[3];  // Color (required by vertex shader)
    };

    Vertex vertices[] = {
        // Triangle 1 (bottom-left, top-left, top-right)
        { {-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },  // Bottom Left
        { {-1.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },  // Top Left
        { { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },  // Top Right
        
        // Triangle 2 (bottom-left, top-right, bottom-right)
        { {-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },  // Bottom Left
        { { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },  // Top Right
        { { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }   // Bottom Right
    };

    vertexCount = 6; // 2 triangles = 6 vertices

    // Create vertex buffer description
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    // Initial data for the buffer
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    // Create the vertex buffer
    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr)) {
        vertexBuffer = nullptr;
    }
}

void FullscreenQuad::Render(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer) {
    if (!context || !vertexBuffer) {
        return;
    }

    // Bind constant buffer if provided (for ColorBuffer at register b1)
    if (constantBuffer) {
        context->PSSetConstantBuffers(1, 1, &constantBuffer);
    }

    // Set vertex buffer
    UINT stride = sizeof(float) * 6; // 3 for position + 3 for color
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    
    // Set primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Draw the fullscreen quad
    context->Draw(vertexCount, 0);
}

void FullscreenQuad::Shutdown() {
    if (vertexBuffer) {
        vertexBuffer->Release();
        vertexBuffer = nullptr;
    }
    vertexCount = 0;
}