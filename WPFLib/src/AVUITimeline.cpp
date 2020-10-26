#include <AVUICommon.h>
#include <AVUITimeline.h>
#include <AVUIClock.h>
#include <AVUIClockGroup.h>

REGISTER_ELEMENT_BEGIN(AVUI::Timeline)

    REGISTER_PROPERTY(AccelerationRatio);
    REGISTER_PROPERTY(AutoReverse);
    REGISTER_PROPERTY(BeginTime);
    REGISTER_PROPERTY(DecelerationRatio);
    REGISTER_PROPERTY(DesiredFrameRate);
    REGISTER_PROPERTY(Duration);
    REGISTER_PROPERTY(FillBehavior);
    REGISTER_PROPERTY(Name);
    REGISTER_PROPERTY(RepeatBehavior);
    REGISTER_PROPERTY(SpeedRatio);
REGISTER_ELEMENT_END()

REGISTER_VALUE_TYPE_BEGIN(AVUI::RepeatBehavior)
    REGISTER_TYPE_CONVERTER(RepeatBehaviorTypeConverter)
REGISTER_VALUE_TYPE_END()
namespace AVUI {

ClockPtr Timeline::CreateClock()
{
    return CreateClock(true);
}


ClockPtr Timeline::CreateClock(bool hasControllableRoot)
{
    return Clock::BuildClockTreeFromTimeline(this, hasControllableRoot);
}

TRefCountedPtr<Clock> Timeline::AllocateClock()
{
    return ClockPtr(new Clock(this), false);
}


}; // namespace AVUI
