#pragma once

#include <AVUITransform3D.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class Transform3DCollection : public TAnimatableCollection<Transform3D>
{
public:
    DECLARE_ELEMENT(Transform3DCollection, AnimatableCollectionBase);
};

typedef TRefCountedPtr<Transform3DCollection> Transform3DCollectionPtr;

}; // namespace AVUI
