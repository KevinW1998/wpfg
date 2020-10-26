#include <AVUICommon.h>
#include <AVUIBorder.h>
#include <AVUISolidColorBrush.h>
#include <AVUIStreamGeometry.h>
#include <AVUIStreamGeometryContext.h>
#include <AVUIPen.h>

REGISTER_ELEMENT_BEGIN(AVUI::Border)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(BorderThickness);
    REGISTER_PROPERTY(Padding);
    REGISTER_PROPERTY(CornerRadius);
    REGISTER_PROPERTY(BorderBrush);
    REGISTER_PROPERTY(Background);
REGISTER_ELEMENT_END()

namespace AVUI {

Size Border::HelperCollapseThickness(const Thickness& th)
{
    return Size(th.get_Left() + th.get_Right(), th.get_Top() + th.get_Bottom());
}

Rect Border::HelperDeflateRect(const Rect& rt, const Thickness& thick)
{
    return Rect(rt.get_Left() + thick.get_Left(), 
                rt.get_Top() + thick.get_Top(), 
                Float::Max(0.0f, (rt.get_Width() - thick.get_Left() - thick.get_Right())), 
                Float::Max(0.0f, (rt.get_Height() - thick.get_Top() - thick.get_Bottom())));
}

bool Border::AreUniformCorners(const CornerRadius& borderRadii)
{
    float topLeft = borderRadii.get_TopLeft();
    if (Float::AreClose(topLeft, borderRadii.get_TopRight()) && Float::AreClose(topLeft, borderRadii.get_BottomLeft()))
    {
        return Float::AreClose(topLeft, borderRadii.get_BottomRight());
    }
    return false;
} 


Size Border::MeasureOverride(const Size& constraint)
{
    UIElement* pChild = get_Child();
    Size size;
    Size size2 = HelperCollapseThickness(get_BorderThickness());
    Size size3 = HelperCollapseThickness(get_Padding());
    if (pChild != NULL)
    {
        Size size4(size2.get_Width() + size3.get_Width(), size2.get_Height() + size3.get_Height());
        Size availableSize(Float::Max(0.0f, constraint.get_Width() - size4.get_Width()), 
                           Float::Max(0.0f, constraint.get_Height() - size4.get_Height()));
        pChild->Measure(availableSize);
        Size desiredSize = pChild->get_DesiredSize();
        size.set_Width(desiredSize.get_Width() + size4.get_Width());
        size.set_Height(desiredSize.get_Height() + size4.get_Height());
        return size;
    }
    return Size(size2.get_Width() + size3.get_Width(), size2.get_Height() + size3.get_Height());

}

Size Border::ArrangeOverride(const Size& finalSize)
{
    Thickness borderThickness = get_BorderThickness();
    Rect rt(finalSize);
    Rect rect2 = HelperDeflateRect(rt, borderThickness);
    UIElement* pChild = get_Child();
    if (pChild != NULL)
    {
        Rect finalRect = HelperDeflateRect(rect2, get_Padding());
        pChild->Arrange(finalRect);
    }
    CornerRadius cornerRadius = get_CornerRadius();
    Brush* pBorderBrush = get_BorderBrush();
    bool flag = AreUniformCorners(cornerRadius);
    m_useComplexRenderCodePath = !flag;
    if (!m_useComplexRenderCodePath && pBorderBrush != NULL)
    {
        SolidColorBrush* pBrush2 = object_cast<SolidColorBrush>(pBorderBrush);
        bool isUniform = borderThickness.get_IsUniform();
        m_useComplexRenderCodePath = (pBrush2 == NULL || (pBrush2->get_Color().get_A() < 1.0f && !isUniform) || 
                                     (!Float::IsZero(cornerRadius.get_TopLeft()) && !isUniform));
    }
    if (m_useComplexRenderCodePath)
    {
        Radii radii(cornerRadius, borderThickness, false);
        if (!Float::IsZero(rect2.get_Width()) && !Float::IsZero(rect2.get_Height()))
        {
            m_pBackgroundGeometry = object_allocate<StreamGeometry>();

            if(TRefCountedPtr<StreamGeometryContext> pContext = m_pBackgroundGeometry->Open())
            {
                GenerateGeometry(pContext, rect2, radii);
            }
        }
        else
        {
            m_pBackgroundGeometry = NULL;
        }

        if (!Float::IsZero(rt.get_Width()) && !Float::IsZero(rt.get_Height()))
        {
            Radii radii2(cornerRadius, borderThickness, true);
            m_pBorderGeometry = object_allocate<StreamGeometry>();

            if(TRefCountedPtr<StreamGeometryContext> pContext2 = m_pBorderGeometry->Open())
            {
                GenerateGeometry(pContext2, rt, radii2);
                if (m_pBackgroundGeometry != NULL)
                {
                    GenerateGeometry(pContext2, rect2, radii);
                }
            }
            return finalSize;
        }
        m_pBorderGeometry = NULL;

        return finalSize;
    }
    m_pBackgroundGeometry = NULL;
    m_pBorderGeometry = NULL;

    return finalSize;
}


void Border::OnRender(DrawingContext& dc)
{
    if (m_useComplexRenderCodePath)
    {
        Brush* pBrush;
        if (m_pBorderGeometry != NULL && ((pBrush = get_BorderBrush()) != NULL))
        {
            dc.DrawGeometry(pBrush, NULL, m_pBorderGeometry, NULL);
        }
        if (m_pBackgroundGeometry != NULL && ((pBrush = get_Background()) != NULL))
        {
            dc.DrawGeometry(pBrush, NULL, m_pBackgroundGeometry, NULL);
        }
    }
    else
    {
        Brush* pBrush2;
        Thickness borderThickness = get_BorderThickness();
        CornerRadius cornerRadius = get_CornerRadius();
        float topLeft = cornerRadius.get_TopLeft();
        bool flag = !Float::IsZero(topLeft);
        if (!borderThickness.get_IsZero() && ((pBrush2 = get_BorderBrush()) != NULL))
        {
            TRefCountedPtr<Pen> pPen = object_allocate<Pen>();
            pPen->set_Brush(pBrush2);
            pPen->set_Thickness(borderThickness.get_Left());

            if (borderThickness.get_IsUniform())
            {
                float x = pPen->get_Thickness() * 0.5f;

                dc.DrawRoundedRectangle(NULL, pPen, Rect(Point(x, x), Point(get_RenderSize().get_Width() - x, get_RenderSize().get_Height() - x)), topLeft, topLeft);
            }
            else
            {
                if (Float::GreaterThan(borderThickness.get_Left(), 0.0f))
                {
                    float num3 = pPen->get_Thickness() * 0.5f;
                    dc.DrawLine(pPen, Point(num3, 0.0f), Point(num3, get_RenderSize().get_Height()));
                }
                if (Float::GreaterThan(borderThickness.get_Right(), 0.0f))
                {
                    pPen = object_allocate<Pen>();
                    pPen->set_Brush(pBrush2);
                    pPen->set_Thickness(borderThickness.get_Right());

                    float num4 = pPen->get_Thickness() * 0.5f;
                    dc.DrawLine(pPen, Point(get_RenderSize().get_Width() - num4, 0.0f), Point(get_RenderSize().get_Width() - num4, get_RenderSize().get_Height()));
                }
                if (Float::GreaterThan(borderThickness.get_Top(), 0.0f))
                {
                    pPen = object_allocate<Pen>();
                    pPen->set_Brush(pBrush2);
                    pPen->set_Thickness(borderThickness.get_Top());

                    float y = pPen->get_Thickness() * 0.5f;
                    dc.DrawLine(pPen, Point(0.0f, y), Point(get_RenderSize().get_Width(), y));
                }
                if (Float::GreaterThan(borderThickness.get_Bottom(), 0.0f))
                {             
                    pPen = object_allocate<Pen>();
                    pPen->set_Brush(pBrush2);
                    pPen->set_Thickness(borderThickness.get_Bottom());;

                    float num6 = pPen->get_Thickness() * 0.5f;
                    dc.DrawLine(pPen, Point(0.0f, get_RenderSize().get_Height() - num6), Point(get_RenderSize().get_Width(), get_RenderSize().get_Height() - num6));
                }
            }
        }
        Brush* pBackground = get_Background();
        if (pBackground != NULL)
        {
            Point point(borderThickness.get_Left(), borderThickness.get_Top());
            Point point2(get_RenderSize().get_Width() - borderThickness.get_Right(), 
                         get_RenderSize().get_Height() - borderThickness.get_Bottom());
            if ((point2.get_X() > point.get_X()) && (point2.get_Y() > point.get_Y()))
            {
                float radiusX = 0.0f;

                if (flag)
                {
                    Radii radii(cornerRadius, borderThickness, false);
                    radiusX = radii.TopLeft;
                }

                dc.DrawRoundedRectangle(pBackground, NULL, Rect(point, point2), radiusX, radiusX);
            }
        }
    }
}

Border::Radii::Radii(const CornerRadius& radii, const Thickness& borders, bool outer)
{
    float num = 0.5f * borders.get_Left();
    float num2 = 0.5f * borders.get_Top();
    float num3 = 0.5f * borders.get_Right();
    float num4 = 0.5f * borders.get_Bottom();
    if (outer)
    {
        if (Float::IsZero(radii.get_TopLeft()))
        {
            LeftTop = TopLeft = 0.0f;
        }
        else
        {
            LeftTop = radii.get_TopLeft() + num;
            TopLeft = radii.get_TopLeft() + num2;
        }
        if (Float::IsZero(radii.get_TopRight()))
        {
            TopRight = RightTop = 0.0f;
        }
        else
        {
            TopRight = radii.get_TopRight() + num2;
            RightTop = radii.get_TopRight() + num3;
        }
        if (Float::IsZero(radii.get_BottomRight()))
        {
            RightBottom = BottomRight = 0.0f;
        }
        else
        {
            RightBottom = radii.get_BottomRight() + num3;
            BottomRight = radii.get_BottomRight() + num4;
        }
        if (Float::IsZero(radii.get_BottomLeft()))
        {
            BottomLeft = LeftBottom = 0.0;
        }
        else
        {
            BottomLeft = radii.get_BottomLeft() + num4;
            LeftBottom = radii.get_BottomLeft() + num;
        }
    }
    else
    {
        LeftTop = Float::Max((float) 0.0, (float) (radii.get_TopLeft() - num));
        TopLeft = Float::Max((float) 0.0, (float) (radii.get_TopLeft() - num2));
        TopRight = Float::Max((float) 0.0, (float) (radii.get_TopRight() - num2));
        RightTop = Float::Max((float) 0.0, (float) (radii.get_TopRight() - num3));
        RightBottom = Float::Max((float) 0.0, (float) (radii.get_BottomRight() - num3));
        BottomRight = Float::Max((float) 0.0, (float) (radii.get_BottomRight() - num4));
        BottomLeft = Float::Max((float) 0.0, (float) (radii.get_BottomLeft() - num4));
        LeftBottom = Float::Max((float) 0.0, (float) (radii.get_BottomLeft() - num));
    }
}


void Border::GenerateGeometry(StreamGeometryContext* pCtx, const Rect& rect, const Radii& radii)
{
    Point startPoint(radii.LeftTop, 0.0f);
    Point point(rect.get_Width() - radii.RightTop, 0.0f);
    Point point3(rect.get_Width(), radii.TopRight);
    Point point4(rect.get_Width(), rect.get_Height() - radii.BottomRight);
    Point point5(rect.get_Width() - radii.RightBottom, rect.get_Height());
    Point point6(radii.LeftBottom, rect.get_Height());
    Point point7(0.0f, rect.get_Height() - radii.BottomLeft);
    Point point8(0.0f, radii.TopLeft);

    if (startPoint.get_X() > point.get_X())
    {
        float num = (radii.LeftTop / (radii.LeftTop + radii.RightTop)) * rect.get_Width();
        startPoint.set_X(num);
        point.set_X(num);
    }
    if (point3.get_Y() > point4.get_Y())
    {
        float num2 = (radii.TopRight / (radii.TopRight + radii.BottomRight)) * rect.get_Height();
        point3.set_Y(num2);
        point4.set_Y(num2);
    }
    if (point5.get_X() < point6.get_X())
    {
        float num3 = (radii.LeftBottom / (radii.LeftBottom + radii.RightBottom)) * rect.get_Width();
        point5.set_X(num3);
        point6.set_X(num3);
    }
    if (point7.get_Y() < point8.get_Y())
    {
        float num4 = (radii.TopLeft / (radii.TopLeft + radii.BottomLeft)) * rect.get_Height();
        point7.set_Y(num4);
        point8.set_Y(num4);
    }
    Vector vectorOffset(rect.get_TopLeft().get_X(), rect.get_TopLeft().get_Y());
    startPoint = startPoint + vectorOffset;
    point = point + vectorOffset;
    point3 = point3 + vectorOffset;
    point4 = point4 + vectorOffset;
    point5 = point5 + vectorOffset;
    point6 = point6 + vectorOffset;
    point7 = point7 + vectorOffset;
    point8 = point8 + vectorOffset;

    pCtx->BeginFigure(startPoint, true, true);
    pCtx->LineTo(point, true, false);

    float num5 = rect.get_TopRight().get_X() - point.get_X();
    float num6 = point3.get_Y() - rect.get_TopRight().get_Y();
    if (!Float::IsZero(num5) || !Float::IsZero(num6))
    {
        pCtx->ArcTo(point3, Size(num5, num6), 0.0f, false, SweepDirection::Clockwise, true, false);
    }
    pCtx->LineTo(point4, true, false);
    num5 = rect.get_BottomRight().get_X() - point5.get_X();
    num6 = rect.get_BottomRight().get_Y() - point4.get_Y();
    if (!Float::IsZero(num5) || !Float::IsZero(num6))
    {
        pCtx->ArcTo(point5, Size(num5, num6), 0.0f, false, SweepDirection::Clockwise, true, false);
    }
    pCtx->LineTo(point6, true, false);
    num5 = point6.get_X() - rect.get_BottomLeft().get_X();
    num6 = rect.get_BottomLeft().get_Y() - point7.get_Y();
    if (!Float::IsZero(num5) || !Float::IsZero(num6))
    {
        pCtx->ArcTo(point7, Size(num5, num6), 0.0f, false, SweepDirection::Clockwise, true, false);
    }
    pCtx->LineTo(point8, true, false);
    num5 = startPoint.get_X() - rect.get_TopLeft().get_X();
    num6 = point8.get_Y() - rect.get_TopLeft().get_Y();
    if (!Float::IsZero(num5) || !Float::IsZero(num6))
    {
        pCtx->ArcTo(startPoint, Size(num5, num6), 0.0f, false, SweepDirection::Clockwise, true, false);
    }
}

}; // namespace AVUI
