#pragma once

#include <AVUITimeline.h>

namespace AVUI {

class AnimationClock;

class AnimationTimeline : public Timeline
{
public:
    DECLARE_ELEMENT(AnimationTimeline, Timeline);

    virtual ObjectPtr GetCurrentValue(Object* pDefaultOriginValue, Object* pDefaultDestinationValue, AnimationClock* pAnimationClock)
    {
          return pDefaultDestinationValue;
    } 

    TRefCountedPtr<AnimationClock> CreateClock();
    virtual TRefCountedPtr<Clock> AllocateClock();

    AnimationTimeline() { }
protected:

    virtual Duration get_NaturalDurationCore(const Clock* pClock) { return (Duration) TimeSpan::FromMilliseconds(1000); }
};


}; // namespace AVUI
