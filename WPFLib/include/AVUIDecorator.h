#pragma once

#include <AVUIFrameworkElement.h>

namespace AVUI {

class Decorator : public FrameworkElement
{
    friend class PopupRoot;
public:

    DECLARE_ELEMENT(Decorator, FrameworkElement);

    IMPLEMENT_REFERENCE_PROPERTY(Child, UIElement, true);

    Decorator() : m_isNotLogical(false) { }

    virtual unsigned int get_VisualChildCount() { return m_pChild != NULL ? 1 : 0; }
    virtual Visual* get_VisualChild(unsigned int idx) { return m_pChild; }

    UIElement* get_Child() { return m_pChild; }
    void set_Child(UIElement* pChild);

protected:

    virtual ~Decorator();

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    void set_IsNotLogical(bool isNotLogical) { m_isNotLogical = isNotLogical; }
    bool get_IsNotLogical() { return m_isNotLogical; }

    TRefCountedPtr<UIElement> m_pChild;
    bool m_isNotLogical;
};


}; // namespace AVUI
