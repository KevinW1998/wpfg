#include <AVUICommon.h>
#include <AVUIGridLength.h>
#include <AVUIStringUtil.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::GridLength)
    REGISTER_TYPE_CONVERTER(GridLengthTypeConverter)
REGISTER_VALUE_TYPE_END()

namespace AVUI {


const float GridLengthTypeConverter::cs_pixelUnitConversion[] = { 96.0f, 37.795275590551178f, 1.3333333333333333f };

const WCHAR* GridLengthTypeConverter::cs_pixelUnitStrings[] = { L"in", L"cm", L"pt" };
const int GridLengthTypeConverter::cs_pixelUnitStringCount = sizeof(cs_pixelUnitStrings) / sizeof(cs_pixelUnitStrings[0]);

const WCHAR* GridLengthTypeConverter::cs_unitStrings[] = { L"auto", L"px", L"*" };
const int GridLengthTypeConverter::cs_unitStringCount = sizeof(cs_unitStrings) / sizeof(cs_unitStrings[0]);


ObjectPtr GridLengthTypeConverter::ConvertFromString(const String* pString) const
{
    return BoxValue(ParseGridLength(*pString));
}

ObjectPtr GridLengthTypeConverter::ConvertToString(const GridLength& value) const
{
    return NULL;
}


GridLength GridLengthTypeConverter::ParseGridLength(const String& str)
{
    float value = 0;
    GridUnitType::Enum unit = GridUnitType::Pixel;
    int charsConsumed = 0;
    int currentIndex;
    float conversionMultiplier = 1.0f;

    TRefCountedPtr<String> pLowerString = str.ToLower();


    for (currentIndex = 0; currentIndex < cs_unitStringCount; currentIndex++)
    {
        String unitString(cs_unitStrings[currentIndex]);

        if(pLowerString->EndsWith(unitString.get_Ptr(), unitString.get_Cch()))
        {
            charsConsumed = unitString.get_Cch();
            unit = (GridUnitType::Enum) currentIndex;
        }
    }

    if (currentIndex >= cs_unitStringCount)
    {
        for(currentIndex = 0; currentIndex < cs_pixelUnitStringCount; currentIndex++)
        {
            String pixelUnitString(cs_pixelUnitStrings[currentIndex]);

            if(pLowerString->EndsWith(pixelUnitString.get_Ptr(), pixelUnitString.get_Cch()))
            {
                charsConsumed = pixelUnitString.get_Cch();
                conversionMultiplier = cs_pixelUnitConversion[currentIndex];
            }
        }
    }

    int cch = pLowerString->get_Cch();

    if(cch == charsConsumed && (unit == GridUnitType::Auto || unit == GridUnitType::Star))
    {
        value = 1.0f;
    }
    else
    {
        if(StringUtil::ScanSubsection(pLowerString->get_Ptr(), cch - charsConsumed, L"%f", &value) != 1)
        {
            LibraryCriticalError();
        }

        value = value * conversionMultiplier;
    }

    return GridLength(value, unit);
}

//=  



}; // namespace AVUI
