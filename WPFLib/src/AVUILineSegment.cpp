#include <AVUICommon.h>
#include <AVUILineSegment.h>
#include <AVUIGeometryBuilder.h>

REGISTER_ELEMENT_BEGIN(AVUI::LineSegment)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Point);
REGISTER_ELEMENT_END()

namespace AVUI {

Point LineSegment::BuildGeometry(GeometryBuilder* pGB, const Point& startPoint)
{
    Point newPoint = get_Point();

    BuildGeometryStatic(pGB, startPoint, newPoint);

    return newPoint;
}

void LineSegment::BuildGeometryStatic(GeometryBuilder* pGB, const Point& startPoint, const Point& endPoint)
{
    pGB->AddPoint(endPoint.get_X(), endPoint.get_Y());
}



Rect LineSegment::GetBounds(Point* pStartPoint)
{
    Point newPoint = get_Point();

    float minX = Float::Min(pStartPoint->get_X(), newPoint.get_X());
    float maxX = Float::Max(pStartPoint->get_X(), newPoint.get_X());
    float minY = Float::Min(pStartPoint->get_Y(), newPoint.get_Y());
    float maxY = Float::Max(pStartPoint->get_Y(), newPoint.get_Y());

    *pStartPoint = newPoint;

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

int LineSegment::ClassifyPoint(const Point& pointClassify, Point* pStartPoint)
{
    int classifiedPoint = 0;
    Point newPoint = get_Point();


    classifiedPoint = StaticClassifyPoint(pointClassify, newPoint, *pStartPoint);

    *pStartPoint = newPoint;

    return classifiedPoint;
}

int LineSegment::StaticClassifyPoint(const Point& ptClassify, const Point& ptLineStart, const Point& ptLineEnd)
{
    if(ptLineStart.get_Y() < ptLineEnd.get_Y()) // edge going down
    {
        float t = (ptClassify.get_Y() - ptLineStart.get_Y()) / (ptLineEnd.get_Y() - ptLineStart.get_Y());

        if(t >= 0.0f && t <= 1.0f)
        {
            float x = ((ptLineEnd.get_X() - ptLineStart.get_X()) * t) + ptLineStart.get_X();

            if(ptClassify.get_X() < x)
            {
                return 1;
            }
        }
    }
    else if(!Float::AreClose(ptLineStart.get_Y(), ptLineEnd.get_Y())) // edge going up
    {
        float t = (ptClassify.get_Y() - ptLineEnd.get_Y()) / (ptLineStart.get_Y() - ptLineEnd.get_Y());

        if(t >= 0.0f && t <= 1.0f)
        {
            float x = ((ptLineStart.get_X() - ptLineEnd.get_X()) * t) + ptLineEnd.get_X();

            if(ptClassify.get_X() < x)
            {
                return -1;
            }
        }
    }
    return 0;
}

bool LineSegment::IsInThickenedLineSegment(const Point& ptClassify, const Point& ptLineStart, const Point& ptLineEnd, float strokeThickness)
{
    float minX = Float::Min(ptLineStart.get_X(), ptLineEnd.get_X());

    if(ptClassify.get_X() < (minX - (strokeThickness / 2.0f)))
    {
        return false;
    }

    float maxX = Float::Max(ptLineStart.get_X(), ptLineEnd.get_X());

    if(ptClassify.get_X() > (maxX + (strokeThickness / 2.0f)))
    {
        return false;
    }

    float minY = Float::Min(ptLineStart.get_Y(), ptLineEnd.get_Y());

    if(ptClassify.get_Y() < (minY - (strokeThickness / 2.0f)))
    {
        return false;
    }

    float maxY = Float::Max(ptLineStart.get_Y(), ptLineEnd.get_Y());

    if(ptClassify.get_Y() > (maxY + (strokeThickness / 2.0f)))
    {
        return false;
    }

    // Requires point to line check here, possibly something more complicated in the case of miter/miter limiting

    return true;
}



}; // namespace AVUI
