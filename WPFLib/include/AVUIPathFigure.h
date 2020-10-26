#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

class PathSegmentCollection;
class GeometryBuilder;
class PathFigure : public Animatable
{
    friend class PathGeometry;

public:
    DECLARE_ELEMENT(PathFigure, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(StartPoint, Point, Point(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsFilled, bool, true, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsClosed, bool, false, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Segments, PathSegmentCollection, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AutoCreate);

    Rect GetBounds();
    int ClassifyPoint(const Point& pt);

private:
    void BuildGeometry(GeometryBuilder* pGB);

};


}; // namespace AVUI
