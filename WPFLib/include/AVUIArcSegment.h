#pragma once

#include <AVUIPathSegment.h>

namespace AVUI {

class GeometryBuilder;

class ArcSegment : public PathSegment
{
public:
    DECLARE_ELEMENT(ArcSegment, PathSegment);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Point, Point, Point(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Size, Size, Size(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RotationAngle, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsLargeArc, bool, false, PropertyFlag::None);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(SweepDirection, SweepDirection, SweepDirection::Counterclockwise, PropertyFlag::None);

    static void BuildGeometryStatic(GeometryBuilder* pGeometryBuilder, const Point& startPoint, const Point& endPoint, 
                                    const Size& size, float rotationAngle, bool isLargeArc, SweepDirection::Enum sweepDirection);

    // Center parameterization
    static void BuildGeometryStaticCenter(GeometryBuilder* pGeometryBuilder, const Point& centerPoint,
                                    float radiusX, float radiusY, float angleStart, float angleDest, SweepDirection::Enum sweepDirection);

    virtual bool IsCurved() { return true; };
    virtual bool IsEmpty() { return false; }
    virtual Rect GetBounds(Point* pStartPoint) { return Rect(); }
    virtual int ClassifyPoint(const Point& pointClassify, Point* pStartPoint) { return 0; }

private:
    virtual Point BuildGeometry(GeometryBuilder* pGB, const Point& startPoint);

};



}; // namespace AVUI
