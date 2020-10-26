#pragma once

#include <AVUIPathSegment.h>

namespace AVUI {

class GeometryBuilder;
class Point;

class BezierSegment : public PathSegment
{
public:
    DECLARE_ELEMENT(BezierSegment, PathSegment);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Point1, Point, Point(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Point2, Point, Point(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Point3, Point, Point(), PropertyFlag::None);

    virtual bool IsCurved() { return true; }
    virtual Rect GetBounds(Point* pStartPoint);
    virtual int ClassifyPoint(const Point& pointClassify, Point* pStartPoint);

    static void StaticBuildGeometry(GeometryBuilder* pGB, const Point& startPoint, const Point& point1, const Point& point2, const Point& point3);
    static void StaticBuildGeometryRecursive(GeometryBuilder* pGB, const Point& p1, const Point& p2, const Point& p3, const Point& p4, int level, float distToleranceSquare, float distToleranceAroundSquare);

private:
    virtual Point BuildGeometry(GeometryBuilder* pGB, const Point& startPoint);

};



}; // namespace AVUI
