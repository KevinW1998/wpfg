#include <AVUICommon.h>
#include <AVUIToggleButton.h>

REGISTER_ELEMENT_BEGIN(AVUI::ToggleButton)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_ROUTED_EVENT(Checked)
    REGISTER_ROUTED_EVENT(Unchecked)
    REGISTER_PROPERTY(IsChecked)
REGISTER_ELEMENT_END()

namespace AVUI
{

void ToggleButton::OnClick()
{
    OnToggle();
    ButtonBase::OnClick();
}

void ToggleButton::OnToggle()
{
    set_IsChecked(!get_IsChecked());
}

void ToggleButton::OnIsCheckedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ToggleButton* pButton = (ToggleButton*) pDO;

    if(pButton->get_IsChecked())
    {
        RoutedEventArgs eventArgs(&(ToggleButton::get_CheckedEvent()));

        pButton->OnChecked(&eventArgs);
    }
    else
    {
        RoutedEventArgs eventArgs(&(ToggleButton::get_UncheckedEvent()));

        pButton->OnUnchecked(&eventArgs);
    }
}

void ToggleButton::OnChecked(RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void ToggleButton::OnUnchecked(RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

};


