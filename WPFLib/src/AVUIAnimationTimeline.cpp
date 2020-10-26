#include <AVUICommon.h>
#include <AVUIAnimationTimeline.h>
#include <AVUIClockGroup.h>
#include <AVUIAnimationClock.h>

REGISTER_ELEMENT_BEGIN(AVUI::AnimationTimeline)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::AnimationClock)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {

TRefCountedPtr<Clock> AnimationTimeline::AllocateClock()
{
    ClockPtr pClock(new AnimationClock(this), false);

    return pClock;
}

TRefCountedPtr<AnimationClock> AnimationTimeline::CreateClock()
{
    ClockPtr pClock = Timeline::CreateClock();

    return (AnimationClock*) (Clock*) pClock;
}


}; // namespace AVUI
