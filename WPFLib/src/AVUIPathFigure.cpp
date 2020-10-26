#include <AVUICommon.h>
#include <AVUIPathFigure.h>
#include <AVUIPathSegmentCollection.h>
#include <AVUIGeometryBuilder.h>

REGISTER_ELEMENT_BEGIN(AVUI::PathFigure)


    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(StartPoint)
    REGISTER_PROPERTY(IsFilled)
    REGISTER_PROPERTY(IsClosed)
    REGISTER_PROPERTY(Segments)
REGISTER_ELEMENT_END()


namespace AVUI {

void PathFigure::BuildGeometry(GeometryBuilder* pGB)
{
    PathSegmentCollection* pSegments = get_Segments();

    if(pSegments != NULL)
    {
        Point startPoint = get_StartPoint();

        pGB->BeginPathFigure();

        pGB->AddPoint(startPoint.get_X(), startPoint.get_Y());

        for(int index = 0; index < pSegments->get_Count(); index++)
        {
            startPoint = (*pSegments)[index]->BuildGeometry(pGB, startPoint);
        }

        pGB->EndPathFigure();
    }
}

Rect PathFigure::GetBounds()
{
    Rect boundsRect;
    PathSegmentCollection* pSegments = get_Segments();

    if(pSegments != NULL)
    {
        Point startPoint = get_StartPoint();

        for(int index = 0; index < pSegments->get_Count(); index++)
        {
            if(index == 0)
            {
                boundsRect = (*pSegments)[index]->GetBounds(&startPoint);
            }
            else
            {
                boundsRect = Rect::Union(boundsRect, (*pSegments)[index]->GetBounds(&startPoint));
            }
        }
    }
    return boundsRect;
}

int PathFigure::ClassifyPoint(const Point& pt)
{

    if(!GetBounds().Contains(pt))
    {
        return 0;
    }
    return 1;

    // TODO - Rationalize this with geometry builder / new method for data.
    /*

    int classifiedPoint = 0;
    PathSegmentCollection* pSegments = get_Segments();

    if(pSegments != NULL)
    {
        Point startPoint = get_StartPoint();

        for(int index = 0; index < pSegments->get_Count(); index++)
        {
            classifiedPoint += (*pSegments)[index]->ClassifyPoint(pt, &startPoint);
        }
    }
    return classifiedPoint;
    */
}

}; // namespace AVUI
