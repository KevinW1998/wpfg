#include <AVUICommon.h>
#include <AVUIThickness.h>
#include <AVUIStringUtil.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::Thickness)
    REGISTER_TYPE_CONVERTER(ThicknessTypeConverter)
REGISTER_VALUE_TYPE_END()

namespace AVUI {

bool Thickness::get_IsUniform() const
{ 
    if(Float::AreClose(m_left, m_top) && Float::AreClose(m_left, m_right))
    {
        return Float::AreClose(m_left, m_bottom);
    }
    return false;
}

bool Thickness::get_IsZero() const
{ 
    if(Float::IsZero(m_left) && Float::IsZero(m_right) && Float::IsZero(m_top))
    {
        return Float::IsZero(m_bottom);
    }
    return false;
}



ObjectPtr ThicknessTypeConverter::ConvertFromString(const String* pString) const
{
    if(pString->IndexOf(L',') != -1)
    {
        float top, left, right, bottom;
        if(StringUtil::Scan(pString->get_Ptr(), L"%f, %f, %f, %f", &left, &top, &right, &bottom) != 4)
        {
            LibraryCriticalError();
        }
        return BoxValue(Thickness(left, top, right, bottom));
    }
    else
    {
        float uniformValue;

        if(StringUtil::Scan(pString->get_Ptr(), L"%f", &uniformValue) != 1)
        {
            LibraryCriticalError();
        }
        return BoxValue(Thickness(uniformValue));
    }
}

ObjectPtr ThicknessTypeConverter::ConvertToString(const Thickness& value) const { return NULL; }
}; // namespace AVUI

