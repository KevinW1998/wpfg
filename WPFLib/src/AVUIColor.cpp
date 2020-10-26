#include <AVUICommon.h>
#include <AVUIColor.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::Color);
    REGISTER_TYPE_CONVERTER(ColorTypeConverter)    
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(AVUI::Nullable<Color>);
    REGISTER_TYPE_CONVERTER(NullableColorTypeConverter)    
REGISTER_VALUE_TYPE_END()

namespace AVUI {


ObjectPtr ColorTypeConverter::ConvertFromString(const String* pString) const
{
    ObjectPtr pObjectColor = GET_TYPE(KnownColor).get_TypeConverter()->ConvertFrom(pString);

    if(pObjectColor != NULL)
    {
        return BoxValue(Color((unsigned int)UnboxValue<int>(pObjectColor)));
    }
    const String& str = *pString;

    if(str[0] != L'#' || (str.get_Cch() != 7 && str.get_Cch() != 9))
    {
         LibraryCriticalError();
    }

    unsigned char a = 0, r = 0, g = 0, b = 0;

    if(str.get_Cch() == 9)
    {        
        a = ParseHexChar(str[1]) * 0x10 + ParseHexChar(str[2]);
        r = ParseHexChar(str[3]) * 0x10 + ParseHexChar(str[4]);
        g = ParseHexChar(str[5]) * 0x10 + ParseHexChar(str[6]);
        b = ParseHexChar(str[7]) * 0x10 + ParseHexChar(str[8]);
    }
    else
    {
        r = ParseHexChar(str[1]) * 0x10 + ParseHexChar(str[2]);
        g = ParseHexChar(str[3]) * 0x10 + ParseHexChar(str[4]);
        b = ParseHexChar(str[5]) * 0x10 + ParseHexChar(str[6]);
        a = 0xFF;
    }

    return BoxValue(Color(r, g, b, a));
}

unsigned char ColorTypeConverter::ParseHexChar(WCHAR wch) 
{
    if(wch >= L'0' && wch <= L'9')
    {
        return (unsigned char)wch - L'0';
    }

    if(wch >= L'a' && wch <= L'f')
    {
        return (unsigned char)(wch - L'a' + 10);
    }
    if(wch >= L'A' && wch <= L'F')
    {
        return (unsigned char)(wch - L'A' + 10);
    }

    LibraryCriticalError();

    return 0;
}

ObjectPtr ColorTypeConverter::ConvertToString(const Color& value) const
{
    return NULL;
}

Color Color::operator+(const Color& rhs) const
{
    Color colorRet;

    colorRet.m_r = rhs.m_r + m_r;
    colorRet.m_g = rhs.m_g + m_g;
    colorRet.m_b = rhs.m_b + m_b;
    colorRet.m_a = rhs.m_a + m_a;

    return colorRet;
}

Color Color::operator-(const Color& rhs) const
{
    Color colorRet;

    colorRet.m_r = m_r - rhs.m_r;
    colorRet.m_g = m_g - rhs.m_g;
    colorRet.m_b = m_b - rhs.m_b;
    colorRet.m_a = m_a - rhs.m_a;

    return colorRet;
}

Color Color::operator*(float coeff) const
{
    Color colorRet;

    colorRet.m_r = m_r * coeff;
    colorRet.m_g = m_g * coeff;
    colorRet.m_b = m_b * coeff;
    colorRet.m_a = m_a * coeff;

    return colorRet;
}


Color::Color(unsigned int argb)
{
    m_a = ((argb >> 24) & 0xFF) / 255.0f;
    m_r = ((argb >> 16) & 0xFF) / 255.0f;
    m_g = ((argb >> 8) & 0xFF) / 255.0f;
    m_b = ((argb >> 0) & 0xFF) / 255.0f;
}

unsigned int Color::get_ARGB() const
{
    unsigned char a = (unsigned char) Int::Max(0, Int::Min((int)(m_a * 255.0f), 255));
    unsigned char r = (unsigned char) Int::Max(0, Int::Min((int)(m_r * 255.0f), 255));
    unsigned char g = (unsigned char) Int::Max(0, Int::Min((int)(m_g * 255.0f), 255));
    unsigned char b = (unsigned char) Int::Max(0, Int::Min((int)(m_b * 255.0f), 255));

    return (a << 24) + (r << 16) + (g << 8) + b;
}


}; // namespace AVUI
