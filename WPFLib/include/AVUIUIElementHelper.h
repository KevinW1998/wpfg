#pragma once

namespace AVUI {
             
class UIElementHelper
{
public:
    static UIElement* GetContainingUIElement2D(DependencyObject* pReference);
    static DependencyObject* GetUIParent(DependencyObject* pChild, bool continuePastVisualTree = false);
    static bool IsHitTestVisible(DependencyObject* pDO);
    static bool IsUIElementOrUIElement3D(DependencyObject* pDO);
    static bool IsVisible(DependencyObject* pDO);
    static DependencyObject* PredictFocus(DependencyObject* pDO, FocusNavigationDirection::Enum direction);
};


}; // namespace AVUI


