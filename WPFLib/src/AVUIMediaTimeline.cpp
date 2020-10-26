#include <AVUICommon.h>
#include <AVUIMediaTimeline.h>
#include <AVUIMediaClock.h>

REGISTER_ELEMENT_BEGIN(AVUI::MediaTimeline)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Source)
REGISTER_ELEMENT_END()
namespace AVUI {

TRefCountedPtr<Clock> MediaTimeline::AllocateClock()
{
    ClockPtr pClock(new MediaClock(this), false);

    return pClock;
}

TRefCountedPtr<MediaClock> MediaTimeline::CreateClock()
{
    ClockPtr pClock = Timeline::CreateClock();

    return (MediaClock*) (Clock*) pClock;
}


}; // namespace AVUI
