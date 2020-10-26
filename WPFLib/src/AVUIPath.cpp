#include <AVUICommon.h>
#include <AVUIPath.h>
#include <AVUIGeometry.h>
#include <AVUIMatrixTransform.h>

REGISTER_ELEMENT_BEGIN(AVUI::Path)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Data);

REGISTER_ELEMENT_END()

namespace AVUI {

Size Path::MeasureOverride(const Size& constraintSize)
{
    Size measureSize;
    Stretch::Enum stretch = get_Stretch();

    if(stretch == Stretch::None)
    {
        measureSize = GetNaturalSize();
    }
    else
    {
        measureSize = GetStretchedRenderSize(stretch, GetStrokeThickness(), constraintSize, GetDefiningGeometryBounds());
    }

    return measureSize;
}

Size Path::GetNaturalSize()
{
    Geometry* pData = get_Data();

    if(pData == NULL)
    {
        return Size();
    }

    float strokeThickness = GetStrokeThickness();
    float halfStrokeThickness = strokeThickness / 2.0f;

    Rect geometryBounds = pData->GetBounds();

    geometryBounds.Inflate(halfStrokeThickness);

    return Size(Float::Max(geometryBounds.get_Right(), 0), Float::Max(geometryBounds.get_Bottom(), 0));
}
 
void Path::OnRender(DrawingContext& drawingContext)
{
    Geometry* pGeometry = get_Data();

    if(pGeometry != NULL)
    {
        drawingContext.DrawGeometry(get_Fill(), GetPen(), get_Data(), m_pGeometryTransform);
    }
}


Size Path::GetStretchedRenderSize(Stretch::Enum stretch, float strokeThickness, const Size& constraintSize, const Rect& geometryBounds)
{
    float xScale;
    float yScale;
    float dx;
    float dy;
    Size stretchedSize;

    GetStretchMetrics(stretch, strokeThickness, constraintSize, geometryBounds, &xScale, &yScale, &dx, &dy, &stretchedSize);

    return stretchedSize;
}

void Path::GetStretchMetrics(Stretch::Enum stretch, float strokeThickness, const Size& constraintSize, const Rect& geometryBounds, 
                       float* pXScale, float* pYScale, float* pDx, float* pDy, Size* pStretchedSize)
{
    if(geometryBounds.get_Width() == 0.0f || geometryBounds.get_Height() == 0.0f)
    {
        *pXScale = *pYScale = 1.0f;
        *pDx = *pDy = 0.0f;
        *pStretchedSize = Size();
    }

    float halfStrokeThickness = strokeThickness / 2.0f;
    bool isUnscaled = false;

    *pXScale = Float::Max(constraintSize.get_Width() - strokeThickness, 0.0f);
    *pYScale = Float::Max(constraintSize.get_Height() - strokeThickness, 0.0f);
    *pDx = halfStrokeThickness - geometryBounds.get_Left();
    *pDy = halfStrokeThickness - geometryBounds.get_Top();

    if (geometryBounds.get_Width() > (*pXScale * Float::get_Epsilon()))
    {
          *pXScale /= geometryBounds.get_Width();
    }
    else
    {
          *pXScale = 1.0f;
          isUnscaled = true;
    }
    if (geometryBounds.get_Height() > (*pYScale * Float::get_Epsilon()))
    {
          *pYScale /= geometryBounds.get_Height();
    }
    else
    {
          *pYScale = 1.0f;
          isUnscaled = true;
    }
    if (stretch != Stretch::Fill && !isUnscaled)
    {
          if (stretch == Stretch::Uniform)
          {
                if (*pYScale > *pXScale)
                {
                      *pYScale = *pXScale;
                }
                else
                {
                      *pXScale = *pYScale;
                }
          }
          else if (*pXScale > *pYScale)
          {
                *pYScale = *pXScale;
          }
          else
          {
                *pXScale = *pYScale;
          }
    }
    *pStretchedSize = Size((geometryBounds.get_Width() * (*pXScale)) + strokeThickness, (geometryBounds.get_Height() * (*pYScale)) + strokeThickness);
}
 
Rect Path::GetDefiningGeometryBounds()
{
    Geometry* pGeometry = get_Data();

    if(pGeometry != NULL)
    {
        return pGeometry->GetBounds();
    }

    return Rect();
}


Size Path::ArrangeOverride(const Size& finalSize)
{
    Size arrangeSize;

    Stretch::Enum stretch = get_Stretch();
    if (stretch == Stretch::None)
    {
        m_pGeometryTransform = NULL;
        arrangeSize = finalSize;
    }
    else
    {
        arrangeSize = GetStretchedRenderSizeAndSetStretchMatrix(stretch, GetStrokeThickness(), finalSize, GetDefiningGeometryBounds());
    }

    return arrangeSize;
}

Size Path::GetStretchedRenderSizeAndSetStretchMatrix(Stretch::Enum mode, float strokeThickness, const Size& availableSize, const Rect& geometryBounds)
{
    float xScale;
    float yScale;
    float dx;
    float dy;
    Size stretchedSize;
    GetStretchMetrics(mode, strokeThickness, availableSize, geometryBounds, &xScale, &yScale, &dx, &dy, &stretchedSize);

    Matrix matrix;
    matrix.ScaleAt(xScale, yScale, geometryBounds.get_X(), geometryBounds.get_Y());
    matrix.Translate(dx, dy);

    m_pGeometryTransform = object_allocate<MatrixTransform>(matrix);

    // Set this matrix.

    return stretchedSize;
}


}; // namespace AVUI
