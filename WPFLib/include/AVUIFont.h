#pragma once

namespace AVUI {

class IClientFont;
class FormattedText;

class Font
{
public:
    Font(const WCHAR* pwzFontName, float pixelSize);
    ~Font();

    TRefCountedPtr<FormattedText> CreateFormattedText(const WCHAR* pwchText, unsigned int cchText, float widthAvailable, bool wrapText);

private:
    IClientFont* m_pFont;
};

};
