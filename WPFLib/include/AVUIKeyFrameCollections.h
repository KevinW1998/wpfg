#pragma once

#include <AVUIKeyFrames.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class SingleKeyFrameCollection : public TAnimatableCollection<SingleKeyFrame>
{
public:
    DECLARE_ELEMENT(SingleKeyFrameCollection, AnimatableCollectionBase);
};

class DoubleKeyFrameCollection : public TAnimatableCollection<DoubleKeyFrame>
{
public:
    DECLARE_ELEMENT(DoubleKeyFrameCollection, AnimatableCollectionBase);
};

class PointKeyFrameCollection : public TAnimatableCollection<PointKeyFrame>
{
public:
    DECLARE_ELEMENT(PointKeyFrameCollection, AnimatableCollectionBase);
};

class ColorKeyFrameCollection : public TAnimatableCollection<ColorKeyFrame>
{
public:
    DECLARE_ELEMENT(ColorKeyFrameCollection, AnimatableCollectionBase);
};

class Vector3DKeyFrameCollection : public TAnimatableCollection<Vector3DKeyFrame>
{
public:
    DECLARE_ELEMENT(Vector3DKeyFrameCollection, AnimatableCollectionBase);
};

class Point3DKeyFrameCollection : public TAnimatableCollection<Point3DKeyFrame>
{
public:
    DECLARE_ELEMENT(Point3DKeyFrameCollection, AnimatableCollectionBase);
};


class Rotation3DKeyFrameCollection : public TAnimatableCollection<Rotation3DKeyFrame>
{
public:
    DECLARE_ELEMENT(Rotation3DKeyFrameCollection, AnimatableCollectionBase);
};



}; // namespace AVUI
