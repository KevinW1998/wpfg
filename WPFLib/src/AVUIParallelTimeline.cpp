#include <AVUICommon.h>
#include <AVUIAnimationClock.h>
#include <AVUIClockGroup.h>
#include <AVUIParallelTimeline.h>

REGISTER_ELEMENT_BEGIN(AVUI::TimelineGroup)
    REGISTER_DEFAULT_FACTORY();

    REGISTER_PROPERTY(Children);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::ParallelTimeline)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(SlipBehavior);
REGISTER_ELEMENT_END()

namespace AVUI {


TRefCountedPtr<ClockGroup> TimelineGroup::CreateClock()
{
    return (ClockGroup*)(Clock*) Timeline::CreateClock();
}

TRefCountedPtr<Clock> TimelineGroup::AllocateClock()
{
    ClockPtr pClock(new ClockGroup(this), false);

    return pClock;
}

Duration ParallelTimeline::get_NaturalDurationCore(const Clock* pClock)
{
    Duration duration = (Duration) TimeSpan::get_Zero();

    if(pClock == NULL)
    {
        return duration;
    }
    
    const ClockGroup* pClockGroup = object_cast<ClockGroup>(pClock);

    if(pClockGroup == NULL)
    {
        return Duration::get_Forever();
    }

    const ItemStructList<ClockPtr>& children = pClockGroup->get_Children();

    for (int i = 0; i < children.get_Count(); i++)
    {
          Nullable<TimeSpan> nullable1 = children[i]->get_EndOfActivePeriod();
          if (!nullable1.get_HasValue())
          {
                return Duration::get_Forever();
          }
          Nullable<TimeSpan> nullable2 = nullable1;
          TimeSpan span1 = duration.get_TimeSpan();
          if (nullable2.get_HasValue() ? (nullable2.get_ValueOrDefault() > span1) : false)
          {
                duration = (Duration) nullable1.get_Value();
          }
    }
    return duration;

}


}; // namespace AVUI
