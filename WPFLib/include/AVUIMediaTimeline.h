#pragma once

#include <AVUITimeline.h>

namespace AVUI {

class MediaClock;

class MediaTimeline : public Timeline
{
public:
    DECLARE_ELEMENT(MediaTimeline, Timeline);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Source, String, NULL, PropertyFlag::None);

    TRefCountedPtr<MediaClock> CreateClock();
    virtual TRefCountedPtr<Clock> AllocateClock();

    MediaTimeline() { }

protected:

    // This should be a calculated length, when available. TODO -- Fix a lot of media handling.
    virtual Duration get_NaturalDurationCore(const Clock* pClock) { return (Duration) TimeSpan::FromMilliseconds(1000); }
};


}; // namespace AVUI
