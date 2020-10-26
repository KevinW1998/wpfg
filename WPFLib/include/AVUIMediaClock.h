#pragma once

#include <AVUIClock.h>

namespace AVUI {

class MediaTimeline;

class MediaClock : public Clock
{
public:
    DECLARE_ELEMENT(MediaClock, Clock);

    MediaClock() : Clock(NULL) { };
    MediaClock(MediaTimeline* pTimeline) : Clock((Timeline*)pTimeline) { };

    MediaTimeline* get_Timeline() { return (MediaTimeline*) Clock::get_Timeline(); }
};

}; // namespace AVUI
