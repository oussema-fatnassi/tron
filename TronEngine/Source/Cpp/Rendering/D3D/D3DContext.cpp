#include "../../../Headers/Rendering/D3D/D3DContext.h"
#include <stdexcept>

D3DContext::D3DContext() : device(nullptr), context(nullptr) {}

D3DContext::~D3DContext() {
    if (context) context->Release();
    if (device) device->Release();
}

bool D3DContext::Initialize(HWND hwnd, int width, int height) {
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &context
    );

    return SUCCEEDED(hr);
}
