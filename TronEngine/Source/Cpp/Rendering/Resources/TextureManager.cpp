#include "../../../Headers/Rendering/Resources/TextureManager.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <wincodec.h>

using Microsoft::WRL::ComPtr;

// Helper function to load a texture using WIC
static HRESULT LoadTextureFromFile(
    ID3D11Device* device,
    const std::wstring& fileName,
    ID3D11ShaderResourceView** textureView)
{
    ComPtr<IWICImagingFactory> wicFactory;
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr)) return hr;

    ComPtr<IWICBitmapDecoder> decoder;
    hr = wicFactory->CreateDecoderFromFilename(
        fileName.c_str(), nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) return hr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return hr;

    ComPtr<IWICFormatConverter> converter;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return hr;

    hr = converter->Initialize(
        frame.Get(), GUID_WICPixelFormat32bppRGBA,
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return hr;

    UINT width, height;
    hr = converter->GetSize(&width, &height);
    if (FAILED(hr)) return hr;

    std::vector<BYTE> pixels(width * height * 4);
    hr = converter->CopyPixels(
        nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data());
    if (FAILED(hr)) return hr;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixels.data();
    initData.SysMemPitch = width * 4;

    ComPtr<ID3D11Texture2D> texture;
    hr = device->CreateTexture2D(&desc, &initData, &texture);
    if (FAILED(hr)) return hr;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    return device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView);
}

bool TextureManager::LoadTexture(ID3D11Device* device, const std::string& name, const std::wstring& filepath) {
    ID3D11ShaderResourceView* srv = nullptr;
    HRESULT hr = LoadTextureFromFile(device, filepath, &srv);
    if (FAILED(hr)) return false;
    textures[name] = srv;
    return true;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(const std::string& name) {
    auto it = textures.find(name);
    return (it != textures.end()) ? it->second : nullptr;
}

void TextureManager::Clear() {
    for (auto& [_, tex] : textures) {
        if (tex) tex->Release();
    }
    textures.clear();
}
