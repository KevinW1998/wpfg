#include <AVUICommon.h>
#include <AVUIDockPanel.h>

REGISTER_ELEMENT_BEGIN(AVUI::DockPanel)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(LastChildFill);
    REGISTER_PROPERTY(Dock);
REGISTER_ELEMENT_END()

namespace AVUI {


Size DockPanel::MeasureOverride(const Size& constraintSize)
{
    UIElementCollection* pChildren = get_Children();
    float num = 0.0f;
    float num2 = 0.0f;
    float num3 = 0.0f;
    float num4 = 0.0f;
    int num5 = 0;
    int count = pChildren->get_Count();
    while (num5 < count)
    {
        UIElement* pElement = (*pChildren)[num5];
        if (pElement != NULL)
        {
            Size availableSize(Float::Max(0.0f, constraintSize.get_Width() - num3), 
                               Float::Max(0.0f, constraintSize.get_Height() - num4));

            pElement->Measure(availableSize);
            Size desiredSize = pElement->get_DesiredSize();

            switch (GetDock(pElement))
            {
            case Dock::Left:
            case Dock::Right:
                num2 = Float::Max(num2, num4 + desiredSize.get_Height());
                num3 += desiredSize.get_Width();
                break;
    
            case Dock::Top:
            case Dock::Bottom:
                num = Float::Max(num, num3 + desiredSize.get_Width());
                num4 += desiredSize.get_Height();
                break;
            }
        }
        num5++;
    }
    num = Float::Max(num, num3);
    return Size(num, Float::Max(num2, num4));
}

Size DockPanel::ArrangeOverride(const Size& arrangeSize)
{
    UIElementCollection* pChildren = get_Children();
    int count = pChildren->get_Count();
    int num2 = count - (get_LastChildFill() ? 1 : 0);
    float x = 0.0f;
    float y = 0.0f;
    float num5 = 0.0f;
    float num6 = 0.0f;
    for (int i = 0; i < count; i++)
    {
        UIElement* pElement = (*pChildren)[i];
        if (pElement != NULL)
        {
            Size desiredSize = pElement->get_DesiredSize();
            Rect finalRect(x, y, Float::Max(0.0f, arrangeSize.get_Width() - (x + num5)), 
                           Float::Max(0.0f, arrangeSize.get_Height() - (y + num6)));
            if (i < num2)
            {
                switch (GetDock(pElement))
                {
                case Dock::Left:
                    x += desiredSize.get_Width();
                    finalRect.set_Width(desiredSize.get_Width());
                    break;

                case Dock::Top:
                    y += desiredSize.get_Height();
                    finalRect.set_Height(desiredSize.get_Height());
                    break;

                case Dock::Right:
                    num5 += desiredSize.get_Width();
                    finalRect.set_X(Float::Max(0.0f, arrangeSize.get_Width() - num5));
                    finalRect.set_Width(desiredSize.get_Width());
                    break;

                case Dock::Bottom:
                    num6 += desiredSize.get_Height();
                    finalRect.set_Y(Float::Max(0.0f, arrangeSize.get_Height() - num6));
                    finalRect.set_Height(desiredSize.get_Height());
                    break;
                }
            }
            pElement->Arrange(finalRect);
        }
    }
    return arrangeSize;
}

void DockPanel::OnDockChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pReference = object_cast<UIElement>(pDO);
    if (pReference != NULL)
    {
        DockPanel* pParent = object_cast<DockPanel>(pReference->get_VisualParent());
        if (pParent != NULL)
        {
            pParent->InvalidateMeasure();
        }
    }
} 

}; // namespace AVUI
