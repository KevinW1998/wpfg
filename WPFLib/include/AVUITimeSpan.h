#pragma once

#include <AVUITypeConverter.h>

namespace AVUI {

struct TimeSpan
{
public:
    TimeSpan() : m_ticks(0) { }
    TimeSpan(int ticks) : m_ticks(ticks) { }
    static TimeSpan get_Zero() { return TimeSpan(0); }

    static TimeSpan FromTicks(int ticks) { return TimeSpan(ticks); }
    static TimeSpan FromSeconds(float seconds) { return TimeSpan((int) (1000.0f * seconds)); }
    int get_Ticks() const { return m_ticks; }

    static TimeSpan FromMilliseconds(int milliseconds) { return TimeSpan(milliseconds); }
    int get_Milliseconds() const { return m_ticks; }
    float get_TotalMilliseconds() const { return 1.0f * m_ticks; }

    bool operator>(const TimeSpan& rhs) const { return m_ticks > rhs.m_ticks; }
    bool operator>=(const TimeSpan& rhs) const { return m_ticks >= rhs.m_ticks; }
    bool operator<(const TimeSpan& rhs) const { return m_ticks < rhs.m_ticks; }
    bool operator<=(const TimeSpan& rhs) const { return m_ticks <= rhs.m_ticks; }
    bool operator==(const TimeSpan& rhs) const { return m_ticks == rhs.m_ticks; }
    bool operator!=(const TimeSpan& rhs) const { return m_ticks != rhs.m_ticks; }
    TimeSpan operator+(const TimeSpan& rhs) const { return TimeSpan(m_ticks + rhs.m_ticks); }
    TimeSpan operator-(const TimeSpan& rhs) const { return TimeSpan(m_ticks - rhs.m_ticks); }

private:
    int m_ticks;

};

struct Duration
{
public:
    Duration() : m_durationType(DurationType_Automatic) { }
    Duration(const TimeSpan& timeSpan) : m_timeSpan(timeSpan), m_durationType(DurationType_TimeSpan) { }

    bool get_HasTimeSpan() const { return m_durationType == DurationType_TimeSpan; }
    const TimeSpan& get_TimeSpan() const { if(!get_HasTimeSpan()) LibraryCriticalError(); return m_timeSpan; }

    bool operator>(const Duration& rhs) const { return false; }
    bool operator==(const Duration& rhs) const 
    {  
        if(!get_HasTimeSpan())
        {
            return m_durationType == rhs.m_durationType;
        }
        if(rhs.get_HasTimeSpan())
        {
            return m_timeSpan == rhs.m_timeSpan;
        }
        return false;
    }
    bool operator!=(const Duration& rhs) const
    {
        return !(*this == rhs);
    }

    static Duration get_Automatic()
    {
        Duration automaticDuration;
        automaticDuration.m_durationType = DurationType_Automatic;
        return automaticDuration;
    }

    static Duration get_Forever()
    {
        Duration foreverDuration;
        foreverDuration.m_durationType = DurationType_Forever;
        return foreverDuration;
    }

private:

    enum DurationType
    {
        DurationType_Automatic,
        DurationType_TimeSpan,
        DurationType_Forever
    };   

    DurationType m_durationType;
    TimeSpan m_timeSpan;
};

class TimeSpanTypeConverter : public StdValueTypeConverter<TimeSpan>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const TimeSpan& value) const;
};
class NullableTimeSpanTypeConverter : public NullableTypeConverter<TimeSpan> { };


class DurationTypeConverter : public StdValueTypeConverter<Duration>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const Duration& value) const;
};


};
