#include <Windows.h>
#include <wchar.h>
#include <AVUICommon.h>
#include <D3DUIFontManager.h>
#include <D3DUIFont.h>


FontManager::FontManager(UIRenderer* pUIRenderer, const WCHAR* wzFontPath)
{
    WIN32_FIND_DATAW findData;
    WCHAR wzPathWithFilter[MAX_PATH];

    swprintf_s(wzPathWithFilter, sizeof(wzPathWithFilter) / sizeof(WCHAR), L"%ws\\*.ftd", wzFontPath);

    HANDLE hSearchData = FindFirstFileW(wzPathWithFilter, &findData);

    if(hSearchData == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        findData.cFileName[wcslen(findData.cFileName) - 4] = L'\0';

        WCHAR wzFullPath[MAX_PATH];

        swprintf_s(wzFullPath, sizeof(wzFullPath) / sizeof(WCHAR), L"%ws\\%ws", wzFontPath, findData.cFileName);

        m_fontData.AddItem(new FontData(pUIRenderer, wzFullPath));

    } while(FindNextFileW(hSearchData, &findData));
}

FontManager::~FontManager()
{
    for(int idx = 0; idx < m_fontData.get_Count(); idx++)
    {
        delete m_fontData[idx];
    }

    m_fontData.Clear();

    for(int idx = 0; idx < m_clientFonts.get_Count(); idx++)
    {
        delete m_clientFonts[idx];
    }

    m_clientFonts.Clear();
}

void FontManager::DeviceLost()
{
    for(int idx = 0; idx < m_fontData.get_Count(); idx++)
    {
        m_fontData[idx]->DeviceLost();
    }
}

void FontManager::DeviceReset()
{
    for(int idx = 0; idx < m_fontData.get_Count(); idx++)
    {
        m_fontData[idx]->DeviceReset();
    }
}


AVUI::IClientFont* FontManager::FindMatchingFont(const WCHAR* wzFace, float pixelSize)
{
    for(int idx = 0; idx < m_clientFonts.get_Count(); idx++)
    {
        ClientFont* pClientFont = m_clientFonts[idx];

        if(AVUI::Float::AreClose(pClientFont->GetFontSize(), pixelSize) && !wcscmp(wzFace, pClientFont->get_FontName()))
        {
            return pClientFont;
        }
    }

    ClientFont* pFont = new ClientFont(FindClosestFontData(wzFace, pixelSize), wzFace, pixelSize);

    m_clientFonts.AddItem(pFont);

    return pFont;
}


FontData* FontManager::FindClosestFontData(const WCHAR* wzFace, float pixelSize)
{
    float minDistance = 0.0f;
    FontData* pReturnedData = NULL;
    bool isFaceMatched = false;

    for(int idx = 0; idx < m_fontData.get_Count(); idx++)
    {
        FontData* pFontData = m_fontData[idx];
        float dist = fabsf(pixelSize - pFontData->GetFontSize());

        if(pixelSize > (1.1f * pFontData->GetFontSize()))
        {
            dist *= 30.0f; // Extra penalty for having to scale up
        }

        if(!wcscmp(pFontData->get_FontName(), wzFace))
        {
            if(!isFaceMatched || dist < minDistance)
            {
                isFaceMatched = true;
                pReturnedData = pFontData;
                minDistance = dist;
            }
        }
        else
        {
            if(!isFaceMatched && (dist < minDistance || pReturnedData == NULL))
            {
                pReturnedData = pFontData;
                minDistance = dist;
            }
        }
    }


    return pReturnedData;
}


