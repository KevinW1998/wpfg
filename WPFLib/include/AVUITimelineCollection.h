#pragma once

#include <AVUITimeline.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class TimelineCollection : public TObjectCollection<Timeline>
{
public:
    DECLARE_ELEMENT(TimelineCollection, ObjectCollectionBase);
};


}; // namespace AVUI
