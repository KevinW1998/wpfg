#include <AVUICommon.h>
#include <AVUILength.h>
#include <AVUIStringUtil.h>

namespace AVUI {


const float LengthTypeConverter::cs_pixelUnitConversion[] = { 96.0f, 37.795275590551178f, 1.3333333333333333f };

const WCHAR* LengthTypeConverter::cs_pixelUnitStrings[] = { L"in", L"cm", L"pt" };
const int LengthTypeConverter::cs_pixelUnitStringCount = sizeof(cs_pixelUnitStrings) / sizeof(cs_pixelUnitStrings[0]);


ObjectPtr LengthTypeConverter::ConvertFromString(const String* pString) const
{
    return BoxValue(ParseLength(*pString));
}

ObjectPtr LengthTypeConverter::ConvertToString(const float& value) const
{
    return NULL;
}


float LengthTypeConverter::ParseLength(const String& str)
{
    float value = 0;
    int charsConsumed = 0;
    int currentIndex;
    float conversionMultiplier = 1.0f;

    TRefCountedPtr<String> pLowerString = str.ToLower();

    if(pLowerString->Equals("auto"))
    {
        return Float::get_NaN();
    }

    for(currentIndex = 0; currentIndex < cs_pixelUnitStringCount; currentIndex++)
    {
        String pixelUnitString(cs_pixelUnitStrings[currentIndex]);

        if(pLowerString->EndsWith(pixelUnitString.get_Ptr(), pixelUnitString.get_Cch()))
        {
            charsConsumed = pixelUnitString.get_Cch();
            conversionMultiplier = cs_pixelUnitConversion[currentIndex];
        }
    }

    int cch = pLowerString->get_Cch();

    if(StringUtil::ScanSubsection(pLowerString->get_Ptr(), cch - charsConsumed, L"%f", &value) != 1)
    {
        LibraryCriticalError();
    }

    return value * conversionMultiplier;
}


}; // namespace AVUI
