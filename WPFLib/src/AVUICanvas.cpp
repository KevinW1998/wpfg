#include <AVUICommon.h>
#include <AVUICanvas.h>

REGISTER_ELEMENT_BEGIN(AVUI::Canvas)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Left);
    REGISTER_PROPERTY(Top);
    REGISTER_PROPERTY(Right);
    REGISTER_PROPERTY(Bottom);
REGISTER_ELEMENT_END()

namespace AVUI {


Size Canvas::MeasureOverride(const Size& constraintSize)
{
    Size availableSize(Float::get_PositiveInfinity(), Float::get_PositiveInfinity());

    UIElementCollection* pChildren = get_Children();

    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        UIElement* pChild = (*pChildren)[idx];

        if(pChild != NULL)
        {
            pChild->Measure(availableSize);
        }
    }

    return Size();
}

Size Canvas::ArrangeOverride(const Size& arrangeSize)
{
    UIElementCollection* pChildren = get_Children();

    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        UIElement* pElement = (*pChildren)[idx];
        if (pElement == NULL)
        {
            continue;
        }

        float x = 0.0f;
        float y = 0.0f;
        float left = GetLeft(pElement);
        if (!Float::IsNaN(left))
        {
            x = left;
        }
        else
        {
            float right = GetRight(pElement);
            if (!Float::IsNaN(right))
            {
                x = (arrangeSize.get_Width() - pElement->get_DesiredSize().get_Width()) - right;
            }
        }
        float top = GetTop(pElement);
        if (!Float::IsNaN(top))
        {
            y = top;
        }
        else
        {
            float bottom = GetBottom(pElement);
            if (!Float::IsNaN(bottom))
            {
                y = (arrangeSize.get_Height() - pElement->get_DesiredSize().get_Height()) - bottom;
            }
        }
        pElement->Arrange(Rect(Point(x, y), pElement->get_DesiredSize()));
    }
    return arrangeSize;
}

void Canvas::OnPositioningChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pReference = object_cast<UIElement>(pDO);
    if (pReference != NULL)
    {
        Canvas* pParent = object_cast<Canvas>(pReference->get_VisualParent());
        if (pParent != NULL)
        {
            pParent->InvalidateArrange();
        }
    }
} 

bool Canvas::GetLayoutClip(const Size& layoutSlotSize, Rect* pRect)
{
    if(get_ClipToBoundsCache())
    {
        *pRect = Rect(get_RenderSize());
        return true;
    }

    return false;
} 

}; // namespace AVUI
