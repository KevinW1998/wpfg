#pragma once

#include <AVUIModel3D.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class Model3DCollection : public TAnimatableCollection<Model3D>
{
public:
    DECLARE_ELEMENT(Model3DCollection, AnimatableCollectionBase);
};

typedef TRefCountedPtr<Model3DCollection> Model3DCollectionPtr;

}; // namespace AVUI
