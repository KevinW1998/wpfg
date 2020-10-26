#include <Windows.h>
#include <D3DUIFont.h>
#include <D3DUIFormattedText.h>
#include <D3DUIRenderer.h>
#include <D3DX9.h>
#include <wchar.h>


ClientFormattedText::ClientFormattedText(ClientFont* pFont, const WCHAR* pwchText, unsigned int cchText, float availableWidth, bool wrapText) : m_pFont(pFont), m_availableWidth(availableWidth), m_isTrimmed(false), m_pwchText(NULL)
{
    FormatText(pwchText, cchText, wrapText);
}

ClientFormattedText::~ClientFormattedText()
{
    delete[] m_pwchText;
}


bool ClientFormattedText::IsWhiteSpace(WCHAR wch)
{
    if(wch == L' ' || wch == L'\t')
    {
        return true;
    }

    return false;
}

void ClientFormattedText::Trim(float trimWidth)
{		
    m_isTrimmed = true;    
    m_trimWidth = trimWidth;
}

bool ClientFormattedText::get_HasEndOfText() const
{
    return false;
}
float ClientFormattedText::get_Width() const
{
    return m_pixelWidth;
}
unsigned int ClientFormattedText::get_Cch() const
{
    return m_cchText;
}


float ClientFormattedText::GetXLocationBeforeIndex(int cp) const
{
    float xPosition = 0.0f;

    if(cp > (int)m_cchText)
    {
        return xPosition;
    }

    while(cp-- > 0)
    {
        xPosition += m_pFont->GetCharacterWidth(m_pwchText[cp]);
    }

    return xPosition;
}

IDirect3DTexture9* ClientFormattedText::GetFontTexture()
{
    return m_pFont->GetFontData()->GetTexture();
}

float ClientFormattedText::get_Height() const
{
    return m_pFont->GetFontAdvance();
}

int ClientFormattedText::FormatTextUntilWrap(const WCHAR* pwchText, int cchText, float wrappingWidth, float* pSpaceConsumed)
{    
    *pSpaceConsumed = 0.0f;         

    for(int cpCur = 0; cpCur < cchText; cpCur++)
    {    
        if(pwchText[cpCur] == '\r')
        {    
            if(cpCur != cchText && pwchText[cpCur + 1] == '\n')    
            {    
                return cpCur + 2;    
            }

            return cpCur + 1;    
        }    
        else if(pwchText[cpCur] == '\n')    
        {    
            return cpCur + 1;    
        }
       
        float charWidth = m_pFont->GetCharacterWidth(pwchText[cpCur]);

        if(*pSpaceConsumed + charWidth > wrappingWidth)    
        {    
            return cpCur;    
        }

        *pSpaceConsumed += charWidth;    
    }
   
    return cchText;    
}

void ClientFormattedText::FormatText(const WCHAR* pwchText, int cchText, bool wrapText)
{
    float wrappingWidth = m_availableWidth; 

    if(!wrapText)
    {
        wrappingWidth = 10e26f;           
    }

     
    float spaceConsumed = 0.0f;

    int cpWrap = FormatTextUntilWrap(pwchText, cchText, wrappingWidth, &spaceConsumed);

    if(wrapText && cpWrap != cchText)
    {
        float wrappedSpaceConsumed = spaceConsumed;
        int cpWrapCurrent = cpWrap;

        while (!IsWhiteSpace(pwchText[cpWrapCurrent]) && cpWrapCurrent != 0)
        {
            wrappedSpaceConsumed -= m_pFont->GetCharacterWidth(pwchText[cpWrapCurrent]);

            cpWrapCurrent -= 1;
        }

        if (cpWrapCurrent != 0)
        {
            spaceConsumed = wrappedSpaceConsumed;
            cpWrap = cpWrapCurrent;
        }
    }

    int cchString = cpWrap;

    m_pwchText = new WCHAR[cchString + 1];

    wcsncpy_s(m_pwchText, cchString + 1, pwchText, cpWrap);

    m_pixelWidth = spaceConsumed;

    m_cchText = cchString;
}    



float ClientFormattedText::AddGlyph(ClientRenderGeometry* pGeometry, float xPosition, float yPosition, WCHAR wch) const
{
    float glyphHeight = m_pFont->GetFontAdvance();

    float leadIn;
    float leadOut;
    float glyphWidth;

    m_pFont->GetGlyphInfo(wch, &leadIn, &leadOut, &glyphWidth);

    float u1, v1, u2, v2;    

    m_pFont->GetFontData()->GetGlyphTexCoords(wch, &u1, &v1, &u2, &v2);

    AddGlyphGeometry(pGeometry, xPosition, yPosition, glyphWidth, glyphHeight, u1, v1, u2, v2);

    return xPosition + glyphWidth;

} 

void ClientFormattedText::AddGlyphGeometry(ClientRenderGeometry* pGeometry, float xPosition, float yPosition, float glyphWidth, float glyphHeight, float u1, float v1, float u2, float v2) const
{
    unsigned int vtxStart = pGeometry->GetVertexCount();

    pGeometry->AddVertexUV(xPosition, yPosition, u1, v1);
    pGeometry->AddVertexUV(xPosition + glyphWidth, yPosition, u2, v1);
    pGeometry->AddVertexUV(xPosition + glyphWidth, yPosition + glyphHeight, u2, v2);
    pGeometry->AddVertexUV(xPosition, yPosition + glyphHeight, u1, v2);

    pGeometry->AddIndexTriplet(vtxStart, vtxStart + 1, vtxStart + 2);
    pGeometry->AddIndexTriplet(vtxStart, vtxStart + 2, vtxStart + 3);
}


void ClientFormattedText::AddToGeometryData(ClientRenderGeometry* pGeometryData, float originX, float originY)
{
    float trimCharsWidth = m_pFont->GetCharacterWidth(L'.') * 3.0f;

    float xPosition = originX;
    float yPosition = originY;

    for(int i = 0; i < m_cchText; i++)
    {
        if(m_pwchText[i] == L'\r' || m_pwchText[i] == L'\n')

        {
            continue;
        }

        float leadIn;
        float leadOut;
        float glyphWidth;

        m_pFont->GetGlyphInfo(m_pwchText[i], &leadIn, &leadOut, &glyphWidth);

        if(m_isTrimmed && (m_pixelWidth - 0.5) > m_trimWidth && (xPosition + glyphWidth + trimCharsWidth) > m_trimWidth)

        {
            for(int trimChar = 0; trimChar < 3; trimChar++)
            {
                xPosition = AddGlyph(pGeometryData, xPosition, yPosition, L'.');
            }
            break;
        }
        else
        {
            float effectiveLeadIn = leadIn;

            if(i == 0)
            {
                effectiveLeadIn = 0.0f;
            }

            AddGlyph(pGeometryData, xPosition + effectiveLeadIn, yPosition, m_pwchText[i]);

            xPosition += effectiveLeadIn + glyphWidth + leadOut;  
        }
    }
}

