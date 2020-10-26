#include <AVUIShape.h>

namespace AVUI {
    
class Path : public Shape
{
public:
    DECLARE_ELEMENT(Path, Shape);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Data, Geometry, NULL, PropertyFlag::AffectsMeasure | PropertyFlag::IsContentProperty);

protected:

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

private:

    Rect GetDefiningGeometryBounds();

    Size GetNaturalSize();
    Size GetStretchedRenderSize(Stretch::Enum stretch, float strokeThickness, const Size& constraintSize, const Rect& geometryBounds);
    void GetStretchMetrics(Stretch::Enum stretch, float strokeThickness, const Size& constraintSize, const Rect& geometryBounds,
                           float* pXScale, float* pYScale, float* pDx, float* pDy, Size* pStretchedSize);
    Size GetStretchedRenderSizeAndSetStretchMatrix(Stretch::Enum mode, float strokeThickness, const Size& availableSize, const Rect& geometryBounds);

    virtual void OnRender(DrawingContext& drawingContext);

    Rect m_rect;
    Geometry* m_pGeometry;
    TRefCountedPtr<Transform> m_pGeometryTransform;
};



}; // namespace AVUI
