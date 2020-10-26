#pragma once

namespace AVUI {

struct GridLength
{
public:
    GridLength() : m_unitType(GridUnitType::Auto), m_unitValue(0.0f) { }
    GridLength(float value, GridUnitType::Enum unitType) : m_unitType(unitType), m_unitValue(value) { }
    GridLength(float value) : m_unitType(GridUnitType::Pixel), m_unitValue(value) { }

    GridUnitType::Enum get_GridUnitType() const { return m_unitType; }
    float get_Value() const { return m_unitValue; }

    bool get_IsStar() const { return m_unitType == GridUnitType::Star; }
    bool get_IsAuto() const { return m_unitType == GridUnitType::Auto; }
    bool get_IsPixel() const { return m_unitType == GridUnitType::Pixel; }

    bool operator==(const GridLength& rhs) const { return m_unitType == rhs.m_unitType && m_unitValue == rhs.m_unitValue; }

private:

    GridUnitType::Enum m_unitType;
    float m_unitValue;
};

class GridLengthTypeConverter : public StdValueTypeConverter<GridLength>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const GridLength& value) const;

private:
    const static WCHAR* cs_unitStrings[];
    const static int cs_unitStringCount;

    const static WCHAR* cs_pixelUnitStrings[];
    const static int cs_pixelUnitStringCount;

    const static float cs_pixelUnitConversion[];
    const static int cs_unitConversionCount;

    static GridLength ParseGridLength(const String& str);
};


};
