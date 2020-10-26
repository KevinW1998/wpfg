#include <AVUICommon.h>
#include <AVUIPopupControlService.h>
#include <AVUIInputManager.h>
#include <AVUIDispatcherTimer.h>
#include <AVUIToolTip.h>
#include <AVUIToolTipService.h>
#include <AVUIBindingOperations.h>
#include <AVUIMouse.h>
#include <AVUIPopup.h>
#include <AVUIBinding.h>
#include <AVUIPropertyPath.h>
#include <AVUIContextMenu.h>

REGISTER_ELEMENT_BEGIN(AVUI::PopupControlService)
    REGISTER_PROPERTY(Owner);
REGISTER_ELEMENT_END()

namespace AVUI {

PopupControlService* PopupControlService::get_Current()
{
    return InputManager::get_Current()->get_PopupControlService();
}

void PopupControlService::ResetToolTipTimer()
{
    if (m_pToolTipTimer != NULL)
    {
        m_pToolTipTimer->Stop();
        m_pToolTipTimer = NULL;
        m_quickShow = false;
    }
}

void PopupControlService::PostProcessInput(MouseEventArgs* pArgs)
{
    if(pArgs == NULL)
    {
        LibraryCriticalError();
    }

    if(pArgs->IsType(GET_TYPE(MouseButtonEventArgs)))
    {
        MouseButtonEventArgs* pButtonArgs = (MouseButtonEventArgs*) pArgs;

        if(pButtonArgs->get_ButtonState() == MouseButtonState::Pressed)
        {
            RaiseToolTipClosingEvent(true);
        }
        else
        {
            ProcessMouseUp(pButtonArgs);
        }
    }
    else
    {
        OnMouseMove(Mouse::get_Current()->get_MouseOver());
    }   
}

void PopupControlService::ProcessMouseUp(MouseButtonEventArgs* pButtonArgs)
{
    RaiseToolTipClosingEvent(false);

    if (!pButtonArgs->get_IsHandled() && pButtonArgs->get_ChangedButton() == MouseButton::Right && 
        Mouse::get_Current()->get_RightButton() == MouseButtonState::Released)
    {
        UIElement* pDirectlyOver = Mouse::get_Current()->get_DirectlyOver();
        if (pDirectlyOver != NULL)
        {
            Point position = pButtonArgs->GetPosition(pDirectlyOver);
            if (RaiseContextMenuOpeningEvent(pDirectlyOver, position.get_X(), position.get_Y(), pButtonArgs->get_IsUserInitiated()))
            {
                pButtonArgs->set_IsHandled(true);
            }
        }
    }
}

void PopupControlService::OnRaiseToolTipClosingEvent(Object* pSender, EventArgs* e)
{
    RaiseToolTipClosingEvent(false);
}

void PopupControlService::RaiseToolTipClosingEvent(bool reset)
{
    ResetToolTipTimer();
    if (reset)
    {
        set_LastChecked(NULL);
    }
    DependencyObject* pLastMouseOverWithToolTip = get_LastMouseOverWithToolTip();
    if (pLastMouseOverWithToolTip && m_pCurrentToolTip != NULL)
    {
        bool isOpen = m_pCurrentToolTip->get_IsOpen();
        if (isOpen)
        {
            UIElement* pElement = object_cast<UIElement>(pLastMouseOverWithToolTip);
            if (pElement != NULL)
            {
                ToolTipEventArgs e(false);

                pElement->RaiseEvent(&e);
            }
        }
        if (isOpen)
        {
            m_pCurrentToolTip->set_IsOpen(false);
            m_pForceCloseTimer = object_allocate<DispatcherTimer>();
            m_pForceCloseTimer->set_Interval(Popup::get_AnimationDelayTime());
            m_pForceCloseTimer->add_Tick(EventHandler::Create(this, &PopupControlService::OnForceClose));
            m_pForceCloseTimer->set_Tag(m_pCurrentToolTip);
            m_pForceCloseTimer->Start();
            m_quickShow = true;

            set_ToolTipTimer(object_allocate<DispatcherTimer>());
            get_ToolTipTimer()->set_Interval(TimeSpan::FromMilliseconds(ToolTipService::GetBetweenShowDelay(pLastMouseOverWithToolTip)));
            get_ToolTipTimer()->add_Tick(EventHandler::Create(this, &PopupControlService::OnBetweenShowDelay));
            get_ToolTipTimer()->Start();
        }
        else
        {
            m_pCurrentToolTip->ClearValue(get_OwnerProperty());
            if (m_ownToolTip)
            {
                BindingOperations::ClearBinding(m_pCurrentToolTip, &ContentControl::get_ContentProperty());
            }
        }
        m_pCurrentToolTip = NULL;
    }
}

void PopupControlService::OnForceClose(Object* pSender, EventArgs* e)
{
    m_pForceCloseTimer->Stop();
    ((ToolTip*) m_pForceCloseTimer->get_Tag())->ForceClose();
    m_pForceCloseTimer = NULL;
}

void PopupControlService::OnBetweenShowDelay(Object* pSender, EventArgs* e)
{
    ResetToolTipTimer();
}
 
void PopupControlService::OnMouseMove(UIElement* pDirectlyOver)
{
    if (pDirectlyOver != get_LastMouseDirectlyOver())
    {
        set_LastMouseDirectlyOver(pDirectlyOver);
        if (pDirectlyOver != get_LastMouseOverWithToolTip())
        {
            InspectElementForToolTip(pDirectlyOver);
        }
    }
}

void PopupControlService::InspectElementForToolTip(DependencyObject* pDO)
{
    DependencyObject* pObj2 = pDO;
    if (LocateNearestToolTip(&pDO))
    {
        if (pDO != NULL)
        {
            if (get_LastMouseOverWithToolTip() != NULL)
            {
                RaiseToolTipClosingEvent(true);
            }
            set_LastChecked(pObj2);
            set_LastMouseOverWithToolTip(pDO);
            bool flag = m_quickShow;
            ResetToolTipTimer();
            if (flag)
            {
                m_quickShow = false;
                RaiseToolTipOpeningEvent();
            }
            else
            {
                set_ToolTipTimer(object_allocate<DispatcherTimer>());
                get_ToolTipTimer()->set_Interval(TimeSpan::FromMilliseconds(ToolTipService::GetInitialShowDelay(pDO)));
                get_ToolTipTimer()->set_Tag(BoxValue(true));
                get_ToolTipTimer()->add_Tick(EventHandler::Create(this, &PopupControlService::OnRaiseToolTipOpeningEvent));
                get_ToolTipTimer()->Start();
            }
        }
    }
    else
    {
        RaiseToolTipClosingEvent(true);
        set_LastMouseOverWithToolTip(NULL);
    }
}

 
void PopupControlService::OnRaiseToolTipOpeningEvent(Object* pSender, EventArgs* e)
{
    RaiseToolTipOpeningEvent();
}

 

void PopupControlService::RaiseToolTipOpeningEvent()
{
    ResetToolTipTimer();
    if (m_pForceCloseTimer != NULL)
    {
        OnForceClose(NULL, NULL);
    }
    DependencyObject* pLastMouseOverWithToolTip = get_LastMouseOverWithToolTip();
    if (pLastMouseOverWithToolTip != NULL)
    {
        bool flag = true;
        UIElement* pElement = object_cast<UIElement>(pLastMouseOverWithToolTip);
        if (pElement != NULL)
        {
            ToolTipEventArgs e(true);
            pElement->RaiseEvent(&e);
            flag = !e.get_IsHandled();
        }
        if (flag)
        {
            ToolTip* pToolTip = object_cast<ToolTip>(ToolTipService::GetToolTip(pLastMouseOverWithToolTip));
            if (pToolTip != NULL)
            {
                m_pCurrentToolTip = pToolTip;
                m_ownToolTip = false;
            }
            else if (m_pCurrentToolTip == NULL || !m_ownToolTip)
            {
                m_pCurrentToolTip = object_allocate<ToolTip>();
                m_ownToolTip = true;
                m_pCurrentToolTip->SetValue(get_ServiceOwnedProperty(), BoxValue(true));
                TRefCountedPtr<Binding> pBinding = object_allocate<Binding>();

//                pBinding->set_Path(object_allocate<PropertyPath>(ToolTipService::get_ToolTipProperty()));
                pBinding->set_Path(object_allocate<PropertyPath>("ToolTip"));
                pBinding->set_Mode(BindingMode::OneWay);
                pBinding->set_Source(pLastMouseOverWithToolTip);
                m_pCurrentToolTip->SetBinding(ContentControl::get_ContentProperty(), pBinding);
            }

            m_pCurrentToolTip->SetValue(get_OwnerProperty(), m_pLastMouseOverWithToolTip);
            m_pCurrentToolTip->add_Closed(RoutedEventHandler::Create(this, &PopupControlService::OnToolTipClosed));
            m_pCurrentToolTip->set_IsOpen(true);
            set_ToolTipTimer(object_allocate<DispatcherTimer>());
            get_ToolTipTimer()->set_Interval(TimeSpan::FromMilliseconds(ToolTipService::GetShowDuration(pLastMouseOverWithToolTip)));
            get_ToolTipTimer()->add_Tick(EventHandler::Create(this, &PopupControlService::OnRaiseToolTipClosingEvent));
            get_ToolTipTimer()->Start();
        }
    }
}

void PopupControlService::OnToolTipClosed(Object* pSender, RoutedEventArgs* e)
{
    ToolTip* pTarget = (ToolTip*) pSender;
    pTarget->remove_Closed(RoutedEventHandler::Create(this, &PopupControlService::OnToolTipClosed));
    pTarget->ClearValue(get_OwnerProperty());
    if (UnboxValue<bool>(pTarget->GetValue(get_ServiceOwnedProperty())))
    {
        BindingOperations::ClearBinding(pTarget, &ContentControl::get_ContentProperty());
    }
}
 

bool PopupControlService::LocateNearestToolTip(DependencyObject** ppDO)
{
    UIElement* pElement = object_cast<UIElement>(*ppDO);
    if (pElement != NULL)
    {
        FindToolTipEventArgs e;
        pElement->RaiseEvent(&e);
        if (e.get_TargetElement() != NULL)
        {
            *ppDO = e.get_TargetElement();
            return true;
        }
        if (e.get_KeepCurrentActive())
        {
            *ppDO = NULL;
            return true;
        }
    }
    return false;
}

 
bool PopupControlService::StopLookingForToolTip(DependencyObject* pDO)
{
    if (pDO != get_LastChecked() && pDO != get_LastMouseOverWithToolTip() && pDO != m_pCurrentToolTip && !WithinCurrentToolTip(pDO))
    {
        return false;
    }
    return true;
}

bool PopupControlService::WithinCurrentToolTip(DependencyObject* pDO)
{
    if (m_pCurrentToolTip == NULL)
    {
        return false;
    }
    Visual* pObj2 = object_cast<Visual>(pDO);

    if (pObj2 == NULL)
    {
        return false;
    }
    return pObj2->IsDescendantOf(m_pCurrentToolTip);
}

 
bool PopupControlService::IsElementEnabled(DependencyObject* pDO)
{
    UIElement* pElement = object_cast<UIElement>(pDO);
    if (pElement != NULL)
    {
        return pElement->get_IsEnabled();
    }
    return true;
}

TRefCountedPtr<Object> PopupControlService::CoerceProperty(DependencyObject* pDO, Object* pValue, const DependencyProperty* pDP)
{
    DependencyObject* pObj2 = (DependencyObject*) (Object*) pDO->GetValue(get_OwnerProperty());
    if (pObj2 != NULL)
    {
        bool flag;
        if ((pObj2->GetValueSource(*pDP, NULL, &flag) != BaseValueSource::Default) || flag)
        {
            return pObj2->GetValue(*pDP);
        }
        if ((pDP != &ToolTip::get_PlacementTargetProperty()) && (pDP != &ContextMenu::get_PlacementTargetProperty()))
        {
            return pValue;
        }
        UIElement* pTarget = GetTarget(pObj2);
        if (pTarget != NULL)
        {
            return pTarget;
        }
    }
    return pValue;
}
 
UIElement* PopupControlService::GetTarget(DependencyObject* pDO)
{
    return object_cast<UIElement>(pDO);    
}

DependencyObject* PopupControlService::FindParent(DependencyObject* pDO)
{
    Visual* pVisual = object_cast<Visual>(pDO);
    if(pVisual != NULL)
    {
        return pVisual->get_VisualParent();
    }
    return NULL;
    /*
    if (reference == null)
    {
        reference = o as Visual3D;
    }
    ContentElement element = (reference == null) ? (o as ContentElement) : null;
    if (element != null)
    {
        o = ContentOperations.GetParent(element);
        if (o != null)
        {
            return o;
        }
        FrameworkContentElement element2 = element as FrameworkContentElement;
        if (element2 != null)
        {
            return element2.Parent;
        }
    }
    else if (reference != null)
    {
        return VisualTreeHelper.GetParent(reference);
    }
    return null;
    */
}

bool PopupControlService::RaiseContextMenuOpeningEvent(UIElement* pSource, float x, float y, bool userInitiated)
{
    ContextMenuEventArgs args(pSource, true, x, y);

    /*
    DependencyObject* pDO = (DependencyObject*) pSource;
    if (userInitiated && pDO != NULL)
    {
        pSource->RaiseEvent(&args); //, userInitiated);
        if (InputElement.IsUIElement(o))
        {
            ((UIElement) o).RaiseEvent(args, userInitiated);
        }
        else if (InputElement.IsContentElement(o))
        {
            ((ContentElement) o).RaiseEvent(args, userInitiated);
        }
        else if (InputElement.IsUIElement3D(o))
        {
            ((UIElement3D) o).RaiseEvent(args, userInitiated);
        }
        else
        {
            source.RaiseEvent(args);
        }
    }
    else
    */
    {
        pSource->RaiseEvent(&args);
    }

    if (!args.get_IsHandled())
    {
        DependencyObject* pTargetElement = args.get_TargetElement();
        if (pTargetElement == NULL || !ContextMenuService::ContextMenuIsEnabled(pTargetElement))
        {
            return false;
        }
        ContextMenu* pContextMenu = object_cast<ContextMenu>(ContextMenuService::GetContextMenu(pTargetElement));
        pContextMenu->SetValue(get_OwnerProperty(), pTargetElement);
        pContextMenu->add_ContextMenuClosed(RoutedEventHandler::Create(this, &PopupControlService::OnContextMenuClosed));

        if ((x == -1.0f) && (y == -1.0f))
        {
            pContextMenu->set_Placement(PlacementMode::Center);
        }
        else
        {
            pContextMenu->set_Placement(PlacementMode::MousePoint);
        }
        RaiseToolTipClosingEvent(true);
        pContextMenu->set_IsOpen(true);
        return true;
    }
    RaiseToolTipClosingEvent(true);
    return true;
}

void PopupControlService::OnContextMenuClosed(Object* pSource, RoutedEventArgs* pArgs)
{
    ContextMenu* pMenu = object_cast<ContextMenu>(pSource);
    if (pMenu != NULL)
    {
        pMenu->remove_ContextMenuClosed(RoutedEventHandler::Create(this, &PopupControlService::OnContextMenuClosed));
        DependencyObject* pDO = (DependencyObject*) (Object*)pMenu->GetValue(get_OwnerProperty());
        if (pDO != NULL)
        {
            pMenu->ClearValue(get_OwnerProperty());
            UIElement* pTarget = GetTarget(pDO);
            if (pTarget != NULL && !IsPresentationSourceNull(pTarget))
            {
                //IInputElement element2 = ((o is ContentElement) || (o is UIElement3D)) ? ((IInputElement) o) : target;
                //ContextMenuEventArgs args = new ContextMenuEventArgs(element2, false);
                //element2.RaiseEvent(args);

                ContextMenuEventArgs args(pTarget, false);
                pTarget->RaiseEvent(&args);
            }
        }
    }
} 
 
bool PopupControlService::IsPresentationSourceNull(DependencyObject* pUIE)
{
    return (PresentationSource::CriticalFromVisual(object_cast<Visual>(pUIE)) == NULL);
}

 

 
 

 

 
 

 

 





}; // namespace AVUI
