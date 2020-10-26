#include <AVUICommon.h>
#include <AVUIControl.h>
#include <AVUIControlTemplate.h>
#include <AVUIStyleHelper.h>
#include <AVUIInputManager.h>
#include <AVUIDispatcher.h>

REGISTER_ELEMENT_BEGIN(AVUI::Control)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Background);
    REGISTER_PROPERTY(BorderBrush);
    REGISTER_PROPERTY(BorderThickness);
    REGISTER_PROPERTY(Template);
    REGISTER_PROPERTY(Foreground);
    REGISTER_PROPERTY(FontFamily);
    REGISTER_PROPERTY(FontSize);
    REGISTER_PROPERTY(FontStyle);
    REGISTER_PROPERTY(FontWeight);
    REGISTER_PROPERTY(Padding);
    REGISTER_PROPERTY(HorizontalContentAlignment);
    REGISTER_PROPERTY(VerticalContentAlignment);

    REGISTER_ROUTED_EVENT(PreviewMouseDoubleClick);
    REGISTER_ROUTED_EVENT(MouseDoubleClick);

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor);
REGISTER_ELEMENT_END()


namespace AVUI {


void Control::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(Control), UIElement::get_PreviewMouseLeftButtonDownEvent(), MouseButtonEventHandler::Create(HandleDoubleClick), true);    
    EventManager::RegisterClassHandler(GET_TYPE(Control), UIElement::get_MouseLeftButtonDownEvent(), MouseButtonEventHandler::Create(HandleDoubleClick), true);    
    EventManager::RegisterClassHandler(GET_TYPE(Control), UIElement::get_PreviewMouseRightButtonDownEvent(), MouseButtonEventHandler::Create(HandleDoubleClick), true);    
    EventManager::RegisterClassHandler(GET_TYPE(Control), UIElement::get_MouseRightButtonDownEvent(), MouseButtonEventHandler::Create(HandleDoubleClick), true);    

    static DependencyPropertyMetadata controlFocusableMetadata(PropertyFlag::None);
    UIElement::get_FocusableProperty().OverrideMetadata(GET_TYPE(Control), BoxValue(true), &controlFocusableMetadata);

}


Size Control::MeasureOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Measure(constraintSize);
            return pVisualChild->get_DesiredSize();
        }
    }
    return Size(0, 0);
}

Size Control::ArrangeOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Arrange(Rect(constraintSize));
        }
    }
    return constraintSize;
}

void Control::OnPropertyChanged(const DependencyPropertyChangedEventArgs& e)
{
    if(e.DependencyPropertyChanged != UIElement::get_StyleProperty())
    {
        ControlTemplate* pControlTemplate = get_Template();

        if(pControlTemplate != NULL && get_HasTemplateGeneratedSubTree())
        {
            StyleHelper::OnTriggerSourcePropertyInvalidated(this, pControlTemplate, e.DependencyPropertyChanged, e, pControlTemplate->TriggerSourceRecordFromChildIndex, pControlTemplate->PropertyTriggersWithActions, 0);
        }
    }

    UIElement::OnPropertyChanged(e);
}

void Control::CaptureMouse()
{
    get_InputManager()->CaptureMouse(this);

    OnIsMouseCapturedChanged();
}

bool Control::get_IsMouseCaptured()
{
    return get_InputManager()->get_MouseCapture() == this;
}

void Control::ReleaseMouseCapture()
{
    get_InputManager()->ReleaseMouseCapture(this);

    OnIsMouseCapturedChanged();
}

InputManager* Control::get_InputManager()
{
    return Dispatcher::get_CurrentDispatcher()->get_InputManager();
}

bool Control::Focus()
{
    if(get_Focusable())
    {
        get_InputManager()->set_KeyboardFocus(this);
        return true;
    }

    return false;
}

Object* Control::FindTemplateChild(const WCHAR* wzChildName)
{
    ControlTemplate* pTemplate = get_Template();
    if(pTemplate != NULL)
    {
        return pTemplate->FindName(String(wzChildName),  this);
    }

    return NULL;
}

void Control::HandleDoubleClick(Object* pSender, MouseButtonEventArgs* pArgs)
{
    if (pArgs->get_ClickCount() == 2)
    {
        Control* pControl = (Control*) pSender;
        MouseButtonEventArgs dblArgs(NULL, pArgs->get_ChangedButton(), true);

        if (pArgs->get_RoutedEvent() == &UIElement::get_PreviewMouseLeftButtonDownEvent() || pArgs->get_RoutedEvent() == &UIElement::get_PreviewMouseRightButtonDownEvent())
        {
            dblArgs.set_RoutedEvent(&Control::get_PreviewMouseDoubleClickEvent());
            dblArgs.set_Source(pArgs->get_OriginalSource());
            dblArgs.OverrideSource(pArgs->get_Source());
            pControl->OnPreviewMouseDoubleClick(&dblArgs);
        }
        else
        {
            dblArgs.set_RoutedEvent(&Control::get_MouseDoubleClickEvent());
            dblArgs.set_Source(pArgs->get_OriginalSource());
            dblArgs.OverrideSource(pArgs->get_Source());
            pControl->OnMouseDoubleClick(&dblArgs);
        }
        if (dblArgs.get_IsHandled())
        {
            pArgs->set_IsHandled(true);
        }
    }
}

void Control::OnPreviewMouseDoubleClick(MouseButtonEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void Control::OnMouseDoubleClick(MouseButtonEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}


}; // namespace AVUI
