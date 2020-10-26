#pragma once

#include <AVUITimeline.h>

namespace AVUI {

class TimelineCollection;
class ClockGroup;

class TimelineGroup : public Timeline
{
public:
    DECLARE_ELEMENT(TimelineGroup, Timeline);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Children, TimelineCollection, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AutoCreate);

    TimelineGroup() : Timeline() { }
    TimelineGroup(const Nullable<TimeSpan>& beginTime) : Timeline(beginTime) { }
    TimelineGroup(const Nullable<TimeSpan>& beginTime, const Duration& duration) : Timeline(beginTime, duration) { }
    TimelineGroup(const Nullable<TimeSpan>& beginTime, const Duration& duration, const RepeatBehavior& repeatBehavior) : Timeline(beginTime, duration, repeatBehavior) { }

    TRefCountedPtr<ClockGroup> CreateClock();
    virtual TRefCountedPtr<Clock> AllocateClock();

};

  
class ParallelTimeline : public TimelineGroup
{
public:
    DECLARE_ELEMENT(ParallelTimeline, TimelineGroup);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(SlipBehavior, SlipBehavior, SlipBehavior::Grow, PropertyFlag::None);

    ParallelTimeline() : TimelineGroup() { }
    ParallelTimeline(const Nullable<TimeSpan>& beginTime) : TimelineGroup(beginTime) { }
    ParallelTimeline(const Nullable<TimeSpan>& beginTime, const Duration& duration) : TimelineGroup(beginTime, duration) { }
    ParallelTimeline(const Nullable<TimeSpan>& beginTime, const Duration& duration, const RepeatBehavior& repeatBehavior) : TimelineGroup(beginTime, duration, repeatBehavior) { }

protected:

    virtual Duration get_NaturalDurationCore(const Clock* pClock);

};

}; // namespace AVUI
