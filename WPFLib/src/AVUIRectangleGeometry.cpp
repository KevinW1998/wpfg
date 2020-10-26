#include <AVUICommon.h>
#include <AVUIRectangleGeometry.h>
#include <AVUIGeometryBuilder.h>
#include <AVUIArcSegment.h>

REGISTER_ELEMENT_BEGIN(AVUI::RectangleGeometry)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(RadiusX);
    REGISTER_PROPERTY(RadiusY);
    REGISTER_PROPERTY(Rect);
REGISTER_ELEMENT_END()

namespace AVUI {


void RectangleGeometry::BuildGeometryStatic(GeometryBuilder* pGeometryBuilder, float radiusX, float radiusY, const Rect& rect)
{
    bool drawCurves = false;
    if(radiusX > 0.0f || radiusY > 0.0f)
    {
        drawCurves = true;
    }

    radiusX = Float::Min(Float::Abs(radiusX), rect.get_Width() / 2.0f);
    radiusY = Float::Min(Float::Abs(radiusY), rect.get_Height() / 2.0f);

    pGeometryBuilder->AddPoint(rect.get_Left(), rect.get_Top() + radiusY);

    if(drawCurves)
    {
        ArcSegment::BuildGeometryStaticCenter(pGeometryBuilder, Point(rect.get_Left() + radiusX, rect.get_Top() + radiusY), 
                                        radiusX, radiusY, Float::get_PI(), (3.0f * Float::get_PI()) / 2.0f, SweepDirection::Clockwise);
    }

    pGeometryBuilder->AddPoint(rect.get_Right() - radiusX, rect.get_Top());

    if(drawCurves)
    {
        ArcSegment::BuildGeometryStaticCenter(pGeometryBuilder, Point(rect.get_Right() - radiusX, rect.get_Top() + radiusY), 
                                        radiusX, radiusY, (3.0f * Float::get_PI()) / 2.0f, 0.0f, SweepDirection::Clockwise);
    }

    pGeometryBuilder->AddPoint(rect.get_Right(), rect.get_Bottom() - radiusY);

    if(drawCurves)
    {
        ArcSegment::BuildGeometryStaticCenter(pGeometryBuilder, Point(rect.get_Right() - radiusX, rect.get_Bottom() - radiusY), 
                                        radiusX, radiusY, 0.0f, 0.5f * Float::get_PI(), SweepDirection::Clockwise);
    }

    pGeometryBuilder->AddPoint(rect.get_Left() + radiusX, rect.get_Bottom());

    if(drawCurves)
    {
        ArcSegment::BuildGeometryStaticCenter(pGeometryBuilder, Point(rect.get_Left() + radiusX, rect.get_Bottom() - radiusY), 
                                        radiusX, radiusY, Float::get_PI() * 0.5f, Float::get_PI(), SweepDirection::Clockwise);
    }
}


Rect RectangleGeometry::GetBounds()
{
    return get_Rect();
}

bool RectangleGeometry::HitTest(const Point& pt, int hitTestFlags, float strokeThickness)
{
    GeometryBuilder gb;

    gb.set_StrokeThickness(strokeThickness);
    gb.set_FillRule(FillRule::NonZero);

    gb.BeginHitTestGeometry(pt, (HitTestFlags::Enum) hitTestFlags);

    BuildGeometry(&gb);

    return gb.EndHitTestGeometry();
}

void RectangleGeometry::BuildGeometry(GeometryBuilder* pGB)
{
    RectangleGeometry::BuildGeometryStatic(pGB, get_RadiusX(), get_RadiusY(), get_Rect());
}



}; // namespace AVUI
 
