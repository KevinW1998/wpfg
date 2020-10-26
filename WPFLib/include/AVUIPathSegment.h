#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

class GeometryBuilder;

class PathSegment : public Animatable
{
    friend class PathFigure;

public:
    DECLARE_ELEMENT(PathSegment, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsStroked, bool, true, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsSmoothJoin, bool, false, PropertyFlag::None);

    virtual bool IsCurved() = 0;
    virtual bool IsEmpty() { return false; }
    virtual Rect GetBounds(Point* pStartPoint) = 0;
    virtual int ClassifyPoint(const Point& pointClassify, Point* pStartPoint) = 0;

private:
    virtual Point BuildGeometry(GeometryBuilder* pGB, const Point& startPoint) = 0;
};



}; // namespace AVUI
