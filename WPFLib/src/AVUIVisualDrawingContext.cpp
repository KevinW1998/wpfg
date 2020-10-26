#include <AVUICommon.h>
#include <AVUIVisualDrawingContext.h>
#include <AVUIVisual.h>
#include <AVUIRenderData.h>
#include <AVUIPen.h>
#include <AVUIBrush.h>
#include <AVUIImageSource.h>
#include <AVUIGeometry.h>
#include <AVUITransform.h>

REGISTER_ELEMENT_BEGIN(AVUI::VisualDrawingContext)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::DrawingContext)

REGISTER_ELEMENT_END()

namespace AVUI {

VisualDrawingContext::VisualDrawingContext() : m_pVisual(NULL), m_pRenderData(NULL)
{
}

VisualDrawingContext::VisualDrawingContext(Visual* pVisual) : m_pVisual(pVisual), m_pRenderData(NULL)
{
}

VisualDrawingContext::~VisualDrawingContext()
{
    if(m_pVisual != NULL)
    {
        if(!IsIdenticalRenderData(m_pVisual->get_RenderData(), m_pRenderData))
        {
            m_pVisual->set_RenderData(m_pRenderData);
        }
    }
    else
    {
        ClearRenderData();
    }
    m_pRenderData = NULL;
}

bool VisualDrawingContext::IsIdenticalRenderData(RenderData* pRenderData1, RenderData* pRenderData2)
{
    if(pRenderData1 == pRenderData2)
    {
        return true;
    }

    if(pRenderData1 == NULL || pRenderData2 == NULL)
    {
        return false;
    }

    RenderData::RenderDataCommand* pCommand1 = pRenderData1->get_FirstCommand();
    RenderData::RenderDataCommand* pCommand2 = pRenderData2->get_FirstCommand();

    while(pCommand1 != NULL && pCommand2 != NULL)
    {
        if(!pCommand1->IsIdenticalTo(pCommand2))
        {
            return false;
        }

        if(pCommand1->get_CommandType() != pCommand2->get_CommandType())
        {
            return false;
        }

        pCommand1 = pRenderData1->get_NextCommand(pCommand1);
        pCommand2 = pRenderData2->get_NextCommand(pCommand2);
    }

    if(pCommand1 != NULL || pCommand2 != NULL)
    {
        return false;
    }

    return true;
}

void VisualDrawingContext::DrawEllipse(Brush* pBrush, Pen* pPen, const Point& center, float radiusX, float radiusY)
{
    EnsureRenderData();

    m_pRenderData->WriteDataRecord(RenderData::DrawEllipseCmd(pBrush, pPen, center, radiusX, radiusY));

}

void VisualDrawingContext::DrawLine(Pen* pPen, const Point& point0, const Point& point1)
{
    EnsureRenderData();

    m_pRenderData->WriteDataRecord(RenderData::DrawLineCmd(pPen, point0, point1));
}
void VisualDrawingContext::DrawRoundedRectangle(Brush* pBrush, Pen* pPen, const Rect& rectangle, float radiusX, float radiusY)
{
    EnsureRenderData();

    m_pRenderData->WriteDataRecord(RenderData::DrawRoundedRectangleCmd(pBrush, pPen, rectangle, radiusX, radiusY));
}

void VisualDrawingContext::DrawGeometry(Brush* pBrush, Pen* pPen, Geometry* pGeometry, Transform* pTransform)
{
    EnsureRenderData();

    m_pRenderData->WriteDataRecord(RenderData::DrawGeometryCmd(pBrush, pPen, pGeometry, pTransform));
}

void VisualDrawingContext::DrawFormattedText(Brush* pBrush, FormattedText* pFormattedText, const Point& origin)
{
    EnsureRenderData();

    m_pRenderData->WriteDataRecord(RenderData::DrawTextCmd(m_pRenderData, pFormattedText, pBrush, origin));
}

void VisualDrawingContext::EnsureRenderData()
{
    if(m_pRenderData == NULL)
    {
        m_pRenderData = object_allocate<RenderData>();
    }
}

void VisualDrawingContext::ClearRenderData()
{
    m_pRenderData = NULL;
}



}; // namespace AVUI
