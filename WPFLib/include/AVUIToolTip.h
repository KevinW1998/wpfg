#pragma once

#include <AVUIContentControl.h>

namespace AVUI {

class Popup;

class ToolTip : public ContentControl
{
public:
    ToolTip() { }
    ~ToolTip() { };

    DECLARE_ELEMENT(ToolTip, ContentControl);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsOpen, bool, false, DependencyPropertyMetadata(PropertyFlag::None, ToolTip::OnIsOpenChanged));

    static const AVUI::DependencyProperty& get_HorizontalOffsetProperty();
    float get_HorizontalOffset() { return UnboxValue< float >(GetValue(get_HorizontalOffsetProperty())); }
    void set_HorizontalOffset(const float& value) { SetValue(get_HorizontalOffsetProperty(), AVUI::BoxValue(value)); }

    static const AVUI::DependencyProperty& get_VerticalOffsetProperty();
    float get_VerticalOffset() { return UnboxValue< float >(GetValue(get_VerticalOffsetProperty())); }
    void set_VerticalOffset(const float& value) { SetValue(get_VerticalOffsetProperty(), AVUI::BoxValue(value)); }

    static const AVUI::DependencyProperty& get_HasDropShadowProperty(); 
    bool get_HasDropShadow() { return UnboxValue< bool >(GetValue(get_HasDropShadowProperty())); }
    void set_HasDropShadow(const bool& value) { SetValue(get_HasDropShadowProperty(), AVUI::BoxValue(value)); }

    static const AVUI::DependencyProperty& get_StaysOpenProperty(); 
    bool get_StaysOpen() { return UnboxValue< bool >(GetValue(get_StaysOpenProperty())); }
    void set_StaysOpen(const bool& value) { SetValue(get_StaysOpenProperty(), AVUI::BoxValue(value)); }

    static const AVUI::DependencyProperty& get_PlacementTargetProperty();
    UIElement* get_PlacementTarget() { return (UIElement*)(Object*)GetValue(get_PlacementTargetProperty()); }
    void set_PlacementTarget(UIElement* value) { SetValue(get_PlacementTargetProperty(), value); }

    static const AVUI::DependencyProperty& get_PlacementRectangleProperty(); 
    Rect get_PlacementRectangle() { return UnboxValue< Rect >(GetValue(get_PlacementRectangleProperty())); }
    void set_PlacementRectangle(const Rect& value) { SetValue(get_PlacementRectangleProperty(), AVUI::BoxValue(value)); }

    static const AVUI::DependencyProperty& get_PlacementProperty();
    PlacementMode::Enum get_Placement() { return (PlacementMode::Enum) UnboxValue< int >(GetValue(get_PlacementProperty())); }
    void set_Placement(const PlacementMode::Enum& value) { SetValue(get_PlacementProperty(), AVUI::BoxValue((int)value)); }

    IMPLEMENT_ROUTED_EVENT(Opened,  RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Closed,  RoutedEventArgs, RoutingStrategy::Bubble);

    void ForceClose();

protected:

    virtual void OnOpened(RoutedEventArgs* pArgs);
    virtual void OnClosed(RoutedEventArgs* pArgs);

private:

    void OnPopupOpened(Object* pSource, EventArgs* e);
    void OnPopupCouldClose(Object* pSource, EventArgs* e);
    void OnPopupClosed(Object* pSource, EventArgs* e);

    static ObjectPtr CoerceHorizontalOffset(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoerceVerticalOffset(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoercePlacementTarget(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoercePlacementRectangle(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoercePlacement(DependencyObject* pDO, Object* pValue);



    void HookupParentPopup();
    static void OnIsOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    TRefCountedPtr<Popup> m_pParentPopup;

};


}; // namespace AVUI

