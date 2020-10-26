#include <Windows.h>
#include <AVUICommon.h>
#include <D3DUIFont.h>
#include <D3DUIFormattedText.h>
#include <D3DUIRenderer.h>
#include <D3DX9.h>
#include <wchar.h>

using namespace AVUI;

FontData::FontData(UIRenderer* pUIRenderer, const WCHAR* wzFontName) : m_pUIRenderer(pUIRenderer)
{
    wcscpy_s(m_wzFileBase, sizeof(m_wzFileBase) / sizeof(WCHAR), wzFontName);

    LoadFontTexture();

    LoadFontData();
}

FontData::~FontData()
{
    delete[] m_rgGlyphEntries;

    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
    }
}

void FontData::LoadFontTexture()
{
    WCHAR wzFile[MAX_PATH];

    swprintf_s(wzFile, sizeof(wzFile) / sizeof(WCHAR), L"%ws.tga", m_wzFileBase);

    D3DXCreateTextureFromFileExW(m_pUIRenderer->get_Device(), wzFile, 
                                 D3DX_DEFAULT,
                                 D3DX_DEFAULT, 
                                 D3DX_DEFAULT,
                                 D3DUSAGE_DYNAMIC,
                                 D3DFMT_A8R8G8B8,
                                 D3DPOOL_DEFAULT,
                                 D3DX_DEFAULT,
                                 D3DX_DEFAULT,
                                 0,
                                 NULL, 
                                 NULL,
                                 &m_pTexture);
}

void FontData::LoadFontData()
{
    WCHAR wzFile[MAX_PATH];

    swprintf_s(wzFile, sizeof(wzFile) / sizeof(WCHAR), L"%ws.ftd", m_wzFileBase);

    HANDLE hFile = CreateFileW(wzFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    unsigned long cbRead;

    ReadFile(hFile, &m_fontData, sizeof(FTD_HEADER), &cbRead, NULL);

    int cGlyphs = m_fontData.endGlyph - m_fontData.beginGlyph;
    m_rgGlyphEntries = new FTD_ENTRY[cGlyphs];                                  
    ReadFile(hFile, m_rgGlyphEntries, sizeof(FTD_ENTRY) * cGlyphs, &cbRead, NULL);

    CloseHandle(hFile);
}

void FontData::DeviceLost()
{
    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }
}

void FontData::DeviceReset()
{
    LoadFontTexture();
}

float FontData::GetCharacterWidth(WCHAR wch)
{
    FTD_ENTRY* pGlyphEntry = GetGlyphEntry(wch);

    return ((float)(pGlyphEntry->leadIn + pGlyphEntry->glyphWidth + pGlyphEntry->leadOut)); 
}


FontData::FTD_ENTRY* FontData::GetGlyphEntry(WCHAR wch)
{
    if(wch >= m_fontData.beginGlyph && wch < m_fontData.endGlyph)
    {
        return &(m_rgGlyphEntries[wch - m_fontData.beginGlyph]);
    }
    else
    {
        return &(m_rgGlyphEntries[m_fontData.unknownGlyph - m_fontData.beginGlyph]);
    }
}

void FontData::GetGlyphInfo(WCHAR wch, float* pLeadIn, float* pLeadOut, float* pGlyphWidth)
{
    FTD_ENTRY* pGlyphEntry = GetGlyphEntry(wch);

    *pLeadIn = (float)(pGlyphEntry->leadIn);
    *pLeadOut = (float)(pGlyphEntry->leadOut);
    *pGlyphWidth = (float)(pGlyphEntry->glyphWidth);
}

void FontData::GetGlyphTexCoords(WCHAR wch, float* pU1, float* pV1, float* pU2, float* pV2)
{
    FTD_ENTRY* pGlyphEntry;

    if(wch >= m_fontData.beginGlyph && wch < m_fontData.endGlyph)
    {
        pGlyphEntry = &(m_rgGlyphEntries[wch - m_fontData.beginGlyph]);
    }
    else
    {
        pGlyphEntry = &(m_rgGlyphEntries[m_fontData.unknownGlyph - m_fontData.beginGlyph]);
    }


    *pU1 = pGlyphEntry->U1;
    *pV1 = pGlyphEntry->V1;
    *pU2 = pGlyphEntry->U2;
    *pV2 = pGlyphEntry->V2;
}



float FontData::GetFontAdvance()
{
    return m_fontData.cellHeight;
}


ClientFont::ClientFont(FontData* pFontData, const WCHAR* wzFontName, float pixelSize)
{
    m_pFontData = pFontData;
    m_pixelSize = pixelSize;

    wcscpy_s(m_wzClientFontName, sizeof(m_wzClientFontName) / sizeof(WCHAR), wzFontName);


    m_scaleRatio = pixelSize / pFontData->GetFontSize();
}

void ClientFont::Destroy() { /*Font lifetime controlled at higher level */ }

IClientFormattedText* ClientFont::CreateFormattedText(const WCHAR *pwchText, unsigned int cchText, float availableWidth, bool wrapText)
{        
    return new ClientFormattedText(this, pwchText, cchText, availableWidth, wrapText);
}

float ClientFont::GetCharacterWidth(WCHAR wch)
{
    return m_pFontData->GetCharacterWidth(wch) * m_scaleRatio;
}

void ClientFont::GetGlyphInfo(WCHAR wch, float* pLeadIn, float* pLeadOut, float* pGlyphWidth)
{
    m_pFontData->GetGlyphInfo(wch, pLeadIn, pLeadOut, pGlyphWidth);

    (*pLeadIn) *= m_scaleRatio;
    (*pLeadOut) *= m_scaleRatio;
    (*pGlyphWidth) *= m_scaleRatio;
}

float ClientFont::GetFontAdvance()
{
    return m_pFontData->GetFontAdvance() * m_scaleRatio;

}



