#pragma once

#include <AVUIButtonBase.h>

namespace AVUI {

class ControlTemplate;
class ToggleButton : public ButtonBase
{
public:
    ToggleButton() { };
    ~ToggleButton() { };

    DECLARE_ELEMENT(ToggleButton, ButtonBase);
    IMPLEMENT_ROUTED_EVENT(Checked, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Unchecked, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsChecked, bool, false, DependencyPropertyMetadata(PropertyFlag::None, OnIsCheckedChanged));

protected:
    virtual void OnClick();
    virtual void OnToggle();
    virtual void OnChecked(RoutedEventArgs* pArgs);
    virtual void OnUnchecked(RoutedEventArgs* pArgs);

private:
    static void OnIsCheckedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);


};

}; // namespace AVUI
