#pragma once

#include <AVUIFrameworkElement.h>

namespace AVUI {

class SolidColorBrush;

class CaretElement : public FrameworkElement
{
public:
    DECLARE_ELEMENT(CaretElement, FrameworkElement);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CaretOpacity, float, 1.0f, DependencyPropertyMetadata(PropertyFlag::None, CaretElement::OnCaretOpacityChanged));

    CaretElement() : m_height(0.0f) { };

    void set_Height(float height) { m_height = height; UpdateRender(); }
    void set_Brush(Brush* pBrush) { m_pBrush = pBrush; UpdateRender(); }

    void ShowCaret();
    void HideCaret();

private:
    static void OnCaretOpacityChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e) { CaretElement* pCaret = object_cast<CaretElement>(pDO); pCaret->set_VisualOpacity(pCaret->get_CaretOpacity()); }

    void UpdateRender();

    float m_height;
    TRefCountedPtr<Brush> m_pBrush;
};

typedef TRefCountedPtr<CaretElement> CaretElementPtr;

}; // namespace AVUI
