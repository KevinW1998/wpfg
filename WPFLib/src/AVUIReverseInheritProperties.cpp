#include <AVUICommon.h>
#include <AVUIReverseInheritProperties.h>
#include <AVUIMouse.h>
#include <AVUIUIElement.h>

namespace AVUI {

FocusWithinProperty::FocusWithinProperty() : ReverseInheritProperty(&UIElement::get_IsKeyboardFocusWithinProperty(), 
                                                                    UIElement::CoreFlags::IsKeyboardFocusWithinCache,
                                                                    UIElement::CoreFlags::IsKeyboardFocusWithinChanged)
{

}

void FocusWithinProperty::FireNotifications(UIElement* pUIE, bool oldValue)
{
    if (pUIE != NULL)
    {
        DependencyPropertyChangedEventArgs args(UIElement::get_IsKeyboardFocusWithinProperty(), BoxValue(oldValue), BoxValue(!oldValue));

        pUIE->RaiseIsKeyboardFocusWithinChanged(&args);
    }    
}

MouseCaptureWithinProperty::MouseCaptureWithinProperty() : ReverseInheritProperty(&UIElement::get_IsMouseCaptureWithinProperty(), 
                                                                    UIElement::CoreFlags::IsMouseCaptureWithinCache,
                                                                    UIElement::CoreFlags::IsMouseCaptureWithinChanged)
{

}

void MouseCaptureWithinProperty::FireNotifications(UIElement* pUIE, bool oldValue)
{
    if (pUIE != NULL)
    {
        DependencyPropertyChangedEventArgs args(UIElement::get_IsMouseCaptureWithinProperty(), BoxValue(oldValue), BoxValue(!oldValue));

        pUIE->RaiseIsMouseCaptureWithinChanged(&args);
    }
}


MouseOverProperty::MouseOverProperty() : ReverseInheritProperty(&UIElement::get_IsMouseOverProperty(), 
                                                                    UIElement::CoreFlags::IsMouseOverCache,
                                                                    UIElement::CoreFlags::IsMouseOverChanged)
{

}

void MouseOverProperty::FireNotifications(UIElement* pUIE, bool oldValue)
{
    bool flag = false;
    if (pUIE != NULL)
    {
        flag = (!oldValue && pUIE->get_IsMouseOver()) || (oldValue && !pUIE->get_IsMouseOver());
    }

    if (flag)
    {
        MouseEventArgs e(oldValue ? &Mouse::get_MouseLeaveEvent() : &Mouse::get_MouseEnterEvent());

        pUIE->RaiseEvent(&e);
    }
}

}; // namespace AVUI

