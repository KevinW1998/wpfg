#pragma once

#include <AVUIPanel.h>

namespace AVUI {

class DockPanel : public Panel
{
public:
    DockPanel() { };
    ~DockPanel() { };

    DECLARE_ELEMENT(DockPanel, Panel);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(Dock,  Dock, Dock::Left, DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnDockChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(LastChildFill,   bool, true, PropertyFlag::AffectsArrange);

    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

    static Dock::Enum GetDock(UIElement* pElement) { return (Dock::Enum) UnboxValue<int>(pElement->GetValue(get_DockProperty())); }
    static void SetDock(UIElement* pElement, Dock::Enum dock) { return pElement->SetValue(get_DockProperty(), BoxValue<int>(dock)); }

private:

    static void OnDockChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

};


}; // namespace AVUI
