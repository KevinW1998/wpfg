#pragma once

#include <AVUIClock.h>
#include <AVUIAnimationTimeline.h>
#include <AVUIItemStructList.h>

namespace AVUI {

class AnimationClock : public Clock
{
public:
    DECLARE_ELEMENT(AnimationClock, Clock);

    AnimationClock() : Clock(NULL) { };
    AnimationClock(AnimationTimeline* pTimeline) : Clock(pTimeline) { };

    ObjectPtr GetCurrentValue(Object* pDefaultOriginValue, Object* pDefaultDestinationValue)
    {
        return get_Timeline()->GetCurrentValue(pDefaultOriginValue, pDefaultDestinationValue, this);
    }

    bool get_NeedsTicksWhenActive() const { return true; }
    AnimationTimeline* get_Timeline() { return (AnimationTimeline*) Clock::get_Timeline(); }
};


typedef TRefCountedPtr<AnimationClock> AnimationClockPtr;
typedef ArrayInstance<AnimationClockPtr> AnimationClockPtrArray;
typedef ItemStructList<AnimationClockPtr> AnimationClockList;


}; // namespace AVUI
