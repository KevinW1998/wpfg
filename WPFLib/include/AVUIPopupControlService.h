#pragma once

#include <AVUIEventableObject.h>

namespace AVUI {

class ToolTip;
class DispatcherTimer;
class DependencyObject;
class UIElement;
class MouseEventArgs;
class MouseButtonEventArgs;

class PopupControlService : public EventableObject
{
    friend class InputManager;
    friend class ToolTipService;
    friend class ToolTip;
    friend class MenuBase;
    friend class ContextMenuService;
    friend class ContextMenu;
public:
    PopupControlService() : m_quickShow(false), m_ownToolTip(false) { }


    static PopupControlService* get_Current();
    ToolTip* get_CurrentToolTip() { return m_pCurrentToolTip; }


    DECLARE_ELEMENT(PopupControlService, Object);

    IMPLEMENT_ROUTED_EVENT(ContextMenuOpened, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(ContextMenuClosed, RoutedEventArgs, RoutingStrategy::Bubble);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(Owner, DependencyObject, NULL, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(ServiceOwned, bool, false, PropertyFlag::IsAttachedProperty);


private:

    static TRefCountedPtr<Object> CoerceProperty(DependencyObject* pDO, Object* pValue, const DependencyProperty* pDP);
    static bool IsPresentationSourceNull(DependencyObject* pUIE);
    static bool IsElementEnabled(DependencyObject* pDO);
    void PostProcessInput(MouseEventArgs* pArgs);
    void ProcessMouseUp(MouseButtonEventArgs* pButtonArgs);
    void RaiseToolTipClosingEvent(bool reset);
    void OnMouseMove(UIElement* pDirectlyOver);
    void RaiseToolTipOpeningEvent();
    bool RaiseContextMenuOpeningEvent(UIElement* pSource, float x, float y, bool userInitiated);

    void ResetToolTipTimer();

    void OnForceClose(Object* pSender, EventArgs* e);
    void OnBetweenShowDelay(Object* pSender, EventArgs* e);
    void OnRaiseToolTipOpeningEvent(Object* pSender, EventArgs* e);
    void OnToolTipClosed(Object* pSender, RoutedEventArgs* e);
    void OnRaiseToolTipClosingEvent(Object* pSender, EventArgs* e);
    bool LocateNearestToolTip(DependencyObject** ppDO);
    bool StopLookingForToolTip(DependencyObject* pDO);
    bool WithinCurrentToolTip(DependencyObject* pDO);
    void OnContextMenuClosed(Object* pSource, RoutedEventArgs* pArgs);

    static UIElement* GetTarget(DependencyObject* pDO);
 

    DependencyObject* get_LastChecked() { return m_pLastChecked; }
    void set_LastChecked(DependencyObject* pDO) { m_pLastChecked = pDO; }

    UIElement* get_LastMouseDirectlyOver() { return m_pLastMouseDirectlyOver; }
    void set_LastMouseDirectlyOver(UIElement* pUIE) { m_pLastMouseDirectlyOver = pUIE; }

    DependencyObject* get_LastMouseOverWithToolTip() { return m_pLastMouseOverWithToolTip; }
    void set_LastMouseOverWithToolTip(DependencyObject* pDO) { m_pLastMouseOverWithToolTip = pDO; }

    DispatcherTimer* get_ToolTipTimer() { return m_pToolTipTimer; }
    void set_ToolTipTimer(DispatcherTimer* pTimer) { ResetToolTipTimer(); m_pToolTipTimer = pTimer; }

    void InspectElementForToolTip(DependencyObject* pDO);
    static DependencyObject* FindParent(DependencyObject* pDO);


    TRefCountedPtr<ToolTip> m_pCurrentToolTip;
    TRefCountedPtr<DispatcherTimer> m_pForceCloseTimer;
    TWeakPtr<DependencyObject> m_pLastChecked;
    TWeakPtr<UIElement> m_pLastMouseDirectlyOver;
    TWeakPtr<DependencyObject> m_pLastMouseOverWithToolTip;
    TRefCountedPtr<DispatcherTimer> m_pToolTipTimer;
    bool m_quickShow;
    bool m_ownToolTip;
};


}; // namespace AVUI

