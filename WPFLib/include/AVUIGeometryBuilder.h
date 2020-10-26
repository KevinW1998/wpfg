#pragma once

#include <tess.h>
#include <AVUIGeometryStroker.h>
#include <AVUIMatrix.h>

namespace AVUI {

class IClientRendererInterface;
class IClientRenderGeometry;
class RenderData;
class GeometryStroker;
class Transform;

class HitTestFlags
{
public:
    enum Enum
    {
        None = 0x0,
        HitTestFill = 0x1,
        HitTestStroke = 0x2
    };
};


class GeometryBuilder : public Tess::IClientTessellatorInterface, public IStrokeSink
{
 
public:
    GeometryBuilder();
    ~GeometryBuilder();


    void set_ClientRendererInterface(IClientRendererInterface* pClientRendererInterface) { m_pClientRendererInterface = pClientRendererInterface; }
    IClientRendererInterface* get_ClientRendererInterface() { return m_pClientRendererInterface; };

    void BeginHitTestGeometry(const Point& ptToTest, HitTestFlags::Enum hitTestFlags);
    void BeginBboxGeometry();
    void BeginPathGeometry();
    void BeginPathFigure();

    void AddPoint(float x, float y);

    void EndPathFigure();
    void EndPathGeometry();
    Rect EndBboxGeometry();
    bool EndHitTestGeometry();

    IClientRenderGeometry* CalculateFill();
    IClientRenderGeometry* CalculateStroke(float thickness);

    void SetTransform(Transform* pTransform);
    void ClearTransform();

    void set_GeometryScale(float geometryScale)
    {
        m_geometryScale = geometryScale;
    }

    float get_GeometryScale() const
    {
        return m_geometryScale;
    }

    void set_StrokeThickness(float strokeThickness)
    {
        m_strokeThickness = strokeThickness;
    }

    float get_StrokeThickness() const
    {
        return m_strokeThickness;
    }

    FillRule::Enum get_FillRule() const
    {
        return m_fillRule;
    }

    FillRule::Enum set_FillRule(FillRule::Enum fillRule)
    {
        return m_fillRule = fillRule;
    }

private:

    void EnsureTessellator();

    virtual void BeginData() { };

    virtual void BeginVertexData() { };
    virtual void VertexData(float x, float y);
    virtual void EndVertexData() { };

    virtual void BeginIndexData() { };
    virtual void IndexData(int index1, int index2, int index3);
    virtual void EndIndexData() { };

    virtual void BeginContourData() { };
    virtual void BeginContour(bool isLeftFilled);
    virtual void ContourData(int index);
    virtual void EndContour();
    virtual void EndContourData() { };

    virtual void EndData() { };

    virtual unsigned int StrokeVertexData(float x, float y, bool isAAVert);
    virtual void StrokeIndexData(int one, int two, int three, bool isAA);

    struct IndexTriplet
    {
        IndexTriplet() { }
        IndexTriplet(int index1, int index2, int index3) : Index1(index1), Index2(index2), Index3(index3) { }

        int Index1;
        int Index2;
        int Index3;
    };

    struct PointWithAA
    {
        PointWithAA() { }
        PointWithAA(const Point& point, bool isAA) : Location(point), IsAA(isAA) { }

        Point Location;
        bool IsAA;
    };

    struct ContourMarker
    {
        ContourMarker() { }
        ContourMarker(int contourIndex, bool isLeftFilled) : ContourIndex(contourIndex), IsLeftFilled(isLeftFilled) { }

        int ContourIndex;
        bool IsLeftFilled;
    };

    IClientRenderGeometry* GeometryFromBuffers(const ItemStructList<PointWithAA>& points, const ItemStructList<IndexTriplet>& indices,
                                               const ItemStructList<IndexTriplet>* pOptionalIndices);
    Vector CalculateNormal(const Point& pointCur, const Point& pointNext);

    class Mode
    {
    public:
        enum Enum
        {
            GeometryCalculation,
            BboxCalculation,
            HitTestCalculation
        };
    };

    Mode::Enum m_mode;
    Rect m_bbox;
    bool m_isFirstFigurePoint;
    bool m_isFirstGeometryPoint;
    Point m_previousPoint;

    Point m_pointToClassify;
    int m_classification;

    Point m_firstHitTestPoint;

    ItemStructList<Vector> m_contourNormals;
    ItemStructList<int>   m_contourIndices;
    ItemStructList<Point> m_contourPoints;
    ItemStructList<ContourMarker> m_contourMarkers;

    ItemStructList<PointWithAA> m_strokePoints;
    ItemStructList<IndexTriplet> m_strokeIndices;
    ItemStructList<IndexTriplet> m_strokeAAIndices;

    ItemStructList<PointWithAA> m_fillPoints;
    ItemStructList<IndexTriplet> m_fillIndices;

    bool m_isCurrentContourLeftFilled;

    IClientRendererInterface* m_pClientRendererInterface;

    Tess::Tessellator* m_pTessellator;

    GeometryStroker* m_pStroker;

    Matrix m_matrixTransform;
    bool m_hasTransform;
    float m_geometryScale;
    float m_strokeThickness;
    FillRule::Enum m_fillRule;
    HitTestFlags::Enum m_hitTestFlags;
    bool m_isStrokeHit;
};


}; // namespace AVUI
