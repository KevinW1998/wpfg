#pragma once

#include <AVUIDependencyProperty.h>
#include <AVUIThickness.h>

namespace AVUI {

class DependencyObject;
class Visual;
class UIElement;
class Point;

class KeyboardNavigation : public Object
{
public:

    KeyboardNavigation() { }
    DECLARE_ELEMENT(KeyboardNavigation, Object);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(TabIndex, int, 0x7fffffff, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(IsTabStop, bool, true, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(AcceptsReturn, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(DirectionalNavigationMargin, Thickness, Thickness(0), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(TabNavigation, KeyboardNavigationMode, KeyboardNavigationMode::Continue, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(DirectionalNavigation, KeyboardNavigationMode, KeyboardNavigationMode::Continue, PropertyFlag::IsAttachedProperty);


    UIElement* PredictFocusedElement(UIElement* pSource, FocusNavigationDirection::Enum direction);

    static KeyboardNavigation* get_Current();

    static FocusNavigationDirection::Enum KeyToTraversalDirection(Key::Enum key);

private:

    bool Navigate(UIElement* pCurrentElement, FocusNavigationDirection::Enum request, ModifierKeys::Enum modifierKeys);
    bool Navigate(UIElement* pCurrentElement, FocusNavigationDirection::Enum request, ModifierKeys::Enum modifierKeys, UIElement* pFirstElement);

    Visual* GetVisualRoot(Visual* pVisual);

    UIElement* GetNextTab(UIElement* pDO, UIElement* pContainer, bool goDownOnly);
    UIElement* GetPrevTab(UIElement* pDO, UIElement* pContainer, bool goDownOnly);
    UIElement* GetGroupParent(UIElement* pDO, bool includeCurrent = false);
    UIElement* GetNextInDirection(UIElement* pDO, FocusNavigationDirection::Enum direction);

    UIElement* MoveNext(UIElement* pDO, UIElement* pContainer, FocusNavigationDirection::Enum direction, float startRange, float endRange);

    bool IsEndlessLoop(UIElement* pSource, UIElement* pContainer);
    bool IsTabStop(UIElement* pDO);
    bool IsGroup(UIElement* pDO);
    void ResetBaseLines(float value, bool horizontalDirection);
    float GetDistance(const Point& ptFrom, const Point& ptTo);
    float GetDistance(const Rect& rectFrom, const Rect& rectTo, FocusNavigationDirection::Enum direction);
    bool IsInDirection(const Rect& rectFrom, const Rect& rectTo, FocusNavigationDirection::Enum direction);
    bool IsInRange(UIElement* pSource, UIElement* pTarget, const Rect& sourceRect, const Rect& targetRect, FocusNavigationDirection::Enum direction, float startRange, float endRange);
    float GetPerpDistance(const Rect& sourceRect, const Rect& targetRect, FocusNavigationDirection::Enum direction);
    UIElement* GetFirstChild(UIElement* pParent);
    UIElement* GetNextSibling(UIElement* pParent);
    UIElement* GetParent(UIElement* pUIE);

    KeyboardNavigationMode::Enum GetKeyNavigationMode(UIElement* pDO);
    Rect GetRectangle(UIElement* pDO);
    UIElement* FindNextInDirection(UIElement* pSource, const Rect& sourceRect, UIElement* pContainer, FocusNavigationDirection::Enum direction, float startRange, float endRange);
    UIElement* GetNextInTree(UIElement* pSource, UIElement* pContainer);
    

    const DependencyProperty* m_pNavigationProperty;
    float m_verticalBaseline;
    float m_horizontalBaseline;

};

}; // namespace AVUI




