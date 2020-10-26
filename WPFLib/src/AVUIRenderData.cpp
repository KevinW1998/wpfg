#include <AVUICommon.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIRenderData.h>
#include <AVUIGeometryBuilder.h>
#include <AVUIRenderer.h>
#include <AVUIBrush.h>
#include <AVUIPen.h>
#include <AVUIGeometry.h>
#include <AVUIFormattedText.h>
#include <AVUIApplication.h>
#include <AVUITransform.h>
#include <AVUIVisual.h>
#include <AVUIRectangleGeometry.h>
#include <AVUIEllipseGeometry.h>

namespace AVUI {

RenderData::RenderData() : m_cCount(0), m_cCapacity(0), m_pbData(NULL), m_cCmds(0), m_pCommandLastText(NULL), m_geometryScale(0.0f), m_containsRenderDependencies(false)
{
    
}

RenderData::~RenderData()
{
    RenderDataCommand* pCommand = get_FirstCommand();
    while(pCommand != NULL)
    {
        pCommand->Clear();
        pCommand = get_NextCommand(pCommand);
    }

    if(m_pbData != NULL)
    {
        Object::FreeMemory(m_pbData);
        m_pbData = NULL;
    }
}

const Rect& RenderData::get_ContentBounds()
{
    return m_contentBounds;
}

bool RenderData::HitTestPoint(const Point& point)
{
    RenderDataCommand* pCommand = get_FirstCommand();
    while(pCommand != NULL)
    {   
        if(pCommand->IsPointInside(point))
        {
            return true;
        }
        pCommand = get_NextCommand(pCommand);
    }

    return false;
}

void RenderData::ClearClientGeometry()
{
    m_geometryScale = 0.0f;

    RenderDataCommand* pCommand = get_FirstCommand();
    while(pCommand != NULL)
    {   
        pCommand->ClearClientGeometry();
        pCommand = get_NextCommand(pCommand);
    }
}


void RenderData::ClearRenderData()
{
    m_cCount = 0;
    m_cCmds = 0;
}

void RenderData::WriteDataRecord(const RenderDataCommand& cmd)
{
    if(!m_containsRenderDependencies && cmd.ContainsRenderDependencies())
    {
        m_containsRenderDependencies = true;
    }

    if((m_cCount + cmd.get_Len() + sizeof(unsigned int)) > m_cCapacity)
    {
        const int c_defaultCapacity = 1024;

        if(m_cCapacity == 0)
        {
            m_cCapacity = c_defaultCapacity;
        }
        else
        {
            m_cCapacity *= 2;
        }

        unsigned char* pbNewData = (unsigned char*) Object::AllocateMemory(m_cCapacity);
        unsigned char* pbOldData = m_pbData;

        if(pbOldData != NULL)
        {
            memcpy(pbNewData, m_pbData, m_cCount);
            Object::FreeMemory(pbOldData);
        }

        m_pbData = pbNewData;
    }


    unsigned int cbLen = cmd.get_Len();

    memcpy(m_pbData + m_cCount, &cbLen, sizeof(cbLen));
    m_cCount += sizeof(cbLen);

    memcpy(m_pbData + m_cCount, cmd.get_Data(), cbLen);
    m_cCount += cbLen;


    if(m_cCmds == 0)
    {
        m_contentBounds = cmd.get_Bounds();
    }
    else
    {
        m_contentBounds = Rect::Union(m_contentBounds, cmd.get_Bounds());
    }

    m_cCmds++;
}


RenderData::RenderDataCommand* RenderData::get_FirstCommand()
{
    if(m_cCmds == 0)
    {
        return NULL;
    }

    return (RenderData::RenderDataCommand*) (m_pbData + sizeof(unsigned int));
}

RenderData::RenderDataCommand* RenderData::get_NextCommand(RenderData::RenderDataCommand* pPrevious)
{
    if(pPrevious == NULL)
    {
        return get_FirstCommand();
    }

    unsigned char* pbData = (unsigned char*) pPrevious;
    unsigned int cbData = *(unsigned int*) (pbData - sizeof(unsigned int));

    if( ((unsigned int) ((pbData - m_pbData) + cbData)) == m_cCount)
    {
        return NULL;
    }

    return (RenderData::RenderDataCommand*) (pbData + cbData + sizeof(unsigned int));
}


void RenderData::BuildGeometry(GeometryBuilder* pGeometryBuilder)
{
    if(Float::AreClose(m_geometryScale, pGeometryBuilder->get_GeometryScale()))
    {
        return;
    }

    RenderDataCommand* pCommand = get_FirstCommand();

    while(pCommand != NULL)
    {
        pCommand->BuildGeometry(pGeometryBuilder);
        pCommand = get_NextCommand(pCommand);
    }

    m_geometryScale = pGeometryBuilder->get_GeometryScale();
}

void RenderData::Render(Renderer* pRenderer)
{
    RenderDataCommand* pCommand = get_FirstCommand();

    while(pCommand != NULL)
    {
        pCommand->Render(pRenderer);
        pCommand = get_NextCommand(pCommand);
    }
}

bool RenderData::DrawEllipseCmd::IsPointInside(const Point& pt) const
{
    // TBD - Real hit test
    if(m_pBrush != NULL)
    {
        return true;
    }

    return false;
}

bool RenderData::DrawEllipseCmd::IsIdenticalTo(RenderDataCommand* pCommand) const
{
    RenderData::DrawEllipseCmd* pDrawEllipseCmd = (RenderData::DrawEllipseCmd*)pCommand;

    if(pDrawEllipseCmd->m_center != m_center)
    {
        return false;
    }

    if(pDrawEllipseCmd->m_radiusX != m_radiusX)
    {
        return false;
    }

    if(pDrawEllipseCmd->m_radiusY != m_radiusY)
    {
        return false;
    }

    if(!RenderGeometryDataCommand::IsIdenticalTo(pCommand))
    {
        return false;
    }

    return true;
}


bool RenderData::DrawRoundedRectangleCmd::IsPointInside(const Point& pt) const
{
    bool hitTestStroke = m_pPen != NULL && m_pPen->get_Thickness() > 0.0f;
    bool hitTestFill = m_pBrush != NULL;

    if(!hitTestFill && !hitTestStroke)
    {
        return false;
    }

    GeometryBuilder gb;

    HitTestFlags::Enum hitTestFlags = HitTestFlags::None;
    if(hitTestStroke)
    {
        hitTestFlags = (HitTestFlags::Enum) (hitTestFlags | HitTestFlags::HitTestStroke);
        gb.set_StrokeThickness(m_pPen->get_Thickness());
    }

    if(hitTestFill)
    {
        hitTestFlags = (HitTestFlags::Enum) (hitTestFlags | HitTestFlags::HitTestFill);
    }

    gb.BeginHitTestGeometry(pt, hitTestFlags);
    gb.BeginPathFigure();
    RectangleGeometry::BuildGeometryStatic(&gb, get_RadiusX(), get_RadiusY(), get_Rect());
    gb.EndPathFigure();

    return gb.EndHitTestGeometry();
}

bool RenderData::DrawGeometryCmd::IsPointInside(const Point& pt) const
{
    bool hitTestStroke = m_pPen != NULL && m_pPen->get_Thickness() > 0.0f;
    bool hitTestFill = m_pBrush != NULL;

    if(!hitTestFill && !hitTestStroke)
    {
        return false;
    }

    Point hitTestPt = pt;

    if(m_pTransform != NULL)
    {
        Matrix transformValue = m_pTransform->get_Value();

        transformValue.Invert();

        hitTestPt = transformValue.Transform(hitTestPt);
    }

    float strokeThickness = 0.0f;
    HitTestFlags::Enum hitTestFlags = HitTestFlags::None;
    if(hitTestStroke)
    {
        hitTestFlags = (HitTestFlags::Enum) (hitTestFlags | HitTestFlags::HitTestStroke);
        strokeThickness = m_pPen->get_Thickness();
    }

    if(hitTestFill)
    {
        hitTestFlags = (HitTestFlags::Enum) (hitTestFlags | HitTestFlags::HitTestFill);
    }

    return m_pGeometry->HitTest(hitTestPt, hitTestFlags, strokeThickness);
}

bool RenderData::DrawLineCmd::IsPointInside(const Point& pt) const
{
    return false;
}



void RenderData::DrawEllipseCmd::BuildGeometry(GeometryBuilder* pGeometryBuilder)
{
    ClearClientGeometry();

    if(m_pPen != NULL)
    {
        pGeometryBuilder->BeginPathGeometry();
        pGeometryBuilder->BeginPathFigure();

        EllipseGeometry::BuildGeometryStatic(pGeometryBuilder, get_RadiusX(), get_RadiusY(), get_Center());

        pGeometryBuilder->EndPathFigure();
        pGeometryBuilder->EndPathGeometry();

        m_pRenderGeometryStroke = pGeometryBuilder->CalculateStroke(m_pPen->get_Thickness());
    }

    if(m_pBrush != NULL)
    {
        pGeometryBuilder->BeginPathGeometry();
        pGeometryBuilder->BeginPathFigure();

        EllipseGeometry::BuildGeometryStatic(pGeometryBuilder, get_RadiusX(), get_RadiusY(), get_Center());

        pGeometryBuilder->EndPathFigure();
        pGeometryBuilder->EndPathGeometry();

        m_pRenderGeometryFill = pGeometryBuilder->CalculateFill();
    }
}

void RenderData::DrawRoundedRectangleCmd::BuildGeometry(GeometryBuilder* pGeometryBuilder)
{
    ClearClientGeometry();

    if(m_pPen != NULL)
    {
        pGeometryBuilder->BeginPathGeometry();
        pGeometryBuilder->BeginPathFigure();

        RectangleGeometry::BuildGeometryStatic(pGeometryBuilder, get_RadiusX(), get_RadiusY(), get_Rect());

        pGeometryBuilder->EndPathFigure();
        pGeometryBuilder->EndPathGeometry();

        m_pRenderGeometryStroke = pGeometryBuilder->CalculateStroke(m_pPen->get_Thickness());
    }

    if(m_pBrush != NULL)
    {
        pGeometryBuilder->BeginPathGeometry();
        pGeometryBuilder->BeginPathFigure();

        RectangleGeometry::BuildGeometryStatic(pGeometryBuilder, get_RadiusX(), get_RadiusY(), get_Rect());

        pGeometryBuilder->EndPathFigure();
        pGeometryBuilder->EndPathGeometry();

        m_pRenderGeometryFill = pGeometryBuilder->CalculateFill();
    }
}

Rect RenderData::DrawRoundedRectangleCmd::get_Bounds() const
{
    float penThickness = 0.0f;

    if(m_pPen != NULL)
    {
        penThickness = m_pPen->get_Thickness();
    }

    return Rect(m_rect.get_Left() - penThickness / 2.0f, m_rect.get_Top() - penThickness / 2.0f, 
                m_rect.get_Width() + penThickness, m_rect.get_Height() + penThickness); 
}

bool RenderData::DrawRoundedRectangleCmd::IsIdenticalTo(RenderDataCommand* pCommand) const
{
    RenderData::DrawRoundedRectangleCmd* pDrawRectCmd = (RenderData::DrawRoundedRectangleCmd*)pCommand;

    if(pDrawRectCmd->m_rect != m_rect)
    {
        return false;
    }

    if(pDrawRectCmd->m_radiusX != m_radiusX)
    {
        return false;
    }

    if(pDrawRectCmd->m_radiusY != m_radiusY)
    {
        return false;
    }

    if(!RenderGeometryDataCommand::IsIdenticalTo(pCommand))
    {
        return false;
    }

    return true;
}


Rect RenderData::DrawGeometryCmd::get_Bounds() const
{
    Rect rectBounds;

    if(m_pGeometry != NULL)
    {
        rectBounds = m_pGeometry->GetBounds();

        if(m_pTransform != NULL)
        {
            rectBounds = m_pTransform->get_Value().TransformBounds(rectBounds);
        }
    }

    float penThickness = 0.0f;

    if(m_pPen != NULL)
    {
        penThickness = m_pPen->get_Thickness();
    }

    return rectBounds.Inflate(penThickness / 2.0f);
}

Rect RenderData::DrawLineCmd::get_Bounds() const
{
    float penThickness = 0.0f;

    if(m_pPen != NULL)
    {
        penThickness = m_pPen->get_Thickness();
    }

    float xMin = Float::Min(m_point0.get_X(), m_point1.get_X());
    float xMax = Float::Max(m_point0.get_X(), m_point1.get_X());

    float yMin = Float::Min(m_point0.get_Y(), m_point1.get_Y());
    float yMax = Float::Max(m_point0.get_Y(), m_point1.get_Y());

    return Rect(xMin, yMin, xMax - xMin, yMax - yMin).Inflate(penThickness);
}


void RenderData::DrawLineCmd::BuildGeometry(GeometryBuilder* pGeometryBuilder)
{
    // TBD Line Rendering
}

bool RenderData::DrawLineCmd::IsIdenticalTo(RenderDataCommand* pCommand) const
{
    RenderData::DrawLineCmd* pDrawLineCmd = (RenderData::DrawLineCmd*)pCommand;

    if(pDrawLineCmd->m_point0 != m_point0)
    {
        return false;
    }

    if(pDrawLineCmd->m_point1 != m_point1)
    {
        return false;
    }

    if(!RenderGeometryDataCommand::IsIdenticalTo(pCommand))
    {
        return false;
    }

    return true;
}


void RenderData::RenderGeometryDataCommand::Render(Renderer* pRenderer)
{
    if(m_pRenderGeometryFill != NULL)
    {
        m_pBrush->set_ActiveBrush(pRenderer);
        pRenderer->get_ClientRendererInterface()->RenderGeometry(m_pRenderGeometryFill, false);
    }

    if(m_pRenderGeometryStroke != NULL)
    {
        Brush* pBrush = m_pPen->get_Brush();
        if(pBrush != NULL)
        {
            pBrush->set_ActiveBrush(pRenderer);
            pRenderer->get_ClientRendererInterface()->RenderGeometry(m_pRenderGeometryStroke, true);
        }
    }

}

bool RenderData::RenderGeometryDataCommand::ContainsRenderDependencies() const
{
    if(m_pBrush != NULL && m_pBrush->ContainsRenderDependencies())
    {
        return true;
    }

    if(m_pPen != NULL)
    {
        const Brush* pBrush = m_pPen->get_Brush();

        if(pBrush != NULL && pBrush->ContainsRenderDependencies())
        {
            return true;
        }
    }
    return false;
}


void RenderData::RenderGeometryDataCommand::RenderDependencies(Renderer* pRenderer)
{
    if(m_pBrush != NULL)
    {
        m_pBrush->RenderDependencies(pRenderer);
    }

    if(m_pPen != NULL)
    {
        Brush* pBrush = m_pPen->get_Brush();

        if(pBrush != NULL)
        {
            pBrush->RenderDependencies(pRenderer);
        }
    }
}


RenderData::RenderGeometryDataCommand::RenderGeometryDataCommand(Brush* pBrush, Pen* pPen) : m_pPen(pPen), m_pBrush(pBrush), m_pRenderGeometryStroke(NULL), m_pRenderGeometryFill(NULL) 
{ 
    if(pBrush != NULL)
    {
        pBrush->AddRef();
    }
    if(pPen != NULL)
    {
        pPen->AddRef();
    }
}

void RenderData::RenderGeometryDataCommand::ClearClientGeometry()
{
    if(m_pRenderGeometryFill != NULL)
    {
        m_pRenderGeometryFill->Destroy();
        m_pRenderGeometryFill = NULL;
    }

    if(m_pRenderGeometryStroke != NULL)
    {
        m_pRenderGeometryStroke->Destroy();
        m_pRenderGeometryStroke = NULL;
    }
}




void RenderData::RenderGeometryDataCommand::Clear()
{
    ClearClientGeometry();

    if(m_pBrush != NULL)
    {
        m_pBrush->Release();
    }

    if(m_pPen != NULL)
    {
        m_pPen->Release();
    }
}

bool RenderData::RenderGeometryDataCommand::IsIdenticalTo(RenderDataCommand* pCommand) const
{
    RenderData::RenderGeometryDataCommand* pRenderGeometryCmd = (RenderData::RenderGeometryDataCommand*)pCommand;

    if(pRenderGeometryCmd->m_pBrush != m_pBrush)
    {
        return false;
    }

    if(pRenderGeometryCmd->m_pPen != m_pPen)
    {
        return false;
    }

    return true;
}



Rect RenderData::DrawEllipseCmd::get_Bounds() const 
{ 
    float penThickness = 0.0f;

    if(m_pPen != NULL)
    {
        penThickness = m_pPen->get_Thickness();
    }

    return Rect(m_center.get_X() - m_radiusX - penThickness / 2.0f, m_center.get_Y() - m_radiusY - penThickness / 2.0f, 
                (m_radiusX * 2.0f) + penThickness, (m_radiusY * 2.0f) + penThickness); 
}

RenderData::DrawTextCmd::DrawTextCmd(RenderData* pRenderData, FormattedText* pFormattedText, Brush* pBrush, const Point& origin) : m_pBrush(pBrush), m_origin(origin), m_pTextGeometry(NULL), m_pFormattedText(pFormattedText)
{
    pFormattedText->AddRef();
    m_bounds = Rect(origin.get_X(), origin.get_Y(), pFormattedText->get_Width(), pFormattedText->get_Height());
}

void RenderData::DrawTextCmd::BuildGeometry(GeometryBuilder* pGeometryBuilder)
{
    ClearClientGeometry();

    m_pTextGeometry = Application::get_Current()->get_ClientRendererInterface()->CreateTextGeometry();

    m_pTextGeometry->AddFormattedText(m_pFormattedText->get_ClientFormattedText(), m_origin.get_X(), m_origin.get_Y());

    m_pTextGeometry->Seal();
}

void RenderData::DrawTextCmd::ClearClientGeometry()
{
    if(m_pTextGeometry != NULL)
    {
        m_pTextGeometry->Destroy();
        m_pTextGeometry = NULL;
    }
}

bool RenderData::DrawTextCmd::ContainsRenderDependencies() const
{
    if(m_pBrush != NULL && m_pBrush->ContainsRenderDependencies())
    {
        return true;
    }
    return false;
}


void RenderData::DrawTextCmd::RenderDependencies(Renderer* pRenderer) 
{ 
    if(m_pBrush != NULL)
    {
        m_pBrush->RenderDependencies(pRenderer);
    }
}

void RenderData::DrawTextCmd::Render(Renderer* pRenderer)
{
    if(m_pBrush != NULL && m_pTextGeometry != NULL)
    {
        m_pBrush->set_ActiveBrush(pRenderer);
        pRenderer->get_ClientRendererInterface()->RenderTextGeometry(m_pTextGeometry);
    }
}

void RenderData::DrawTextCmd::Clear()
{
    if(m_pTextGeometry != NULL)
    {
        m_pTextGeometry->Destroy();
        m_pTextGeometry = NULL;
    }

    if(m_pFormattedText != NULL)
    {
        m_pFormattedText->Release();
        m_pFormattedText = NULL;
    }
}

bool RenderData::DrawTextCmd::IsIdenticalTo(RenderDataCommand* pCommand) const
{
    return false;
}


RenderData::DrawGeometryCmd::DrawGeometryCmd(Brush* pBrush, Pen* pPen, Geometry* pGeometry, Transform* pTransform) : RenderGeometryDataCommand(pBrush, pPen), m_pGeometry(pGeometry), m_pTransform(pTransform)
{
    if(m_pGeometry != NULL)
    {
        m_pGeometry->AddRef();
    }

    if(m_pTransform != NULL)
    {
        m_pTransform->AddRef();
    }
}

void RenderData::DrawGeometryCmd::Clear()
{
    if(m_pGeometry != NULL)
    {
        m_pGeometry->Release();
        m_pGeometry = NULL;
    }

    if(m_pTransform != NULL)
    {
        m_pTransform->Release();
        m_pTransform = NULL;
    }

    RenderData::RenderGeometryDataCommand::Clear();
}

void RenderData::DrawGeometryCmd::BuildGeometry(GeometryBuilder* pGeometryBuilder)
{
    ClearClientGeometry();

    if(m_pGeometry != NULL)
    {
        float oldScale = pGeometryBuilder->get_GeometryScale();
        float newScale = oldScale;

        if(m_pTransform != NULL)
        {
            newScale = newScale * Visual::CalculateGeometryScale(m_pTransform->get_Value());
            pGeometryBuilder->set_GeometryScale(newScale);
        }
        pGeometryBuilder->SetTransform(m_pTransform);

        m_pGeometry->BuildGeometry(pGeometryBuilder);

        pGeometryBuilder->ClearTransform();
        if(newScale != oldScale)
        {
            pGeometryBuilder->set_GeometryScale(oldScale);
        }
    }

    if(m_pPen != NULL)
    {
        m_pRenderGeometryStroke = pGeometryBuilder->CalculateStroke(m_pPen->get_Thickness());
    }

    Brush* pBrush = get_Brush();

    if(pBrush != NULL)
    {
        m_pRenderGeometryFill = pGeometryBuilder->CalculateFill();
    }
}

bool RenderData::DrawGeometryCmd::IsIdenticalTo(RenderDataCommand* pCommand) const
{
    RenderData::DrawGeometryCmd* pDrawGeometryCmd = (RenderData::DrawGeometryCmd*)pCommand;

    if(pDrawGeometryCmd->m_pBrush != m_pBrush)
    {
        return false;
    }

    if(pDrawGeometryCmd->m_pPen != m_pPen)
    {
        return false;
    }

    if(pDrawGeometryCmd->m_pGeometry != m_pGeometry)
    {
        return false;
    }


    if(pDrawGeometryCmd->m_pTransform != m_pTransform)
    {
        return false;
    }

    return true;
}


void RenderData::RenderDependencies(Renderer* pRenderer)
{
    RenderDataCommand* pCommand = get_FirstCommand();
    while(pCommand != NULL)
    {   
        pCommand->RenderDependencies(pRenderer);
        pCommand = get_NextCommand(pCommand);
    }
}



}; // namespace AVUI
