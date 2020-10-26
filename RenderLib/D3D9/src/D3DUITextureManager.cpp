#include <Windows.h>
#include <wchar.h>
#include <AVUICommon.h>
#include <D3DUIRenderer.h>
#include <D3DUITextureManager.h>


TextureManager::TextureManager(UIRenderer* pUIRenderer, const WCHAR* wzTexturePath) : m_pRenderer(pUIRenderer), m_basePath(wzTexturePath)
{
}

TextureManager::~TextureManager()
{
}


AVUI::IClientImageSource* TextureManager::LoadImageSource(const WCHAR* wzImageName)
{
    for(int idx = 0; idx < m_imageSourceData.get_Count(); idx++)
    {
        if(!_wcsicmp(wzImageName, m_imageSourceData[idx]->get_ImageName()))
        {
            m_imageSourceData[idx]->AddRef();
            return m_imageSourceData[idx];
        }
    }

    m_imageSourceData.AddItem(new BitmapSourceData(this, wzImageName));

    return m_imageSourceData[m_imageSourceData.get_Count() - 1];
}

void TextureManager::DeviceLost()
{
    for(int idx = 0; idx < m_imageSourceData.get_Count(); idx++)
    {
        m_imageSourceData[idx]->DeviceLost();
    }
}

void TextureManager::DeviceReset()
{
    for(int idx = 0; idx < m_imageSourceData.get_Count(); idx++)
    {
        m_imageSourceData[idx]->DeviceReset();
    }
}

IDirect3DDevice9* TextureManager::get_Device()
{
    return m_pRenderer->get_Device();
}

void TextureManager::RemoveSourceData(BitmapSourceData* pData)
{
    m_imageSourceData.Remove(pData);
}


BitmapSourceData::BitmapSourceData(TextureManager* pTextureManager, const WCHAR* wzImageName) : 
                                                                                    m_pTextureManager(pTextureManager), m_imageName(wzImageName), m_cRef(1)
{
    LoadImageData();
}

void BitmapSourceData::LoadImageData()
{
    D3DXIMAGE_INFO imageInfo;
    HRESULT hr = S_OK;
    WCHAR wzFullImagePath[MAX_PATH];


    swprintf_s(wzFullImagePath, sizeof(wzFullImagePath) / sizeof(WCHAR), L"%ws\\%ws", m_pTextureManager->get_BasePath(), get_ImageName());


    hr = D3DXCreateTextureFromFileExW(m_pTextureManager->get_Device(), wzFullImagePath, 
                                          D3DX_DEFAULT,
                                          D3DX_DEFAULT, 
                                          D3DX_DEFAULT,
                                          D3DUSAGE_DYNAMIC,
                                          D3DFMT_A8R8G8B8,
                                          D3DPOOL_DEFAULT,
                                          D3DX_DEFAULT | D3DX_FILTER_SRGB_IN,
                                          D3DX_DEFAULT | D3DX_FILTER_SRGB_IN,
                                          0,
                                          &imageInfo,
                                          NULL,
                                          &m_pTexture);

    if(FAILED(hr))
    {
        m_pTexture = NULL;
        m_width = 0;
        m_height = 0;
    }
    else
    {
        m_width = (float) imageInfo.Width;
        m_height = (float) imageInfo.Height;
    }
}

void BitmapSourceData::DeviceLost()
{
    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }
}

void BitmapSourceData::DeviceReset()
{
    LoadImageData();
}

void BitmapSourceData::Destroy()
{
    m_cRef--;
    if(m_cRef == 0)
    {
        m_pTextureManager->RemoveSourceData(this);
        delete this;
    }
}

void BitmapSourceData::AddRef()
{
    m_cRef++;
}




