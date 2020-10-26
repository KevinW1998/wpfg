#include <AVUICommon.h>
#include <AVUIStackPanel.h>

REGISTER_ELEMENT_BEGIN(AVUI::StackPanel)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Orientation);
REGISTER_ELEMENT_END()

namespace AVUI {

StackPanel::StackPanel()
{

}

StackPanel::~StackPanel()
{

}

Size StackPanel::MeasureOverride(const Size& constraintSize)
{
    bool isHorizontal = get_Orientation() == Orientation::Horizontal;
    Size childConstraint = constraintSize;
    Size desiredSize(0.0f, 0.0f);

    if(isHorizontal)
    {
        childConstraint.set_Width(Float::get_PositiveInfinity());
    }
    else
    {
        childConstraint.set_Height(Float::get_PositiveInfinity());
    }

    UIElementCollection& children = *(get_Children());

    unsigned int count = children.get_Count();
    for(unsigned int idx = 0; idx < count; idx++)
    {
        UIElement* pElement = children[idx];

        pElement->Measure(childConstraint);

        const Size& childDesiredSize = pElement->get_DesiredSize();
        if(isHorizontal)
        {
            desiredSize.set_Width(desiredSize.get_Width() + childDesiredSize.get_Width());
            desiredSize.set_Height(Float::Max(desiredSize.get_Height(), childDesiredSize.get_Height()));
        }
        else
        {
            desiredSize.set_Width(Float::Max(desiredSize.get_Width(), childDesiredSize.get_Width()));
            desiredSize.set_Height(desiredSize.get_Height() + childDesiredSize.get_Height());
        }        
    }
    return desiredSize;

}

Size StackPanel::ArrangeOverride(const Size& arrangeSize)
{
    bool isHorizontal = get_Orientation() == Orientation::Horizontal;

    Rect childRect(arrangeSize);

    float offset = 0.0f;

    UIElementCollection& children = (*get_Children());

    unsigned int count = children.get_Count();
    for(unsigned int idx = 0; idx < count; idx++)
    {
        UIElement* pElement = children[idx];
        if (isHorizontal)
        {
            childRect.set_X(childRect.get_X() + offset);
            offset = pElement->get_DesiredSize().get_Width();
            childRect.set_Width(offset);
            childRect.set_Height(Float::Max(arrangeSize.get_Height(), pElement->get_DesiredSize().get_Height()));
        }
        else
        {
            childRect.set_Y(childRect.get_Y() + offset);
            offset = pElement->get_DesiredSize().get_Height();
            childRect.set_Height(offset);
            childRect.set_Width(Float::Max(arrangeSize.get_Width(), pElement->get_DesiredSize().get_Width()));
        }
        pElement->Arrange(childRect);
    }

    return arrangeSize;
}



}; // namespace AVUI
