#pragma once

#include <AVUIItemStructList.h>
#include <AVUIPoint.h>

namespace AVUI {

class IStrokeSink
{
public:

    // Alternates between outside and inside of stroke
    virtual unsigned int StrokeVertexData(float x, float y, bool isAAVert) = 0;
    virtual void StrokeIndexData(int one, int two, int three, bool isAA) = 0;
}; 

class GeometryStroker
{
public:
    GeometryStroker(IStrokeSink* pClientStroke) : m_pClientStroke(pClientStroke) { }

    void StrokeContour(const ItemStructList<Point>& contourPoints, const ItemStructList<Vector>& contourNormals, int contourStartIndex, int pointCount, int indexStart);

    void set_Thickness(float thickness) { m_thickness = thickness; };
private:
    void StrokeSegment(const Point& pointCur, const Point& pointNext, const Vector& normalCur);
    bool IsCrossPositive(const Point& pointPrevious, const Point& pointCur, const Point& pointNext);

    IStrokeSink* m_pClientStroke;
    float m_thickness;

    bool m_hasLastIndices;
    unsigned int m_idxLastR0;
    unsigned int m_idxLastR1;
    unsigned int m_idxLastAAR0;
    unsigned int m_idxLastAAR1;

    unsigned int m_idxFirstL0;
    unsigned int m_idxFirstL1;
    unsigned int m_idxFirstAAL0;
    unsigned int m_idxFirstAAL1;
};

}; // namespace AVUI
