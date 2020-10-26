#pragma once

#include <AVUIMenuBase.h>
#include <AVUIMenuItem.h>
#include <AVUIContextMenuService.h>
#include <AVUIPopupControlService.h>
#include <AVUIPopup.h>

namespace AVUI {

class ContextMenu : public MenuBase
{
public:

    DECLARE_ELEMENT(ContextMenu, MenuBase);
    ContextMenu() { Initialize(); }

    static const AVUI::DependencyProperty& get_HorizontalOffsetProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoerceHorizontalOffset));
        const static AVUI::DependencyProperty& property = ContextMenuService::get_HorizontalOffsetProperty().AddOwner(GET_TYPE(_thisType), BoxValue(0.0f), &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_VerticalOffsetProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoerceVerticalOffset));
        const static AVUI::DependencyProperty& property = ContextMenuService::get_VerticalOffsetProperty().AddOwner(GET_TYPE(_thisType), BoxValue(0.0f), &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_IsOpenProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, OnIsOpenChanged));
        const static AVUI::DependencyProperty& property = Popup::get_IsOpenProperty().AddOwner(GET_TYPE(_thisType), BoxValue(false), &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_PlacementTargetProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoercePlacementTarget));
        const static AVUI::DependencyProperty& property = ContextMenuService::get_PlacementTargetProperty().AddOwner(GET_TYPE(_thisType), NULL, &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_PlacementRectangleProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoercePlacementRectangle));
        const static AVUI::DependencyProperty& property = ContextMenuService::get_PlacementRectangleProperty().AddOwner(GET_TYPE(_thisType), BoxValue(Rect::get_Empty()), &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_PlacementProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoercePlacement));
        const static AVUI::DependencyProperty& property = ContextMenuService::get_PlacementProperty().AddOwner(GET_TYPE(_thisType), BoxValue<int>(PlacementMode::MousePoint), &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_HasDropShadowProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoerceHasDropShadow));
        const static AVUI::DependencyProperty& property = ContextMenuService::get_HasDropShadowProperty().AddOwner(GET_TYPE(_thisType), BoxValue(false), &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_StaysOpenProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(PropertyFlag::None);
        const static AVUI::DependencyProperty& property = Popup::get_StaysOpenProperty().AddOwner(GET_TYPE(_thisType), BoxValue(false), &metadata);

        return property;
    }

    ALIAS_ROUTED_EVENT(PopupControlService, ContextMenuOpened, RoutedEventArgs);
    ALIAS_ROUTED_EVENT(PopupControlService, ContextMenuClosed, RoutedEventArgs);

    /*
    CustomPopupPlacementCallbackProperty = Popup.CustomPopupPlacementCallbackProperty.AddOwner(typeof(ContextMenu));
    */

    static const AVUI::DependencyProperty& get_InsideContextMenuProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(PropertyFlag::Inherits);
        const static AVUI::DependencyProperty& property = MenuItem::get_InsideContextMenuProperty().AddOwner(GET_TYPE(_thisType), BoxValue(true), &metadata);

        return property;
    }

    bool get_HasDropShadow() { return UnboxValue<bool>(GetValue(get_HasDropShadowProperty())); }
    void set_HasDropShadow(bool hasDropShadow) { SetValue(get_HasDropShadowProperty(), BoxValue(hasDropShadow)); }

    float get_HorizontalOffset() { return UnboxValue<float>(GetValue(get_HorizontalOffsetProperty())); }
    void set_HorizontalOffset(float horizontalOffset) { SetValue(get_HorizontalOffsetProperty(), BoxValue(horizontalOffset)); }

    bool get_IsOpen() { return UnboxValue<bool>(GetValue(get_IsOpenProperty())); }
    void set_IsOpen(bool isOpen) { SetValue(get_IsOpenProperty(), BoxValue(isOpen)); }

    PlacementMode::Enum get_Placement() { return (PlacementMode::Enum) UnboxValue<int>(GetValue(get_PlacementProperty())); }
    void set_Placement(PlacementMode::Enum placement) { SetValue(get_PlacementProperty(), BoxValue<int>(placement)); }

    Rect get_PlacementRectangle() { return UnboxValue<Rect>(GetValue(get_PlacementRectangleProperty())); }
    void set_PlacementRectangle(const Rect& placementRectangle) { SetValue(get_PlacementRectangleProperty(), BoxValue(placementRectangle)); }

    UIElement* get_PlacementTarget() { return (UIElement*)(Object*)GetValue(get_PlacementTargetProperty()); }
    void set_PlacementTarget(UIElement* pPlacementTarget) { SetValue(get_PlacementTargetProperty(), (Object*)pPlacementTarget); }

    bool get_StaysOpen() { return UnboxValue<bool>(GetValue(get_StaysOpenProperty())); }
    void set_StaysOpen(bool staysOpen) { SetValue(get_StaysOpenProperty(), BoxValue(staysOpen)); }

    float get_VerticalOffset() { return UnboxValue<float>(GetValue(get_VerticalOffsetProperty())); }
    void set_VerticalOffset(float verticalOffset) { SetValue(get_VerticalOffsetProperty(), BoxValue(verticalOffset)); }

protected:

    virtual bool get_HandlesScrolling() { return true; } 
    virtual void OnClosed(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnOpened(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }

private:

    void Initialize();

    static void StaticConstructor();
    static ObjectPtr CoerceHorizontalOffset(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoerceVerticalOffset(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoercePlacementTarget(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoercePlacementRectangle(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoercePlacement(DependencyObject* pDO, Object* pValue);
    static ObjectPtr CoerceHasDropShadow(DependencyObject* pDO, Object* pValue);
    static void OnIsOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    void OnIsMenuModeChanged(Object* pSender, EventArgs* pArgs);
    void HookupParentPopup();
    void OnPopupClosed(Object* pSender, EventArgs* pArgs);
    void OnPopupOpened(Object* pSource, EventArgs* pArgs);
    void OnPopupUnloaded(Object* pSender, RoutedEventArgs* pArgs);
    void OnPopupCouldClose(Object* pSender, EventArgs* pArgs);
    void ClosingMenu();

    void ClosingMenuAnonymousDelegate(Object* pSender, Object* pArg);
    void PopupUnloadedAnonymousDelegate(Object* pSender, Object* pArg);

    TRefCountedPtr<Popup> m_pParentPopup;
    TWeakPtr<Control> m_pWeakRefToPreviousFocus;
};


}; // namespace AVUI

