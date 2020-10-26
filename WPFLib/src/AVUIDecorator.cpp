#include <AVUICommon.h>
#include <AVUIDecorator.h>

REGISTER_ELEMENT_BEGIN(AVUI::Decorator)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Child);
REGISTER_ELEMENT_END()


namespace AVUI {

Decorator::~Decorator()
{
    if(m_pChild != NULL)
    {
        RemoveVisualChild(m_pChild);
        m_pChild = NULL;
    }
}


Size Decorator::MeasureOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Measure(constraintSize);
            return pVisualChild->get_DesiredSize();
        }
    }
    return Size(0, 0);
}

Size Decorator::ArrangeOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Arrange(Rect(constraintSize));
        }
    }
    return constraintSize;
}

void Decorator::set_Child(UIElement* pChild)
{
    RemoveVisualChild(m_pChild);
    if(!m_isNotLogical)
    {
        RemoveLogicalChild(m_pChild);
    }

    m_pChild = pChild;

    if(!m_isNotLogical)
    {
        AddLogicalChild(m_pChild);
    }

    AddVisualChild(m_pChild);

    InvalidateMeasure();
}


}; // namespace AVUI
