#pragma once

namespace AVUI { class IClientFont; }
class ClientFont;
class FontData;
class UIRenderer;
struct IDirect3DDevice9;

class FontManager
{

public:
    FontManager(UIRenderer* pRenderer, const WCHAR* wzFontPath);
    ~FontManager();

    AVUI::IClientFont* FindMatchingFont(const WCHAR* wzFace, float pixelSize);

    void DeviceLost();
    void DeviceReset();

private:
    FontData* FindClosestFontData(const WCHAR* wzFace, float pixelSize);

    AVUI::ItemStructList<FontData*> m_fontData;
    AVUI::ItemStructList<ClientFont*> m_clientFonts;
};
