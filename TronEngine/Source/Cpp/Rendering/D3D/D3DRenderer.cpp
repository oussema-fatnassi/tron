#include "../../../Headers/Rendering/D3D/D3DRenderer.hpp"

D3DRenderer::D3DRenderer() : context(nullptr), swapChain(nullptr), commandQueue(nullptr) {}

D3DRenderer::~D3DRenderer() {}

void D3DRenderer::Initialize(D3DContext* ctx, SwapChain* sc, CommandQueue* queue) {
    context = ctx;
    swapChain = sc;
    commandQueue = queue;
}

void D3DRenderer::Render() {
    ID3D11DeviceContext* ctx = context->GetContext();
    ID3D11RenderTargetView* rtv = swapChain->GetBackBufferRTV();

    float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    ctx->ClearRenderTargetView(rtv, clearColor);

    ctx->OMSetRenderTargets(1, &rtv, nullptr);

    // Ici, tu boucles sur commandQueue pour dessiner (à faire plus tard).

    // Fin : rendu fait
}

void D3DRenderer::DrawMesh(ID3D11DeviceContext* context, const MeshData& mesh, const Material& material) {
    if (!context) return;

    // 1. Set input layout
    context->IASetInputLayout(material.shader->inputLayout);

    // 2. Set shaders
    context->VSSetShader(material.shader->vertexShader, nullptr, 0);
    context->PSSetShader(material.shader->pixelShader, nullptr, 0);

    // 3. Set texture (si utilisée)
    if (material.texture)
        context->PSSetShaderResources(0, 1, &material.texture);

    // 4. Set vertex and index buffers
    UINT stride = sizeof(float) * 6; // pos(3) + color(3)
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 5. Draw
    context->DrawIndexed(mesh.indexCount, 0, 0);
}

