#pragma once
#include <d3d11.h>

class D3DContext {
public:
    D3DContext();
    ~D3DContext();

    bool Initialize(HWND hwnd, int width, int height);

    ID3D11Device* GetDevice() const { return device; }
    ID3D11DeviceContext* GetContext() const { return context; }

private:
    ID3D11Device* device;
    ID3D11DeviceContext* context;
};
