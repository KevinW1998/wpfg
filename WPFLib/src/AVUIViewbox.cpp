#include <AVUICommon.h>
#include <AVUIViewbox.h>
#include <AVUIScaleTransform.h>

REGISTER_ELEMENT_BEGIN(AVUI::Viewbox)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Stretch);
    REGISTER_PROPERTY(StretchDirection);
REGISTER_ELEMENT_END()

namespace AVUI {

Size Viewbox::MeasureOverride(const Size& constraintSize)
{
    UIElement* pChild = get_Child();
    Size size;

    if (pChild != NULL)
    {
        Size availableSize(Float::get_PositiveInfinity(), Float::get_PositiveInfinity());
        pChild->Measure(availableSize);
        Size desiredSize = pChild->get_DesiredSize();
        Size size4 = ComputeScaleFactor(constraintSize, desiredSize, get_Stretch(), get_StretchDirection());
        size.set_Width(size4.get_Width() * desiredSize.get_Width());
        size.set_Height(size4.get_Height() * desiredSize.get_Height());
    }
    return size;
}

Size Viewbox::ArrangeOverride(const Size& finalSize)
{    
    Size arrangeSize(finalSize);
    UIElement* pChild = get_Child();
    if (pChild != NULL)
    {
        Size desiredSize = pChild->get_DesiredSize();
        Size size2 = ComputeScaleFactor(arrangeSize, desiredSize, get_Stretch(), get_StretchDirection());

        // Deviation -- we set the render transform, not on a 'positioning' container visual
        set_RenderTransform(object_allocate<ScaleTransform>(size2.get_Width(), size2.get_Height()));
        pChild->Arrange(Rect(Point(), pChild->get_DesiredSize()));
        arrangeSize.set_Width(size2.get_Width() * desiredSize.get_Width());
        arrangeSize.set_Height(size2.get_Height() * desiredSize.get_Height());
    }
    return arrangeSize;
}


Size Viewbox::ComputeScaleFactor(const Size& availableSize, const Size& contentSize, Stretch::Enum stretch, StretchDirection::Enum stretchDirection)
{
    float width = 1.0f;
    float height = 1.0f;
    bool flag = !Float::IsPositiveInfinity(availableSize.get_Width());
    bool flag2 = !Float::IsPositiveInfinity(availableSize.get_Height());
    if ((((stretch != Stretch::Uniform) && (stretch != Stretch::UniformToFill)) && (stretch != Stretch::Fill)) || (!flag && !flag2))
    {
        return Size(width, height);
    }

    width = Float::IsZero(contentSize.get_Width()) ? 0.0f : (availableSize.get_Width() / contentSize.get_Width());
    height = Float::IsZero(contentSize.get_Height()) ? 0.0f : (availableSize.get_Height() / contentSize.get_Height());

    if (!flag)
    {
        width = height;
    }
    else if (!flag2)
    {
        height = width;
    }
    else
    {
        switch (stretch)
        {
        case Stretch::Fill:
                break;

        case Stretch::Uniform:
            {
                float num3 = (width < height) ? width : height;
                width = height = num3;
                break;
            }
        case Stretch::UniformToFill:
            {
                float num4 = (width > height) ? width : height;
                width = height = num4;
                break;
            }
        }
    }

    switch (stretchDirection)
    {
    case StretchDirection::UpOnly:
            if (width < 1.0f)
            {
                width = 1.0f;
            }
            if (height < 1.0f)
            {
                height = 1.0f;
            }
            break;

    case StretchDirection::DownOnly:
            if (width > 1.0f)
            {
                width = 1.0f;
            }
            if (height > 1.0f)
            {
                height = 1.0f;
            }
            break;

    case StretchDirection::Both:
            break;
    }

    return Size(width, height);
}





}; // namespace AVUI

