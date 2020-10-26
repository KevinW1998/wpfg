#include <AVUICommon.h>
#include <AVUICornerRadius.h>
#include <AVUIStringUtil.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::CornerRadius)
    REGISTER_TYPE_CONVERTER(CornerRadiusTypeConverter)
REGISTER_VALUE_TYPE_END()

namespace AVUI {


ObjectPtr CornerRadiusTypeConverter::ConvertFromString(const String* pString) const
{
    if(pString->IndexOf(L',') != -1)
    {
        float top, left, right, bottom;
        if(StringUtil::Scan(pString->get_Ptr(), L"%f, %f, %f, %f", &left, &top, &right, &bottom) != 4)
        {
            LibraryCriticalError();
        }
        return BoxValue(CornerRadius(left, top, right, bottom));
    }
    else
    {
        float uniformValue;

        if(StringUtil::Scan(pString->get_Ptr(), L"%f", &uniformValue) != 1)
        {
            LibraryCriticalError();
        }
        return BoxValue(CornerRadius(uniformValue));
    }
}

ObjectPtr CornerRadiusTypeConverter::ConvertToString(const CornerRadius& value) const { return NULL; }
}; // namespace AVUI
