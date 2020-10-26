#pragma once

#include <AVUIPanel.h>

namespace AVUI {

class Canvas : public Panel
{
public:
    Canvas() { };
    ~Canvas() { };

    DECLARE_ELEMENT(Canvas, Panel);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(Left,  float, Float::get_NaN(), DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnPositioningChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(Top,   float, Float::get_NaN(), DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnPositioningChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(Right, float, Float::get_NaN(), DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnPositioningChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(Bottom,float, Float::get_NaN(), DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnPositioningChanged));

    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

    static float GetLeft(UIElement* pElement) { return UnboxValue<float>(pElement->GetValue(get_LeftProperty())); }
    static void SetLeft(UIElement* pElement, float length) { return pElement->SetValue(get_LeftProperty(), BoxValue<float>(length)); }

    static float GetTop(UIElement* pElement) { return UnboxValue<float>(pElement->GetValue(get_TopProperty())); }
    static void SetTop(UIElement* pElement, float length) { return pElement->SetValue(get_TopProperty(), BoxValue<float>(length)); }

    static float GetRight(UIElement* pElement) { return UnboxValue<float>(pElement->GetValue(get_RightProperty())); }
    static void SetRight(UIElement* pElement, float length) { return pElement->SetValue(get_RightProperty(), BoxValue<float>(length)); }

    static float GetBottom(UIElement* pElement) { return UnboxValue<float>(pElement->GetValue(get_BottomProperty())); }
    static void SetBottom(UIElement* pElement, float length) { return pElement->SetValue(get_BottomProperty(), BoxValue<float>(length)); }

protected:

    virtual bool GetLayoutClip(const Size& layoutSlotSize, Rect* pRect);

private:

    static void OnPositioningChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

};


}; // namespace AVUI
