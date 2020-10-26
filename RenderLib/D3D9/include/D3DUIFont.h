#pragma once

#include <AVUIIClientRendererInterface.h>
#include <D3DX9.h>

class UIRenderer;

class FontData
{
public:
    FontData(UIRenderer* pUIRenderer, const WCHAR* wzFileName);
    ~FontData();
    IDirect3DTexture9* GetTexture() { return m_pTexture; }

    void GetGlyphTexCoords(WCHAR wch, float* pU1, float* pV1, float* pU2, float* pV2);


    float GetCharacterWidth(WCHAR wch);
    void GetGlyphInfo(WCHAR wch, float* pLeadIn, float* pLeadOut, float* pGlyphWidth);
    float GetFontAdvance();

    const WCHAR* get_FontName() const { return m_fontData.wzFontName; }

    float GetFontSize() { return m_fontData.fontSize; }

    void DeviceLost();
    void DeviceReset();

private:

    void LoadFontData();
    void LoadFontTexture();

#pragma pack(push, 1)
    typedef struct
    {
        WCHAR wzFontName[64];
        float fontSize;
        float cellHeight;
        int beginGlyph;
        int endGlyph;
        int unknownGlyph;
    } FTD_HEADER;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct
    {
        int leadIn;
        int glyphWidth;
        int leadOut;
        float U1;
        float V1;
        float U2;
        float V2;
    } FTD_ENTRY;
#pragma pack(pop)

    FTD_ENTRY* GetGlyphEntry(WCHAR wch);

    UIRenderer* m_pUIRenderer;
    IDirect3DTexture9* m_pTexture;

    FTD_HEADER m_fontData;
    FTD_ENTRY* m_rgGlyphEntries;

    WCHAR m_wzFileBase[MAX_PATH];
};



class ClientFont : public AVUI::IClientFont
{
public:
    ClientFont(FontData* pFontData, const WCHAR* wzFontName, float pixelSize);
    AVUI::IClientFormattedText* CreateFormattedText(const WCHAR *pwchText, unsigned int cchText, float availableWidth, bool wrapText);

    virtual void Destroy();
    virtual float GetCharacterWidth(WCHAR wch);
    void GetGlyphInfo(WCHAR wch, float* pLeadIn, float* pLeadOut, float* pGlyphWidth);
    float GetFontAdvance();

    const WCHAR* get_FontName() const { return m_wzClientFontName; }

    float GetFontSize() { return m_pixelSize; }

    FontData* GetFontData() { return m_pFontData; }

private:

    FontData* m_pFontData;
    float m_scaleRatio;
    float m_pixelSize;

    static const int FONT_NAME_MAX = 64;
    WCHAR m_wzClientFontName[FONT_NAME_MAX];
};

