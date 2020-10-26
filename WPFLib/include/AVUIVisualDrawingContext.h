#include <AVUIDrawingContext.h>

namespace AVUI {

class RenderData;

class Visual;

class VisualDrawingContext : public DrawingContext
{
public:
    VisualDrawingContext();
    VisualDrawingContext(Visual* pVisual);
    ~VisualDrawingContext();
    void DrawEllipse(Brush* pBrush, Pen* pPen, const Point& center, float radiusX, float radiusY);
    void DrawLine(Pen* pPen, const Point& point0, const Point& point1);
    void DrawRoundedRectangle(Brush* pBrush, Pen* pPen, const Rect& rectangle, float radiusX, float radiusY); 
    void DrawGeometry(Brush* pBrush, Pen* pPen, Geometry* pGeometry, Transform* pTransform);
    void DrawFormattedText(Brush* pBrush, FormattedText* pFormattedText, const Point& origin);

    DECLARE_ELEMENT(VisualDrawingContext, DrawingContext);

private:
    static bool IsIdenticalRenderData(RenderData* pRenderData1, RenderData* pRenderData2);
    void EnsureRenderData();
    void ClearRenderData();

    TRefCountedPtr<RenderData> m_pRenderData;
    Visual* m_pVisual;
};

 


}; // namespace AVUI
