#pragma once

#include <AVUIPathSegment.h>

namespace AVUI {

class GeometryBuilder;

class LineSegment : public PathSegment
{
public:
    DECLARE_ELEMENT(LineSegment, PathSegment);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Point, Point, Point(), PropertyFlag::None);

    virtual bool IsCurved() { return false; }
    virtual Rect GetBounds(Point* pStartPoint);
    virtual int ClassifyPoint(const Point& pointClassify, Point* pStartPoint);

    static void BuildGeometryStatic(GeometryBuilder* pGeometryBuilder, const Point& startPoint, const Point& endPoint);
    static int StaticClassifyPoint(const Point& ptClassify, const Point& ptLineStart, const Point& ptLineEnd);
    static int CalculateDistance(const Point& ptClassify, const Point& ptLineStart, const Point& ptLineEnd);

    static bool IsInThickenedLineSegment(const Point& ptClassify, const Point& ptLineStart, const Point& ptLineEnd, float strokeThickness);

private:
    virtual Point BuildGeometry(GeometryBuilder* pGB, const Point& startPoint);

};



}; // namespace AVUI
