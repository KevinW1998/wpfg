#include <AVUICommon.h>
#include <AVUIGeometryBuilder.h>
#include <AVUIGeometryStroker.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIRenderer.h>
#include <AVUITransform.h>
#include <AVUILineSegment.h>

namespace AVUI {

#pragma warning(disable:4127)

void Renderer::set_CurrentOpacity(float newOpacity)
{ 
    if(newOpacity != m_currentOpacity)
    {
        m_currentOpacity = newOpacity; 
        m_pClientRendererInterface->set_Opacity(newOpacity);
    }
}



GeometryBuilder::GeometryBuilder() : m_pTessellator(NULL), m_pStroker(NULL), m_hasTransform(false), m_geometryScale(1.0f),
                                     m_strokeThickness(0.0f), m_fillRule(FillRule::EvenOdd), m_hitTestFlags(HitTestFlags::None), m_isStrokeHit(false)
{
}

GeometryBuilder::~GeometryBuilder()
{
    if(m_pTessellator != NULL)
    {
        delete m_pTessellator;
    }

    if(m_pStroker != NULL)
    {
        delete m_pStroker;
    }
}

void GeometryBuilder::EnsureTessellator()
{
    if(m_pTessellator == NULL)
    {
        m_pTessellator = new Tess::Tessellator(this);
        m_pStroker = new GeometryStroker(this);
    }
}


void GeometryBuilder::SetTransform(Transform* pTransform)
{
    if(pTransform != NULL)
    {
        m_hasTransform = true;
        m_matrixTransform = pTransform->get_Value();
    }
    else
    {
        m_hasTransform = false;
    }
}

void GeometryBuilder::ClearTransform()
{
    m_hasTransform = false;
}

void GeometryBuilder::BeginHitTestGeometry(const Point& ptToTest, HitTestFlags::Enum hitTestFlags)
{
    m_mode = Mode::HitTestCalculation;
    m_pointToClassify = ptToTest;
    m_classification = 0;
    m_hitTestFlags = hitTestFlags;
    m_isStrokeHit = false;
    m_isFirstGeometryPoint = true;
}

void GeometryBuilder::BeginBboxGeometry()
{
    m_mode = Mode::BboxCalculation;
    m_isFirstGeometryPoint = true;
}

void GeometryBuilder::BeginPathGeometry() 
{ 
    EnsureTessellator();

    m_pTessellator->BeginPolygon(); 

    m_contourPoints.Clear(); 
    m_contourMarkers.Clear(); 
    m_contourIndices.Clear();
    m_contourNormals.Clear();

    m_fillPoints.Clear(); 
    m_fillIndices.Clear(); 

    m_mode = Mode::GeometryCalculation;
    m_isFirstGeometryPoint = true;
}

void GeometryBuilder::EndPathGeometry() 
{ 
    m_pTessellator->EndPolygon(); 
}

Rect GeometryBuilder::EndBboxGeometry()
{
    return m_bbox;
}

bool GeometryBuilder::EndHitTestGeometry()
{
    bool isHit = false;

    if(m_hitTestFlags & HitTestFlags::HitTestFill)
    {
        if(get_FillRule() == FillRule::EvenOdd)
        {
            isHit = (Int::Abs(m_classification) % 2) != 0;
        }
        else
        {
            isHit = m_classification != 0;
        }

        if(isHit)
        {
            return true;
        }
    }

    if(m_hitTestFlags & HitTestFlags::HitTestStroke)
    {
        return m_isStrokeHit;
    }

    return false;
}

void GeometryBuilder::AddPoint(float x, float y) 
{ 
    if(m_mode == Mode::GeometryCalculation)
    {
        m_pTessellator->AddVertex(x, y);
    }
    else if(m_mode == Mode::BboxCalculation)
    {
        if(m_isFirstGeometryPoint)
        {
            m_bbox = Rect(Point(x, y), Size());
        }
        else
        {
            m_bbox = Rect::Union(m_bbox, Rect(Point(x, y), Size()));
        }
    }
    else if(m_mode == Mode::HitTestCalculation)
    {
        if(!m_isFirstFigurePoint)
        {
            if(m_hitTestFlags & HitTestFlags::HitTestFill)
            {
                m_classification += LineSegment::StaticClassifyPoint(m_pointToClassify, m_previousPoint, Point(x, y));
            }
            else if(m_hitTestFlags & HitTestFlags::HitTestStroke)
            {
                if(!m_isStrokeHit)
                {
                    m_isStrokeHit = LineSegment::IsInThickenedLineSegment(m_pointToClassify, m_previousPoint, Point(x, y), m_strokeThickness);
                }
            }
        }
        else
        {
            m_firstHitTestPoint = Point(x, y);
        }
    }

    m_isFirstFigurePoint = false;
    m_isFirstGeometryPoint = false;

    m_previousPoint = Point(x, y);
}

void GeometryBuilder::BeginPathFigure() 
{ 
    if(m_mode == Mode::GeometryCalculation)
    {
        m_pTessellator->BeginContour(); 
    }

    m_isFirstFigurePoint = true;
}

void GeometryBuilder::EndPathFigure() 
{ 
    if(m_mode == Mode::GeometryCalculation)
    {
        m_pTessellator->EndContour(); 
    }
    else if(m_mode == Mode::HitTestCalculation)
    {
        if(!m_isStrokeHit)
        {
            m_isStrokeHit = LineSegment::IsInThickenedLineSegment(m_pointToClassify, m_previousPoint, m_firstHitTestPoint, m_strokeThickness);
        }
    }
}


void GeometryBuilder::VertexData(float x, float y) 
{ 
    Point ptData(x, y);

    if(m_hasTransform)
    {
        ptData = m_matrixTransform.Transform(ptData);
    }

    m_fillPoints.AddItem(PointWithAA(ptData, false)); 
}
void GeometryBuilder::IndexData(int index1, int index2, int index3) { m_fillIndices.AddItem(IndexTriplet(index1, index2, index3));  }

void GeometryBuilder::BeginContour(bool isLeftFilled)
{
    m_isCurrentContourLeftFilled = isLeftFilled;
}

void GeometryBuilder::ContourData(int index)
{
    int indexCount = m_contourIndices.get_Count();
    int lastMarker = 0;
    if(m_contourMarkers.get_Count() > 0)
    {
        lastMarker = m_contourMarkers[m_contourMarkers.get_Count() - 1].ContourIndex;
    }
    if(indexCount > lastMarker)
    {
        m_contourNormals.AddItem(CalculateNormal(m_fillPoints[m_contourIndices[indexCount - 1]].Location, m_fillPoints[index].Location));
    }

    m_contourIndices.AddItem(index);
    m_contourPoints.AddItem(m_fillPoints[index].Location);

}

Vector GeometryBuilder::CalculateNormal(const Point& pointCur, const Point& pointNext)
{
    Vector normalCurrent(pointNext.get_Y() - pointCur.get_Y(), -(pointNext.get_X() - pointCur.get_X()));

    if(Float::Abs(normalCurrent.get_X()) >= 0.001f || Float::Abs(normalCurrent.get_Y()) >= 0.001f)
    {
        normalCurrent.Normalize();
    }

    return normalCurrent;
}

void GeometryBuilder::EndContour()
{
    int contourStart = 0;

    if(m_contourMarkers.get_Count() > 0)
    {
        contourStart = m_contourMarkers[m_contourMarkers.get_Count() - 1].ContourIndex;
    }

    m_contourMarkers.AddItem(ContourMarker(m_contourPoints.get_Count(), m_isCurrentContourLeftFilled));

    m_contourNormals.AddItem(CalculateNormal(m_fillPoints[m_contourIndices[m_contourIndices.get_Count() - 1]].Location, m_fillPoints[m_contourIndices[contourStart]].Location));
}

unsigned int GeometryBuilder::StrokeVertexData(float x, float y, bool isAA)  { m_strokePoints.AddItem(PointWithAA(Point(x, y), isAA)); return m_strokePoints.get_Count() - 1; }
void GeometryBuilder::StrokeIndexData(int index1, int index2, int index3, bool isAA) 
{ 
    if(isAA)
    {
        m_strokeAAIndices.AddItem(IndexTriplet(index1, index2, index3)); 
    }
    else
    {
        m_strokeIndices.AddItem(IndexTriplet(index1, index2, index3)); 
    }
}

IClientRenderGeometry* GeometryBuilder::CalculateFill()
{
    int cFillPoints = m_fillPoints.get_Count();

    if(cFillPoints == 0)
    {
        return NULL;
    }

    int contourStart = 0;
    for(int markerIdx = 0; markerIdx < m_contourMarkers.get_Count(); markerIdx++)
    {
        // This is contourStart + contourCount
        int contourLast = m_contourMarkers[markerIdx].ContourIndex;
        bool isLeftFilled = m_contourMarkers[markerIdx].IsLeftFilled;

        Vector normalPrev;
        PointWithAA pointPrev;

        if(contourLast == contourStart)
        {
            continue;
        }

        normalPrev = m_contourNormals[contourLast - 1];
        pointPrev = m_fillPoints[m_contourIndices[contourLast - 1]];
        cFillPoints = m_fillPoints.get_Count();

        for(int idx = contourStart; idx < contourLast; idx++)
        {
            int contourIdx = m_contourIndices[idx];
            Vector normalCur = m_contourNormals[idx];
            PointWithAA newPoint = m_fillPoints[contourIdx];

            Vector totalNormal(normalCur.get_X() + normalPrev.get_X(), normalCur.get_Y() + normalPrev.get_Y());

            if(isLeftFilled)
            {
                totalNormal = Vector(-totalNormal.get_X(), -totalNormal.get_Y());
            }

            static volatile float aagenLenExtrude = 0.25f;
            static volatile float aagenLenIntrude = 0.25f;

            Point ptIntruded(newPoint.Location.get_X() + (totalNormal.get_X() * (aagenLenIntrude / m_geometryScale)),
                             newPoint.Location.get_Y() + (totalNormal.get_Y() * (aagenLenIntrude / m_geometryScale)));

            Point ptExtruded(newPoint.Location.get_X() - totalNormal.get_X() * (aagenLenExtrude / m_geometryScale), 
                             newPoint.Location.get_Y() - totalNormal.get_Y() * (aagenLenExtrude / m_geometryScale) );

            m_fillPoints[contourIdx] = PointWithAA(ptIntruded, false);
            m_fillPoints.AddItem(PointWithAA(ptExtruded, true));

            if(idx != contourLast - 1)
            {
                m_fillIndices.AddItem(IndexTriplet(m_fillPoints.get_Count() - 1, m_contourIndices[idx + 1], m_contourIndices[idx]));
                m_fillIndices.AddItem(IndexTriplet(m_fillPoints.get_Count() - 1, m_fillPoints.get_Count(), m_contourIndices[idx + 1]));
            }
            else
            {
                m_fillIndices.AddItem(IndexTriplet(m_fillPoints.get_Count() - 1, m_contourIndices[contourStart], m_contourIndices[idx]));
                m_fillIndices.AddItem(IndexTriplet(m_fillPoints.get_Count() - 1, cFillPoints, m_contourIndices[contourStart]));
            }

            normalPrev = normalCur;
            pointPrev = newPoint;
        }


        contourStart = contourLast;
    }

    return GeometryFromBuffers(m_fillPoints, m_fillIndices, NULL);
}

IClientRenderGeometry* GeometryBuilder::CalculateStroke(float thickness)
{
    if(Float::IsZero(thickness))
    {
        return NULL;
    }

    Point* rgPoints = m_contourPoints.get_Ptr();

    m_strokePoints.Clear(); 
    m_strokeIndices.Clear();
    m_strokeAAIndices.Clear();

    int contourMarkerPrev = 0;
    for(int index = 0; index < m_contourMarkers.get_Count(); index++)
    {
        int contourMarker = m_contourMarkers[index].ContourIndex;
        int pointsOld = m_strokePoints.get_Count();

        m_pStroker->set_Thickness(thickness);
        m_pStroker->StrokeContour(m_contourPoints, m_contourNormals, contourMarkerPrev, contourMarker - contourMarkerPrev, m_strokePoints.get_Count());

        contourMarkerPrev = contourMarker;                                  
    }

    return GeometryFromBuffers(m_strokePoints, m_strokeIndices, &m_strokeAAIndices);
}

IClientRenderGeometry* GeometryBuilder::GeometryFromBuffers(const ItemStructList<PointWithAA>& points, const ItemStructList<IndexTriplet>& indices, 
                                                            const ItemStructList<IndexTriplet>* pOptionalIndices)
{
    if(points.get_Count() == 0 || indices.get_Count() == 0)
    {
        return NULL;
    }

    IClientRenderGeometry* pGeometry = m_pClientRendererInterface->CreateGeometry();

    for(int i = 0; i < points.get_Count(); i++)
    {
        pGeometry->AddVertex(points[i].Location.get_X(), points[i].Location.get_Y(), points[i].IsAA);
    }

    for(int i = 0; i < indices.get_Count(); i++)
    {
        const IndexTriplet& indexTriplet = indices[i];

        pGeometry->AddIndexTriplet(indexTriplet.Index1, indexTriplet.Index2, indexTriplet.Index3);
    }

    if(pOptionalIndices != NULL)
    {
        for(int i = 0; i < pOptionalIndices->get_Count(); i++)
        {
            const IndexTriplet& indexTriplet = (*pOptionalIndices)[i];

            pGeometry->AddIndexTriplet(indexTriplet.Index1, indexTriplet.Index2, indexTriplet.Index3);
        }        
    }

    pGeometry->Seal();

    return pGeometry;
}


}; // namespace AVUI
