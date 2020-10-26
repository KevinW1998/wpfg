#pragma once

#include <AVUIIClientRendererInterface.h>
#include <D3DUIFont.h>

class ClientRenderGeometry;

class ClientFormattedText : public AVUI::IClientFormattedText
{
public:
    ClientFormattedText(ClientFont* pFont, const WCHAR* pwchText, unsigned int cchText, float availableWidth, bool wrapText);
    ~ClientFormattedText();

    bool IsWhiteSpace(WCHAR wch);
    void FormatText(const WCHAR* pwchText, int cchText, bool wrapText);

    void Trim(float trimWidth);

    bool get_HasEndOfText() const;
    float get_Width() const;
    unsigned int get_Cch() const;
    float get_Height() const;

    float GetXLocationBeforeIndex(int cp) const;
    IDirect3DTexture9* GetFontTexture();

    void Destroy()
    {
        delete this;
    }

    int FormatTextUntilWrap(const WCHAR* pwchText, int cchText, float wrappingWidth, float* pSpaceConsumed);
    float AddGlyph(ClientRenderGeometry* pGeometry, float xPosition, float yPosition, WCHAR wch) const;
    void AddGlyphGeometry(ClientRenderGeometry* pGeometry, float xPosition, float yPosition, float glyphWidth, float glyphHeight, float u1, float v1, float u2, float v2) const;
    void AddToGeometryData(ClientRenderGeometry* pGeometryData, float originX, float originY);

private:
    WCHAR* m_pwchText;
    float m_pixelWidth;
    ClientFont* m_pFont;
    float m_trimWidth;
    float m_availableWidth;
    bool m_isTrimmed;
    int m_cchText;
};

