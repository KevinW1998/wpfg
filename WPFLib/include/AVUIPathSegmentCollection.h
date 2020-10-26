#pragma once

#include <AVUIPathSegment.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class PathSegmentCollection : public TAnimatableCollection<PathSegment>
{
public:
    DECLARE_ELEMENT(PathSegmentCollection, AnimatableCollectionBase);
};

typedef TRefCountedPtr<PathSegmentCollection> PathSegmentCollectionPtr;

}; // namespace AVUI
