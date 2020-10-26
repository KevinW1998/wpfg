#include <AVUICommon.h>
#include <AVUIFont.h>
#include <AVUIApplication.h>
#include <AVUIFormattedText.h>
#include <AVUIIClientRendererInterface.h>

namespace AVUI {


Font::Font(const WCHAR* pwzFontName, float pixelSize)
{
    m_pFont = Application::get_Current()->get_ClientRendererInterface()->CreateClientFont(pwzFontName, pixelSize);
}

Font::~Font()
{
    m_pFont->Destroy();
}

TRefCountedPtr<FormattedText> Font::CreateFormattedText(const WCHAR* pwchText, unsigned int cchText, float widthAvailable, bool wrapText)
{
    return (object_allocate<FormattedText>(m_pFont->CreateFormattedText(pwchText, cchText, widthAvailable, wrapText)));
}


}; // namespace AVUI
