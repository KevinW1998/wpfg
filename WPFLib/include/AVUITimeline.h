#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

struct RepeatBehavior
{
    RepeatBehavior() : m_iterationCount(1.0f), m_repeatBehaviorType(IterationCount) { }
    RepeatBehavior(float count) : m_repeatDuration(TimeSpan(0)), m_iterationCount(count), m_repeatBehaviorType(IterationCount) { }
    RepeatBehavior(const TimeSpan& duration) : m_repeatDuration(duration), m_iterationCount(0.0f), m_repeatBehaviorType(RepeatDuration) { }

    bool operator==(const RepeatBehavior& rhs) const
    {
        if(m_repeatBehaviorType == rhs.m_repeatBehaviorType)
        {
            if(m_repeatBehaviorType == IterationCount)
            {
                return m_iterationCount == rhs.m_iterationCount;
            }
            if(m_repeatBehaviorType == RepeatDuration)
            {
                return m_repeatDuration == rhs.m_repeatDuration;
            }

            return true;
        }
        return false;
    }

    float get_Count() const
    {
        if(m_repeatBehaviorType != IterationCount)
        {
            LibraryCriticalError();
        }
        return m_iterationCount;
    }
    bool get_HasCount() const { return m_repeatBehaviorType == IterationCount; }

    const TimeSpan& get_Duration() const
    {
        if(m_repeatBehaviorType != RepeatDuration)
        {
            LibraryCriticalError();
        }
        return m_repeatDuration;
    }
    bool get_HasDuration() const { return m_repeatBehaviorType == RepeatDuration; }

    static RepeatBehavior get_Forever()
    {
        RepeatBehavior repeatBehavior;
        repeatBehavior.m_repeatBehaviorType = Forever;

        return repeatBehavior;
    }

private:

    enum RepeatBehaviorType
    {
        IterationCount,
        RepeatDuration,
        Forever
    };

    float m_iterationCount;
    TimeSpan m_repeatDuration;
    RepeatBehaviorType m_repeatBehaviorType;

};

class RepeatBehaviorTypeConverter : public StdValueTypeConverter<RepeatBehavior>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const RepeatBehavior& value) const;
};


class Clock;

class Timeline : public Animatable
{
public:

    TRefCountedPtr<Clock> CreateClock();
    TRefCountedPtr<Clock> CreateClock(bool hasControllableRoot);

    virtual TRefCountedPtr<Clock> AllocateClock();

    DECLARE_ELEMENT(Timeline, Animatable);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AccelerationRatio, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AutoReverse, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(BeginTime, Nullable<TimeSpan>, Nullable<TimeSpan>(TimeSpan::get_Zero()), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(DecelerationRatio, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(DesiredFrameRate, Nullable<int>, Nullable<int>(), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Duration, Duration, Duration(), PropertyFlag::None);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(FillBehavior, FillBehavior, FillBehavior::HoldEnd, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Name, String, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RepeatBehavior, RepeatBehavior, RepeatBehavior(1.0f), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(SpeedRatio, float, 1.0f, PropertyFlag::None);

    Duration get_NaturalDuration(const Clock* pClock) { return get_NaturalDurationCore(pClock); };

protected:

    Timeline() { }
    Timeline(const Nullable<TimeSpan>& beginTime)
    {
        set_BeginTime(beginTime);
    }
    Timeline(const Nullable<TimeSpan>& beginTime, const Duration& duration)
    {
        set_BeginTime(beginTime);
        set_Duration(duration);
    }

    Timeline(const Nullable<TimeSpan>& beginTime, const Duration& duration, const RepeatBehavior& repeatBehavior)
    {
        set_BeginTime(beginTime);
        set_Duration(duration);
        set_RepeatBehavior(repeatBehavior);
    }

    virtual Duration get_NaturalDurationCore(const Clock* pClock) { return Duration::get_Automatic(); }

};

typedef TRefCountedPtr<Timeline> TimelinePtr;


}; // namespace AVUI
