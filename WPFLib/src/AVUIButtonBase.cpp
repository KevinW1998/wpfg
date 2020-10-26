#include <AVUICommon.h>
#include <AVUIButtonBase.h>
#include <AVUIMouse.h>
#include <AVUIInputManager.h>

REGISTER_ELEMENT_BEGIN(AVUI::ButtonBase)

    REGISTER_PROPERTY(IsPressed)
    REGISTER_ROUTED_EVENT(Click)
    REGISTER_PROPERTY(ClickMode)
    REGISTER_PROPERTY(Command)
    REGISTER_PROPERTY(CommandParameter)
REGISTER_ELEMENT_END()

namespace AVUI {


void ButtonBase::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    if (get_ClickMode() != ClickMode::Hover)
    {
        pArgs->set_IsHandled(true);
        Focus();
        if (pArgs->get_ButtonState() == MouseButtonState::Pressed)
        {
            CaptureMouse();
            if (get_IsMouseCaptured())
            {
                if (pArgs->get_ButtonState() == MouseButtonState::Pressed)
                {
                    if (!get_IsPressed())
                    {
                        SetIsPressed(true);
                    }
                }
                else
                {
                    ReleaseMouseCapture();
                }
            }
        }
        if (get_ClickMode() == ClickMode::Press)
        {
            OnClick();
        }
    }
    ContentControl::OnMouseLeftButtonDown(pArgs);
}

void ButtonBase::OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs)
{
    if (get_ClickMode() != ClickMode::Hover)
    {
        pArgs->set_IsHandled(true);
        bool flag = (!get_IsSpaceKeyDown() && get_IsPressed()) && (get_ClickMode() == ClickMode::Release);
        if (get_IsMouseCaptured() && !get_IsSpaceKeyDown())
        {
            ReleaseMouseCapture();
        }
        if (flag)
        {
            OnClick();
        }
    }
    ContentControl::OnMouseLeftButtonUp(pArgs);
}

void ButtonBase::OnMouseEnter(MouseEventArgs* pArgs)
{
    ContentControl::OnMouseEnter(pArgs);
    if (HandleIsMouseOverChanged())
    {
        pArgs->set_IsHandled(true);
    }
}

void ButtonBase::OnMouseLeave(MouseEventArgs* pArgs)
{
    ContentControl::OnMouseLeave(pArgs);
    if (HandleIsMouseOverChanged())
    {
        pArgs->set_IsHandled(true);
    }
}

void ButtonBase::OnMouseMove(MouseEventArgs* pArgs)
{
    ContentControl::OnMouseMove(pArgs);
    if ((get_ClickMode() != ClickMode::Hover && get_IsMouseCaptured() && Mouse::get_Current()->get_LeftButton() == MouseButtonState::Pressed && !get_IsSpaceKeyDown()))
    {
        UpdateIsPressed();
        pArgs->set_IsHandled(true);
    }
}

void ButtonBase::UpdateIsPressed()
{
    Point point = get_InputManager()->get_MousePosition(this);
    Size renderSize = get_RenderSize();

    if(point.get_X() >= 0 && point.get_X() <= renderSize.get_Width() &&
       point.get_Y() >= 0 && point.get_Y() <= renderSize.get_Height())
    {
        SetIsPressed(true);
    }
    else
    {
        SetIsPressed(false);
    }
}

void ButtonBase::OnClick()
{
    RoutedEventArgs eventArgs(&(ButtonBase::get_ClickEvent()));

    UIElement::RaiseEvent(&eventArgs);

    RoutedCommand* pCommand = get_Command();
    if(pCommand != NULL)
    {
        pCommand->Execute(get_CommandParameter(), this);
    }
}

void ButtonBase::SetIsPressed(bool pressed)
{
    if (pressed)
    {
        SetValue(get_IsPressedProperty(), BoxValue(pressed));
    }
    else
    {
        ClearValue(get_IsPressedProperty());
    }
}

bool ButtonBase::HandleIsMouseOverChanged()
{
    if (get_ClickMode() != ClickMode::Hover)
    {
        return false;
    }
    if (get_IsMouseOver())
    {
        SetIsPressed(true);
        OnClick();
    }
    else
    {
        SetIsPressed(false);
    }
    return true;
}
 
void ButtonBase::OnLostMouseCapture(MouseEventArgs* pArgs)
{
    ContentControl::OnLostMouseCapture(pArgs);

    if (pArgs->get_OriginalSource() == this && get_ClickMode() != ClickMode::Hover && !get_IsSpaceKeyDown())
    {
        /*
        if (base.IsKeyboardFocused && !this.IsInMainFocusScope)
        {
            Keyboard.Focus(null);
        }
        */
        SetIsPressed(false);
    }
}

void ButtonBase::OnKeyUp(KeyEventArgs* pArgs)
{
    ContentControl::OnKeyUp(pArgs);
    if (get_ClickMode() != ClickMode::Hover && pArgs->get_Key() == Key::Space && get_IsSpaceKeyDown() && ((Keyboard::get_Current()->get_KeyboardModifiers() & (ModifierKeys::Control | ModifierKeys::Alt)) != ModifierKeys::Alt))
    {
        set_IsSpaceKeyDown(false);
        if (GetMouseLeftButtonReleased())
        {
            bool flag = get_IsPressed() && (get_ClickMode() == ClickMode::Release);
            if (get_IsMouseCaptured())
            {
                ReleaseMouseCapture();
            }
            if (flag)
            {
                OnClick();
            }
        }
        else if (get_IsMouseCaptured())
        {
            UpdateIsPressed();
        }
        pArgs->set_IsHandled(true);
    }
}

void ButtonBase::OnKeyDown(KeyEventArgs* pArgs)
{
    ContentControl::OnKeyDown(pArgs);
    if (get_ClickMode() != ClickMode::Hover)
    {
        if (pArgs->get_Key() == Key::Space)
        {
            if (((Keyboard::get_Current()->get_KeyboardModifiers() & (ModifierKeys::Control | ModifierKeys::Alt)) != ModifierKeys::Alt) && !get_IsMouseCaptured() && pArgs->get_OriginalSource() == this)
            {
                set_IsSpaceKeyDown(true);
                SetIsPressed(true);
                CaptureMouse();
                if (get_ClickMode() == ClickMode::Press)
                {
                    OnClick();
                }
                pArgs->set_IsHandled(true);
            }
        }
        else if (pArgs->get_Key() == Key::Return && UnboxValue<bool>(GetValue(KeyboardNavigation::get_AcceptsReturnProperty())))
        {
            if (pArgs->get_OriginalSource() == this)
            {
                set_IsSpaceKeyDown(false);
                SetIsPressed(false);
                if (get_IsMouseCaptured())
                {
                    ReleaseMouseCapture();
                }
                OnClick();
                pArgs->set_IsHandled(true);
            }
        }
        else if (get_IsSpaceKeyDown())
        {
            SetIsPressed(false);
            set_IsSpaceKeyDown(false);
            if (get_IsMouseCaptured())
            {
                ReleaseMouseCapture();
            }
        }
    }
}
 
bool ButtonBase::GetMouseLeftButtonReleased()
{
    return (Mouse::get_Current()->get_LeftButton() == MouseButtonState::Released);
}

 

 



}; // namespace AVUI
