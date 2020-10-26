#include <AVUICommon.h>
#include <AVUIEllipseGeometry.h>
#include <AVUIGeometryBuilder.h>

REGISTER_ELEMENT_BEGIN(AVUI::EllipseGeometry)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(RadiusX);
    REGISTER_PROPERTY(RadiusY);
    REGISTER_PROPERTY(Center);
REGISTER_ELEMENT_END()

namespace AVUI {


void EllipseGeometry::BuildGeometryStatic(GeometryBuilder* pGeometryBuilder, float radiusX, float radiusY, const Point& center)
{
    float l2Norm = (fabs(radiusX) + fabs(radiusY)) / 2.0f;
    float angleDiff = acos(l2Norm / (l2Norm + 0.125f / pGeometryBuilder->get_GeometryScale())) * 2.0f;
    unsigned int cSteps = (unsigned int) (Float::get_2PI() / angleDiff);


    for(unsigned int step = 0; step < cSteps; step++)
    {
        float angle = (step * Float::get_2PI()) / cSteps;

        pGeometryBuilder->AddPoint(center.get_X() + cos(angle) * radiusX,
                                   center.get_Y() + sin(angle) * radiusY);
    }
}


Rect EllipseGeometry::GetBounds()
{
    Point center = get_Center();
    
    return Rect(center.get_X() - get_RadiusX(), center.get_Y() - get_RadiusY(), 2.0f * get_RadiusX(), 2.0f * get_RadiusY() );
}

bool EllipseGeometry::HitTest(const Point& pt, int hitTestFlags, float strokeThickness)
{
    GeometryBuilder gb;

    gb.set_StrokeThickness(strokeThickness);
    gb.set_FillRule(FillRule::NonZero);

    gb.BeginHitTestGeometry(pt, (HitTestFlags::Enum) hitTestFlags);

    BuildGeometry(&gb);

    return gb.EndHitTestGeometry();
}

void EllipseGeometry::BuildGeometry(GeometryBuilder* pGB)
{
    EllipseGeometry::BuildGeometryStatic(pGB, get_RadiusX(), get_RadiusY(), get_Center());
}



}; // namespace AVUI
 
