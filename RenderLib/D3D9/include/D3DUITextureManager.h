#pragma once

#include <AVUIIClientRendererInterface.h>

struct IDirect3DTexture9;
class UIRenderer;
class TextureManager;

class BitmapSourceData : public AVUI::IClientImageSource
{
    friend class TextureManager;
public:
    BitmapSourceData(TextureManager* pTextureManager, const WCHAR* wzImageName);

    float get_Height() { return m_height; }
    float get_Width() { return m_width; }
    void Destroy();

    void AddRef();

    IDirect3DTexture9* get_Texture() { return m_pTexture; }
    const WCHAR* get_ImageName() const { return m_imageName.get_Ptr(); }

private:

    void LoadImageData();

    void DeviceLost();
    void DeviceReset();

    int m_cRef;

    TextureManager* m_pTextureManager;

    AVUI::String m_imageName;
    IDirect3DTexture9* m_pTexture;
    float m_width;
    float m_height;
};

class TextureManager
{
    friend class BitmapSourceData;
public:
    TextureManager(UIRenderer* pRenderer, const WCHAR* wzTexturePath);
    ~TextureManager();

    AVUI::IClientImageSource* LoadImageSource(const WCHAR* wzImageName);

    void DeviceLost();
    void DeviceReset();

    const WCHAR* get_BasePath() const { return m_basePath.get_Ptr(); }

    IDirect3DDevice9* get_Device();

private:

    void RemoveSourceData(BitmapSourceData* pData);

    AVUI::ItemStructList<BitmapSourceData*> m_imageSourceData;
    AVUI::String m_basePath;
    UIRenderer* m_pRenderer;
};

