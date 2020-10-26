#include <AVUICommon.h>
#include <AVUIArcSegment.h>
#include <AVUIGeometryBuilder.h>
#include <AVUIGeometryHelper.h>
#include <AVUIBezierSegment.h>

REGISTER_ELEMENT_BEGIN(AVUI::ArcSegment)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Point);
    REGISTER_PROPERTY(Size);
    REGISTER_PROPERTY(RotationAngle);
    REGISTER_PROPERTY(IsLargeArc);
    REGISTER_PROPERTY(SweepDirection);
REGISTER_ELEMENT_END()

namespace AVUI {

Point ArcSegment::BuildGeometry(GeometryBuilder* pGB, const Point& startPoint)
{
    Point endPoint = get_Point();

    BuildGeometryStatic(pGB, startPoint, endPoint, get_Size(), get_RotationAngle(), get_IsLargeArc(), get_SweepDirection());

    return endPoint;
}

void ArcSegment::BuildGeometryStatic(GeometryBuilder* pGB, const Point& startPoint, const Point& endPoint, 
                                     const Size& size, float rotationAngle, bool isLargeArc, SweepDirection::Enum sweepDirection)
{
    Point rgCurveData[12]; // 4 curves specify a circle
    int cCurves = 0;

    GeometryHelper::ArcToBezier(startPoint.get_X(), startPoint.get_Y(), size.get_Width(), size.get_Height(), 
                                rotationAngle, isLargeArc, sweepDirection, endPoint.get_X(), endPoint.get_Y(),
                                rgCurveData, &cCurves);

    Point curStart = startPoint;

    for(int idx = 0; idx < cCurves; idx++)
    {
        BezierSegment::StaticBuildGeometry(pGB, curStart, rgCurveData[idx * 3],
                                                          rgCurveData[idx * 3 + 1],
                                                          rgCurveData[idx * 3 + 2]);

        curStart = rgCurveData[(idx * 3) + 2];
    }
}

    // Center parameterization
void ArcSegment::BuildGeometryStaticCenter(GeometryBuilder* pGeometryBuilder, const Point& centerPoint,
                                           float radiusX, float radiusY, float angle1, float angle2, SweepDirection::Enum sweepDirection)
{
    float l2Norm = (fabs(radiusX) + fabs(radiusY)) / 2.0f;
    float angleDiff = acos(l2Norm / (l2Norm + 0.125f / pGeometryBuilder->get_GeometryScale())) * 2.0f;

    if(sweepDirection == SweepDirection::Clockwise)
    {
        while(angle2 < angle1)
        {
            angle2 += Float::get_2PI();
        }
    }
    else
    {
        while(angle1 < angle2)
        {
            angle1 += Float::get_2PI();
        }
        angleDiff = -angleDiff;
    }

    float angle = angle1 + angleDiff; // init to one position past initial

    if(angleDiff > 0.0f)
    {
        while((angle + angleDiff) < angle2)
        {
            pGeometryBuilder->AddPoint(centerPoint.get_X() + cos(angle) * radiusX, centerPoint.get_Y() + sin(angle) * radiusY);
            angle += angleDiff;
        }
    }
    else
    {
        while((angle + angleDiff) > angle2)
        {
            pGeometryBuilder->AddPoint(centerPoint.get_X() + cos(angle) * radiusX, centerPoint.get_Y() + sin(angle) * radiusY);
            angle += angleDiff;
        }
    }

    pGeometryBuilder->AddPoint(centerPoint.get_X() + cos(angle2) * radiusX, centerPoint.get_Y() + sin(angle2) * radiusY);
}



}; // namespace AVUI



