#include <AVUICommon.h>
#include <AVUIBezierSegment.h>
#include <AVUIGeometryBuilder.h>
#include <AVUILineSegment.h>
#include <AVUIPoint.h>

REGISTER_ELEMENT_BEGIN(AVUI::BezierSegment)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Point1);
    REGISTER_PROPERTY(Point2);
    REGISTER_PROPERTY(Point3);
REGISTER_ELEMENT_END()

namespace AVUI {

Point BezierSegment::BuildGeometry(GeometryBuilder* pGB, const Point& startPoint)
{
    BezierSegment::StaticBuildGeometry(pGB, startPoint, get_Point1(), get_Point2(), get_Point3());

    return get_Point3();
}

Rect BezierSegment::GetBounds(Point* pStartPoint)
{
    float minX = Float::Min(pStartPoint->get_X(), get_Point1().get_X());
    minX = Float::Min(minX, get_Point2().get_X());
    minX = Float::Min(minX, get_Point3().get_X());

    float minY = Float::Min(pStartPoint->get_Y(), get_Point1().get_Y());
    minY = Float::Min(minY, get_Point2().get_Y());
    minY = Float::Min(minY, get_Point3().get_Y());

    float maxX = Float::Max(pStartPoint->get_X(), get_Point1().get_X());
    maxX = Float::Max(maxX, get_Point2().get_X());
    maxX = Float::Max(maxX, get_Point3().get_X());

    float maxY = Float::Max(pStartPoint->get_Y(), get_Point1().get_Y());
    maxY = Float::Max(maxY, get_Point2().get_Y());
    maxY = Float::Max(maxY, get_Point3().get_Y());

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

int BezierSegment::ClassifyPoint(const Point& pointClassify, Point* pStartPoint)
{
    return 1;
}

void BezierSegment::StaticBuildGeometry(GeometryBuilder* pGB, const Point& startPoint, const Point& point1, const Point& point2, const Point& point3)
{
    float distToleranceSquare = 0.5f / pGB->get_GeometryScale();
    distToleranceSquare = distToleranceSquare * distToleranceSquare;

    float distToleranceAroundSquare = 4.0f / pGB->get_GeometryScale();

    StaticBuildGeometryRecursive(pGB, startPoint, point1, point2, point3, 0, distToleranceSquare, distToleranceAroundSquare);

    pGB->AddPoint(point3.get_X(), point3.get_Y());
}

void BezierSegment::StaticBuildGeometryRecursive(GeometryBuilder* pGB, const Point& p1, const Point& p2, const Point& p3, const Point& p4, int level, float distToleranceSquare, float distToleranceAroundSquare)
{
    const int MAX_RECURSION_DEPTH = 32;

    if(level > MAX_RECURSION_DEPTH)
    {
        return;
    }

    Point p12 = (p1 + p2) / 2.0f;
    Point p23 = (p2 + p3) / 2.0f;
    Point p34 = (p3 + p4) / 2.0f;
    Point p123 = (p12 + p23) / 2.0f;
    Point p234 = (p23 + p34) / 2.0f;
    Point p1234 = (p123 + p234) / 2.0f;

    Vector dp = p4 - p1;

    float d2 = fabs(((p2.get_X() - p4.get_X()) * dp.get_Y() - (p2.get_Y() - p4.get_Y()) * dp.get_X()));
    float d3 = fabs(((p3.get_X() - p4.get_X()) * dp.get_Y() - (p3.get_Y() - p4.get_Y()) * dp.get_X()));

    if(d2 <= Float::get_Epsilon() && d3 <= Float::get_Epsilon())
    {        
        if(fabs(p1.get_X() + p3.get_X() - p2.get_X() - p2.get_X()) +
           fabs(p1.get_Y() + p3.get_Y() - p2.get_Y() - p2.get_Y()) +
           fabs(p2.get_X() + p4.get_X() - p3.get_X() - p3.get_X()) +
           fabs(p2.get_Y() + p4.get_Y() - p3.get_Y() - p3.get_Y()) <= distToleranceAroundSquare)
        {
            pGB->AddPoint(p1234.get_X(), p1234.get_Y());
            return;
        }    
    }
    else if(d3 <= Float::get_Epsilon())
    {
        if(d2 * d2 <= distToleranceSquare * (dp.get_X() * dp.get_X() + dp.get_Y() * dp.get_Y()))
        {
            pGB->AddPoint(p23.get_X(), p23.get_Y());
            return;
        }
    }
    else if(d2 <= Float::get_Epsilon())
    {
        if(d3 * d3 <= distToleranceSquare * (dp.get_X() * dp.get_X() + dp.get_Y() * dp.get_Y()))
        {
            pGB->AddPoint(p23.get_X(), p23.get_Y());
            return;
        }
    }
    else
    {
        if((d2 + d3)*(d2 + d3) <= distToleranceSquare * (dp.get_X() * dp.get_X() + dp.get_Y() * dp.get_Y()))
        {
            pGB->AddPoint(p23.get_X(), p23.get_Y());
            return;
        }
    }

    StaticBuildGeometryRecursive(pGB, p1, p12, p123, p1234, level + 1, distToleranceSquare, distToleranceAroundSquare); 
    StaticBuildGeometryRecursive(pGB, p1234, p234, p34, p4, level + 1, distToleranceSquare, distToleranceAroundSquare); 

}

}; // namespace AVUI

