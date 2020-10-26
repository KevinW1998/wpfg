#include <AVUICommon.h>
#include <AVUIStreamGeometry.h>
#include <AVUIStreamGeometryContext.h>
#include <AVUIStreamGeometryStream.h>
#include <AVUIGeometryBuilder.h>
#include <AVUIArcSegment.h>
#include <AVUILineSegment.h>
#include <AVUIBezierSegment.h>

REGISTER_ELEMENT_BEGIN(AVUI::StreamGeometry)

    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(FillRule);

REGISTER_ELEMENT_END()

namespace AVUI {


TRefCountedPtr<StreamGeometryContext> StreamGeometry::Open()
{
    return object_allocate<StreamGeometryContext>(this);
}

void StreamGeometry::BuildGeometry(GeometryBuilder* pGB)
{
    pGB->BeginPathGeometry();

    BuildGeometryCommon(pGB);

    pGB->EndPathGeometry();
}


void StreamGeometry::BuildGeometryCommon(GeometryBuilder* pGB)
{
    if(m_pGeometryStream != NULL)
    {
        StreamGeometryStream::BaseRecord* pRecord = NULL;
        int cbCur = 0;
        bool isFirstFigure = true;
        Point ptCur;

        while(pRecord = m_pGeometryStream->GetNextRecord(&cbCur))
        {
            if(pRecord->recordType == StreamGeometryStream::RecordType::BeginFigure)
            {
                StreamGeometryStream::BeginFigureRecord* pFigureRecord = (StreamGeometryStream::BeginFigureRecord*) pRecord;

                if(!isFirstFigure)
                {
                    pGB->EndPathFigure();
                }
                else
                {
                    isFirstFigure = false;
                }

                pGB->BeginPathFigure();

                ptCur = pFigureRecord->startPoint;
                pGB->AddPoint(ptCur.get_X(), ptCur.get_Y());
            }
            else if(pRecord->recordType == StreamGeometryStream::RecordType::ArcTo)
            {
                StreamGeometryStream::ArcToRecord* pArcRecord = (StreamGeometryStream::ArcToRecord*) pRecord;

                ArcSegment::BuildGeometryStatic(pGB, ptCur, pArcRecord->point, pArcRecord->size, 
                                                pArcRecord->rotationAngle, pArcRecord->isLargeArc,
                                                pArcRecord->sweepDirection);

                ptCur = pArcRecord->point;
            }
            else if(pRecord->recordType == StreamGeometryStream::RecordType::LineTo)
            {
                StreamGeometryStream::LineToRecord* pLineRecord = (StreamGeometryStream::LineToRecord*) pRecord;

                LineSegment::BuildGeometryStatic(pGB, ptCur, pLineRecord->point);

                ptCur = pLineRecord->point;
            }
            else if(pRecord->recordType == StreamGeometryStream::RecordType::BezierTo)
            {
                StreamGeometryStream::BezierToRecord* pBezierToRecord = (StreamGeometryStream::BezierToRecord*) pRecord;

                BezierSegment::StaticBuildGeometry(pGB, ptCur, pBezierToRecord->point1, pBezierToRecord->point2, pBezierToRecord->point3);

                ptCur = pBezierToRecord->point3;
            }
            else if(pRecord->recordType == StreamGeometryStream::RecordType::QuadraticBezierTo)
            {
                // No quadratic bezier support
                LibraryCriticalError();
            }

        }

        if(!isFirstFigure)
        {
            pGB->EndPathFigure();
        }
    }
}

Rect StreamGeometry::GetBounds()
{
    GeometryBuilder gb;
    gb.BeginBboxGeometry();

    BuildGeometryCommon(&gb);

    return gb.EndBboxGeometry();
}

bool StreamGeometry::HitTest(const Point& pt, int hitTestFlags, float strokeThickness)
{
    GeometryBuilder gb;

    gb.set_StrokeThickness(strokeThickness);
    gb.set_FillRule(get_FillRule());

    gb.BeginHitTestGeometry(pt, (HitTestFlags::Enum) hitTestFlags);

    BuildGeometryCommon(&gb);

    return gb.EndHitTestGeometry();
}



}; // namespace AVUI
