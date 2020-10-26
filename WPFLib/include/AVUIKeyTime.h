#pragma once

#include <AVUITypeConverter.h>

namespace AVUI {

class KeyTime
{
    friend class KeyTimeTypeConverter;
public:
    KeyTime() { };

    KeyTimeType::Enum get_Type() const { return m_type; }
    TimeSpan get_TimeSpan() const { return m_timeSpanValue; }
    float get_Percent() const { return m_floatValue; }

    bool operator==(const KeyTime& rhs) const { return m_type == rhs.m_type && m_timeSpanValue == rhs.m_timeSpanValue && m_floatValue == rhs.m_floatValue; }

private:

    KeyTimeType::Enum m_type;
    TimeSpan m_timeSpanValue;
    float m_floatValue;
};

class KeyTimeTypeConverter : public StdValueTypeConverter<KeyTime>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const KeyTime& value) const;
};


};
