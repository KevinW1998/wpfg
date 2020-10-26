#include <AVUICommon.h>
#include <AVUIPathGeometry.h>
#include <AVUIPathFigureCollection.h>
#include <AVUIGeometryBuilder.h>

REGISTER_ELEMENT_BEGIN(AVUI::PathGeometry)

    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(FillRule);
    REGISTER_PROPERTY(Figures);

REGISTER_ELEMENT_END()

namespace AVUI {

void PathGeometry::BuildGeometry(GeometryBuilder* pGB)
{
    pGB->BeginPathGeometry();

    pGB->EndPathGeometry();
}

void PathGeometry::BuildGeometryCommon(GeometryBuilder* pGB)
{
    PathFigureCollection* pFigures = get_Figures();
    if(pFigures != NULL)
    {
        for(int index = 0; index < pFigures->get_Count(); index++)
        {
            PathFigure* pFigure = (*pFigures)[index];

            pFigure->BuildGeometry(pGB);
        }
    }
}



Rect PathGeometry::GetBounds()
{
    GeometryBuilder gb;
    gb.BeginBboxGeometry();

    BuildGeometryCommon(&gb);

    return gb.EndBboxGeometry();
}

bool PathGeometry::HitTest(const Point& pt, int hitTestFlags, float strokeThickness)
{
    GeometryBuilder gb;

    gb.set_StrokeThickness(strokeThickness);
    gb.set_FillRule(get_FillRule());

    gb.BeginHitTestGeometry(pt, (HitTestFlags::Enum) hitTestFlags);

    BuildGeometryCommon(&gb);

    return gb.EndHitTestGeometry();
}




}; // namespace AVUI
