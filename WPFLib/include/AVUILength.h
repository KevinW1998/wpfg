#pragma once

namespace AVUI {

class LengthTypeConverter : public StdValueTypeConverter<float>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const float& value) const;

private:

    const static WCHAR* cs_pixelUnitStrings[];
    const static int cs_pixelUnitStringCount;

    const static float cs_pixelUnitConversion[];
    const static int cs_unitConversionCount;

    static float ParseLength(const String& str);
};


};
