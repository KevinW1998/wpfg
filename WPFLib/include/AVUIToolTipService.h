#pragma once

#include <AVUIEventableObject.h>
#include <AVUIDependencyObject.h>

namespace AVUI {


class ToolTipEventArgs : public RoutedEventArgs
{
public:
    DECLARE_ELEMENT(ToolTipEventArgs, RoutedEventArgs);

    ToolTipEventArgs(bool opening);
};

class FindToolTipEventArgs : public RoutedEventArgs
{
public:

    FindToolTipEventArgs();

    DECLARE_ELEMENT(FindToolTipEventArgs, RoutedEventArgs);

    bool get_KeepCurrentActive() { return m_keepCurrentActive; }
    void set_KeepCurrentActive(bool keepCurrentActive) { m_keepCurrentActive = keepCurrentActive; }

    DependencyObject* get_TargetElement() { return m_pTargetElement; }
    void set_TargetElement(DependencyObject* pTargetElement) { m_pTargetElement = pTargetElement; }

private:

    bool m_keepCurrentActive;
    TRefCountedPtr<DependencyObject> m_pTargetElement;
};

DEFINE_EVENTHANDLER(ToolTip);
DEFINE_EVENTHANDLER(FindToolTip);


class ToolTipService : public EventableObject
{
    friend class InputManager;
public:
    ToolTipService() { }

    DECLARE_ELEMENT(ToolTipService, Object);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(ToolTip, Object, NULL, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(HorizontalOffset, float, 0.0f, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(VerticalOffset, float, 0.0f, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(HasDropShadow, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(PlacementTarget, Object, NULL, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(PlacementRectangle, Rect, Rect::get_Empty(), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(Placement, PlacementMode, PlacementMode::Mouse, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(ShowOnDisabled, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(IsOpen, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(IsEnabled, bool, true, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(ShowDuration, int, 5000, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(InitialShowDelay, int, 1000, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(BetweenShowDelay, int, 100, PropertyFlag::IsAttachedProperty);

    IMPLEMENT_ROUTED_EVENT(ToolTipOpening,  ToolTipEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(ToolTipClosing,  ToolTipEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(FindToolTip,  FindToolTipEventArgs, RoutingStrategy::Bubble);


    static int GetInitialShowDelay(DependencyObject* pElement)
    {
        return UnboxValue<int>(pElement->GetValue(get_InitialShowDelayProperty()));
    }

    static int GetBetweenShowDelay(DependencyObject* pElement)
    {
        return UnboxValue<int>(pElement->GetValue(get_BetweenShowDelayProperty()));
    }

    static bool GetIsEnabled(DependencyObject* pElement)
    {
        return UnboxValue<bool>(pElement->GetValue(get_IsEnabledProperty()));
    }

    static bool GetShowOnDisabled(DependencyObject* pElement)
    {
        return UnboxValue<bool>(pElement->GetValue(get_ShowOnDisabledProperty()));
    }

    static int GetShowDuration(DependencyObject* pElement)
    {
        return UnboxValue<int>(pElement->GetValue(get_ShowDurationProperty()));
    }

    static Object* GetToolTip(DependencyObject* pElement)
    {
        return pElement->GetValue(get_ToolTipProperty());
    }


private:

    static void StaticConstructor();
    static void OnFindToolTip(Object* pSender, FindToolTipEventArgs* pArgs);
    static bool ToolTipIsEnabled(DependencyObject* pDO);

};

}; // namespace AVUI


