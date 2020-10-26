#include <AVUICommon.h>
#include <AVUIGeometryStroker.h>

namespace AVUI {

void GeometryStroker::StrokeSegment(const Point& pointCur, const Point& pointNext, const Vector& normalCur)
{    
    if(m_thickness < 1.0f)
    {
        return;
    }

    float aaWidth = 1.0f;
    Vector normalCurrent(normalCur.get_X() * (m_thickness - 1.0f) / 2.0f, normalCur.get_Y() * (m_thickness - 1.0f) / 2.0f);
    Vector normalWithAA(normalCurrent.get_X() + aaWidth * normalCur.get_X(), normalCurrent.get_Y() + aaWidth * normalCur.get_Y());


    //  aal0                    aar0  //
    ////////////////////////////////////
    //  l0                      r0    //
    //                                //
    //  l1                      r1    //
    ////////////////////////////////////
    // aal1                     aar1  //


    Point aal0(pointCur.get_X() + normalWithAA.get_X(), pointCur.get_Y() + normalWithAA.get_Y());
    Point aal1(pointCur.get_X() - normalWithAA.get_X(), pointCur.get_Y() - normalWithAA.get_Y());

    Point l0(pointCur.get_X() + normalCurrent.get_X(), pointCur.get_Y() + normalCurrent.get_Y());
    Point l1(pointCur.get_X() - normalCurrent.get_X(), pointCur.get_Y() - normalCurrent.get_Y());

    Point r0(pointNext.get_X() + normalCurrent.get_X(), pointNext.get_Y() + normalCurrent.get_Y());
    Point r1(pointNext.get_X() - normalCurrent.get_X(), pointNext.get_Y() - normalCurrent.get_Y());

    Point aar0(pointNext.get_X() + normalWithAA.get_X(), pointNext.get_Y() + normalWithAA.get_Y());
    Point aar1(pointNext.get_X() - normalWithAA.get_X(), pointNext.get_Y() - normalWithAA.get_Y());


    unsigned int aal0Idx = m_pClientStroke->StrokeVertexData(aal0.get_X(), aal0.get_Y(), true);
    unsigned int aal1Idx = m_pClientStroke->StrokeVertexData(aal1.get_X(), aal1.get_Y(), true);

    unsigned int l0Idx = m_pClientStroke->StrokeVertexData(l0.get_X(), l0.get_Y(), false);
    unsigned int l1Idx = m_pClientStroke->StrokeVertexData(l1.get_X(), l1.get_Y(), false);

    unsigned int r0Idx = m_pClientStroke->StrokeVertexData(r0.get_X(), r0.get_Y(), false);
    unsigned int r1Idx = m_pClientStroke->StrokeVertexData(r1.get_X(), r1.get_Y(), false);

    unsigned int aar0Idx = m_pClientStroke->StrokeVertexData(aar0.get_X(), aar0.get_Y(), true);
    unsigned int aar1Idx = m_pClientStroke->StrokeVertexData(aar1.get_X(), aar1.get_Y(), true);

    if(m_hasLastIndices)
    {
        m_pClientStroke->StrokeIndexData(m_idxLastAAR0, l0Idx, m_idxLastR0, true);
        m_pClientStroke->StrokeIndexData(m_idxLastAAR0, aal0Idx, l0Idx, true);

        m_pClientStroke->StrokeIndexData(m_idxLastR0, m_idxLastR1, l1Idx, false);
        m_pClientStroke->StrokeIndexData(m_idxLastR0, l1Idx, l0Idx, false);
    }
    else
    {
        m_idxFirstAAL0 = aal0Idx;
        m_idxFirstAAL1 = aal1Idx;
        m_idxFirstL0 = l0Idx;
        m_idxFirstL1 = l1Idx;
    }

    m_hasLastIndices = true;
    m_idxLastR0 = r0Idx;
    m_idxLastR1 = r1Idx;
    m_idxLastAAR0 = aar0Idx;
    m_idxLastAAR1 = aar1Idx;


    // Top AA segment
    m_pClientStroke->StrokeIndexData(aal0Idx, l0Idx, r0Idx, true);
    m_pClientStroke->StrokeIndexData(aal0Idx, r0Idx, aar0Idx, true);

    // Line main body
    m_pClientStroke->StrokeIndexData(l0Idx, l1Idx, r1Idx, false);
    m_pClientStroke->StrokeIndexData(l0Idx, r1Idx, r0Idx, false);

    // Bottom AA segment
    m_pClientStroke->StrokeIndexData(l1Idx, aal1Idx, aar1Idx, true);
    m_pClientStroke->StrokeIndexData(l1Idx, aar1Idx, r1Idx, true);

}

bool GeometryStroker::IsCrossPositive(const Point& pointPrevious, const Point& pointCur, const Point& pointNext)
{
    Vector pnTopc(pointNext.get_X() - pointCur.get_X(), pointNext.get_Y() - pointCur.get_Y());
    Vector pcTopp(pointCur.get_X() - pointPrevious.get_X(), pointCur.get_Y() - pointPrevious.get_Y());

    return (pnTopc.get_X() * pcTopp.get_Y() - pnTopc.get_Y() * pcTopp.get_X()) > 0;
}

void GeometryStroker::StrokeContour(const ItemStructList<Point>& contourPoints, 
                                    const ItemStructList<Vector>& contourNormals,
                                    int contourStartIndex, int pointCount, int indexStart)
{
    if(pointCount == 0)
    {
        return;
    }

    m_hasLastIndices = false;

    Point pointPrevious = contourPoints[contourStartIndex + pointCount - 1];
    int lastIndex = contourStartIndex + pointCount;

    for(int index = contourStartIndex; index < lastIndex; index++)
    {
        int nextIndex = index + 1;
        if(index == lastIndex - 1)
        {
            nextIndex = contourStartIndex;
        }

        const Point& pointCur = contourPoints[index];
        const Point& pointNext = contourPoints[nextIndex];        

        bool isCrossPositive = IsCrossPositive(pointPrevious, pointCur, pointNext);

        StrokeSegment(pointCur, pointNext, contourNormals[index]);

        pointPrevious = pointCur;
    }

    m_pClientStroke->StrokeIndexData(m_idxLastAAR0, m_idxFirstL0, m_idxLastR0, true);
    m_pClientStroke->StrokeIndexData(m_idxLastAAR0, m_idxFirstAAL0, m_idxFirstL0, true);

    m_pClientStroke->StrokeIndexData(m_idxLastR0, m_idxLastR1, m_idxFirstL1, false);
    m_pClientStroke->StrokeIndexData(m_idxLastR0, m_idxFirstL1, m_idxFirstL0, false);
}

}; // namespace AVUI
