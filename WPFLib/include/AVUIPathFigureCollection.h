#pragma once

#include <AVUIPathFigure.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class PathFigureCollection : public TAnimatableCollection<PathFigure>
{
public:
    DECLARE_ELEMENT(PathFigureCollection, AnimatableCollectionBase);
};

typedef TRefCountedPtr<PathFigureCollection> PathFigureCollectionPtr;

}; // namespace AVUI
