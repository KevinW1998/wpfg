#pragma once

#include <AVUITransform.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class TransformCollection : public TAnimatableCollection<Transform>
{
public:
    DECLARE_ELEMENT(TransformCollection, AnimatableCollectionBase);
};

typedef TRefCountedPtr<TransformCollection> TransformCollectionPtr;

}; // namespace AVUI
