#include <AVUICommon.h>
#include <AVUIRepeatButton.h>
#include <AVUIDispatcherTimer.h>

REGISTER_ELEMENT_BEGIN(AVUI::RepeatButton)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
    REGISTER_PROPERTY(Delay)
    REGISTER_PROPERTY(Interval)
REGISTER_ELEMENT_END()


namespace AVUI {

void RepeatButton::StaticConstructor()
{
    static DependencyPropertyMetadata repeatButtonClickModeMetadata(PropertyFlag::None);
    ButtonBase::get_ClickModeProperty().OverrideMetadata(GET_TYPE(RepeatButton), BoxValue<int>(ClickMode::Press), &repeatButtonClickModeMetadata);    
}

int RepeatButton::GetKeyboardDelay()
{
    int keyboardDelay = 2; //SystemParameters.KeyboardDelay;
    if ((keyboardDelay < 0) || (keyboardDelay > 3))
    {
        keyboardDelay = 0;
    }
    return ((keyboardDelay + 1) * 250);
}

int RepeatButton::GetKeyboardSpeed()
{
    int keyboardSpeed = 0x19; //SystemParameters.KeyboardSpeed;
    if ((keyboardSpeed < 0) || (keyboardSpeed > 0x1f))
    {
        keyboardSpeed = 0x1f;
    }
    return ((((0x1f - keyboardSpeed) * 0x16f) / 0x1f) + 0x21);
}



bool RepeatButton::HandleIsMouseOverChanged()
{
    if (get_ClickMode() != ClickMode::Hover)
    {
        return false;
    }
    if (get_IsMouseOver())
    {
        StartTimer();
    }
    else
    {
        StopTimer();
    }
    return true;
}

void RepeatButton::StartTimer()
{
    if (m_pTimer == NULL)
    {
        m_pTimer = object_allocate<DispatcherTimer>();
        m_pTimer->add_Tick(EventHandler::Create(this, &RepeatButton::OnTimeout));
    }
    else if (m_pTimer->get_IsEnabled())
    {
        return;
    }
    m_pTimer->set_Interval(TimeSpan::FromMilliseconds(get_Delay()));
    m_pTimer->Start();
}

void RepeatButton::StopTimer()
{
    if (m_pTimer != NULL)
    {
        m_pTimer->Stop();
    }
}

void RepeatButton::OnTimeout(Object* pSender, EventArgs* pArgs)
{
    TimeSpan span = TimeSpan::FromMilliseconds(get_Interval());
    if (m_pTimer->get_Interval() != span)
    {
        m_pTimer->set_Interval(span);
    }
    if (get_IsPressed())
    {
        OnClick();
    }
}

 
void RepeatButton::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    ButtonBase::OnMouseLeftButtonDown(pArgs);
    if (get_IsPressed() && (get_ClickMode() != ClickMode::Hover))
    {
        StartTimer();
    }
}

void RepeatButton::OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs)
{
    ButtonBase::OnMouseLeftButtonUp(pArgs);
    if (get_ClickMode() != ClickMode::Hover)
    {
        StopTimer();
    }

}

void RepeatButton::OnMouseEnter(MouseEventArgs* pArgs)
{
    ButtonBase::OnMouseEnter(pArgs);
    if (HandleIsMouseOverChanged())
    {
        pArgs->set_IsHandled(true);
    }
}

void RepeatButton::OnMouseLeave(MouseEventArgs* pArgs)
{
    ButtonBase::OnMouseLeave(pArgs);
    if (HandleIsMouseOverChanged())
    {
        pArgs->set_IsHandled(true);
    }
}

void RepeatButton::OnKeyDown(KeyEventArgs* pArgs)
{
    ButtonBase::OnKeyDown(pArgs);
    if (pArgs->get_Key() == Key::Space && get_ClickMode() != ClickMode::Hover)
    {
        StartTimer();
    }
}


}; // namespace AVUI
