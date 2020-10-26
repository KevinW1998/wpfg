#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class ContextMenuEventArgs;
class ContextMenu;

class ContextMenuService : public EventableObject
{
    friend class PopupControlService;
public:
    DECLARE_ELEMENT(ContextMenuService, Object);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(ContextMenu, ContextMenu, NULL, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(PlacementTarget, UIElement, NULL, PropertyFlag::IsAttachedProperty);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(HorizontalOffset, float, 0.0f, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(VerticalOffset, float, 0.0f, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(HasDropShadow, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(PlacementRectangle, Rect, Rect::get_Empty(), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(Placement, PlacementMode, PlacementMode::MousePoint, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(ShowOnDisabled, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(IsEnabled, bool, true, PropertyFlag::IsAttachedProperty);


    IMPLEMENT_ROUTED_EVENT(ContextMenuOpening, ContextMenuEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(ContextMenuClosing, ContextMenuEventArgs, RoutingStrategy::Bubble);

    static TRefCountedPtr<ContextMenu> GetContextMenu(DependencyObject* pElement) { return (ContextMenu*)(Object*)pElement->GetValue(get_ContextMenuProperty()); }
    static void SetContextMenu(DependencyObject* pElement, ContextMenu* pValue) { return pElement->SetValue(get_ContextMenuProperty(), (Object*)pValue); }

    static TRefCountedPtr<UIElement> GetPlacementTarget(DependencyObject* pElement) { return (UIElement*)(Object*)pElement->GetValue(get_PlacementTargetProperty()); }
    static void SetPlacementTarget(DependencyObject* pElement, UIElement* pValue) { return pElement->SetValue(get_PlacementTargetProperty(), (Object*)pValue); }

    static float GetHorizontalOffset(DependencyObject* pElement) { return UnboxValue<float>(pElement->GetValue(get_HorizontalOffsetProperty())); }
    static void SetHorizontalOffset(DependencyObject* pElement, float value) { return pElement->SetValue(get_HorizontalOffsetProperty(), BoxValue(value)); }

    static float GetVerticalOffset(DependencyObject* pElement) { return UnboxValue<float>(pElement->GetValue(get_VerticalOffsetProperty())); }
    static void SetVerticalOffset(DependencyObject* pElement, float value) { return pElement->SetValue(get_VerticalOffsetProperty(), BoxValue(value)); }

    static bool GetHasDropShadow(DependencyObject* pElement) { return UnboxValue<bool>(pElement->GetValue(get_HasDropShadowProperty())); }
    static void SetHasDropShadow(DependencyObject* pElement, bool value) { return pElement->SetValue(get_HasDropShadowProperty(), BoxValue(value)); }

    static Rect GetPlacementRectangle(DependencyObject* pElement) { return UnboxValue<Rect>(pElement->GetValue(get_PlacementRectangleProperty())); }
    static void SetPlacementRectangle(DependencyObject* pElement, const Rect& value) { return pElement->SetValue(get_PlacementRectangleProperty(), BoxValue(value)); }

    static PlacementMode::Enum GetPlacement(DependencyObject* pElement) { return (PlacementMode::Enum) UnboxValue<int>(pElement->GetValue(get_PlacementProperty())); }
    static void SetPlacement(DependencyObject* pElement, PlacementMode::Enum value) { return pElement->SetValue(get_PlacementProperty(), BoxValue<int>(value)); }

    static bool GetShowOnDisabled(DependencyObject* pElement) { return UnboxValue<bool>(pElement->GetValue(get_ShowOnDisabledProperty())); }
    static void SetShowOnDisabled(DependencyObject* pElement, bool value) { return pElement->SetValue(get_ShowOnDisabledProperty(), BoxValue(value)); }

    static bool GetIsEnabled(DependencyObject* pElement) { return UnboxValue<bool>(pElement->GetValue(get_IsEnabledProperty())); }
    static void SetIsEnabled(DependencyObject* pElement, bool value) { return pElement->SetValue(get_IsEnabledProperty(), BoxValue(value)); }

private:

    static void StaticConstructor();
    static bool ContextMenuIsEnabled(DependencyObject* pDO);
    static void OnContextMenuOpening(Object* pSender, ContextMenuEventArgs* pArgs);
};

class ContextMenuEventArgs : public RoutedEventArgs
{
    friend class ContextMenuService;
    friend class PopupControlService;
public:

    DECLARE_ELEMENT(ContextMenuEventArgs, RoutedEventArgs);

    ContextMenuEventArgs(Object* pSource, bool opening, float left = -1.0f, float top = -1.0f) 
    { 
        m_left = left;
        m_top = top;
        set_RoutedEvent(opening ? &ContextMenuService::get_ContextMenuOpeningEvent() : &ContextMenuService::get_ContextMenuClosingEvent());
        set_Source(pSource);
    }

    float get_CursorLeft() { return m_left; }
    float get_CursorTop() { return m_top; }

private:
    DependencyObject* get_TargetElement() { return m_pTargetElement; }
    void set_TargetElement(DependencyObject* pTarget) { m_pTargetElement = pTarget; }

    float m_left;
    float m_top;
    TRefCountedPtr<DependencyObject> m_pTargetElement;
};

DEFINE_EVENTHANDLER(ContextMenu);



}; // namespace AVUI

