#include <AVUICommon.h>
#include <AVUIMouse.h>
#include <AVUIUIElement.h>
#include <AVUIApplication.h>
#include <AVUIDispatcher.h>
#include <AVUITimeManager.h>
#include <AVUIInputManager.h>
#include <AVUIWindow.h>
#include <AVUIWindowManager.h>
#include <AVUICursors.h>
#include <AVUICursor.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIDeferredElementTreeState.h>
#include <AVUIReverseInheritProperties.h>
#include <AVUITransform.h>

REGISTER_ELEMENT_BEGIN(AVUI::Mouse)
    REGISTER_ROUTED_EVENT(GotMouseCapture);
    REGISTER_ROUTED_EVENT(LostMouseCapture);

    REGISTER_ROUTED_EVENT(PreviewMouseMove);
    REGISTER_ROUTED_EVENT(MouseMove);

    REGISTER_ROUTED_EVENT(PreviewMouseDownOutsideCapturedElement);
    REGISTER_ROUTED_EVENT(PreviewMouseDown);
    REGISTER_ROUTED_EVENT(MouseDown);


    REGISTER_ROUTED_EVENT(PreviewMouseUpOutsideCapturedElement);
    REGISTER_ROUTED_EVENT(PreviewMouseUp);
    REGISTER_ROUTED_EVENT(MouseUp);

    REGISTER_ROUTED_EVENT(MouseWheel);
    REGISTER_ROUTED_EVENT(MouseEnter);
    REGISTER_ROUTED_EVENT(MouseLeave);
    REGISTER_ROUTED_EVENT(QueryCursor);
REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::MouseButtonEventArgs)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::MouseEventArgs)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::QueryCursorEventArgs)

REGISTER_ELEMENT_END()

namespace AVUI {

Point MouseEventArgs::GetPosition(UIElement* pRelativeTo)
{
    return Mouse::get_Current()->get_MousePosition(pRelativeTo);
}


bool Mouse::MessageMouseMove(int xDelta, int yDelta, int zDelta)
{
    m_hasLastClickTime = false;

    m_xPosition += xDelta;
    m_yPosition += yDelta;
    m_zPosition += zDelta;

    return UpdateMouseOver();
}

bool Mouse::MessageMousePosition(int xPosition, int yPosition, int zPosition)
{
    m_hasLastClickTime = false;

    m_xPosition = xPosition;
    m_yPosition = yPosition;
    m_zPosition = zPosition;

    return UpdateMouseOver();
}

bool Mouse::MessageMouseButtonDown(MouseButton::Enum buttonDown)
{
    switch(buttonDown)
    {
    case MouseButton::Left:
        m_isLeftDown = true;
        break;
    case MouseButton::Middle:
        m_isMiddleDown = true;
        break;
    case MouseButton::Right:
        m_isRightDown = true;
        break;
    }

    bool isHandled = false;
    bool isDoubleClick = false;
    int currentTime = Dispatcher::get_CurrentDispatcher()->get_TimeManager()->get_CurrentOverallTime().get_Milliseconds();

    if(m_hasLastClickTime && (currentTime - m_dwLastClickTime) < 700)
    {
        isDoubleClick = true;
        m_hasLastClickTime = false;
    }
    else
    {
        m_dwLastClickTime = currentTime;
        m_hasLastClickTime = true;
    }

    MouseButtonEventArgs eventArgsPreviewMouseButtonDown(&get_PreviewMouseDownEvent(), buttonDown, isDoubleClick);
    MouseButtonEventArgs eventArgsMouseButtonDown(&get_MouseDownEvent(), buttonDown, isDoubleClick);

    UIElement* pInvokedElement = NULL;
    if(m_pMouseOver != NULL)
    {
        pInvokedElement = m_pMouseOver;
    }
    else if(m_pMouseCapture != NULL)
    {
        pInvokedElement = m_pMouseCapture;

        if(!m_pMouseCapture->get_IsMouseOver())
        {
            eventArgsPreviewMouseButtonDown.set_RoutedEvent(&get_PreviewMouseDownOutsideCapturedElementEvent());
        }
    }

    if(pInvokedElement != NULL)
    {
        pInvokedElement->RaiseEvent(&eventArgsPreviewMouseButtonDown);

        isHandled = eventArgsPreviewMouseButtonDown.get_IsHandled();
        if(isHandled)
        {
            eventArgsMouseButtonDown.set_IsHandled(true);
        }
        else
        {
            pInvokedElement->RaiseEvent(&eventArgsMouseButtonDown);
        }
    }


    if(!isDoubleClick && !Application::get_Current()->get_WindowManager()->get_ActiveWindow()->get_IsShowingAsDialog())
    {
        Visual* pVisual = Application::get_Current()->get_WindowManager()->ResolvePointToWindow(Point(m_xPosition * 1.0f, m_yPosition * 1.0f));

        if(pVisual != NULL && pVisual->IsType(GET_TYPE(Window)))
        {
            ((Window*)pVisual)->Activate();
            isHandled = true;
        }
    }

    InputManager::get_Current()->PostProcessInput(&eventArgsMouseButtonDown);

    return isHandled || eventArgsMouseButtonDown.get_IsHandled();;
}

bool Mouse::MessageMouseButtonUp(MouseButton::Enum buttonUp)
{
    switch(buttonUp)
    {
    case MouseButton::Left:
        m_isLeftDown = false;
        break;
    case MouseButton::Middle:
        m_isMiddleDown = false;
        break;
    case MouseButton::Right:
        m_isRightDown = false;
        break;
    }


    MouseButtonEventArgs eventArgsPreviewMouseButtonUp(&get_PreviewMouseUpEvent(), buttonUp, false);
    MouseButtonEventArgs eventArgsMouseButtonUp(&get_MouseUpEvent(), buttonUp, false);

    UIElement* pInvokedElement = NULL;
    if(m_pMouseOver != NULL)
    {
        pInvokedElement = m_pMouseOver;
    }
    else if(m_pMouseCapture != NULL)
    {
        pInvokedElement = m_pMouseCapture;

        if(!m_pMouseCapture->get_IsMouseOver())
        {
            eventArgsPreviewMouseButtonUp.set_RoutedEvent(&get_PreviewMouseUpOutsideCapturedElementEvent());
        }
    }

    if(pInvokedElement != NULL)
    {
        pInvokedElement->RaiseEvent(&eventArgsPreviewMouseButtonUp);

        bool isHandled = eventArgsPreviewMouseButtonUp.get_IsHandled();
        if(isHandled)
        {
            eventArgsMouseButtonUp.set_IsHandled(true);
        }
        else
        {
            pInvokedElement->RaiseEvent(&eventArgsMouseButtonUp);
        }
    }



    InputManager::get_Current()->PostProcessInput(&eventArgsMouseButtonUp);

    return eventArgsMouseButtonUp.get_IsHandled();;
}

UIElement* Mouse::ResolveVisualToUIElement(Visual* pVisual)
{
    while(pVisual != NULL)
    {
        if(pVisual->IsType(GET_TYPE(UIElement)))
        {
            UIElement* pUIElement = object_cast<UIElement>(pVisual);
            if(pUIElement->get_IsEnabled())
            {
                return pUIElement;
            }
        }

        pVisual = pVisual->get_VisualParent();
    }

    return NULL;
}

void Mouse::LoseMouseCapture()
{
    if(m_pMouseCapture == NULL)
    {
        return;
    }

    UIElement* pOldCapture = m_pMouseCapture;

    SetMouseCapture(NULL);

    MouseEventArgs eventArgsLostCapture(&get_LostMouseCaptureEvent());
    pOldCapture->RaiseEvent(&eventArgsLostCapture);
}

void Mouse::SetMouseCapture(UIElement* pMouseCapture)
{
    TRefCountedPtr<UIElement> pOldCapture = (UIElement*)m_pMouseCapture;
    if(m_pMouseCapture != NULL)
    {
        m_pMouseCapture->remove_IsEnabledChanged(DependencyPropertyChangedEventHandler::Create(this, &Mouse::OnCaptureIsEnabledChanged));
        m_pMouseCapture->remove_IsVisibleChanged(DependencyPropertyChangedEventHandler::Create(this, &Mouse::OnCaptureIsVisibleChanged));
        m_pMouseCapture->remove_IsHitTestVisibleChanged(DependencyPropertyChangedEventHandler::Create(this, &Mouse::OnCaptureIsHitTestVisibleChanged));

        m_pMouseCapture->ClearValue(UIElement::get_IsMouseCapturedProperty());
    }

    m_pMouseCapture = pMouseCapture;

    if(m_pMouseCapture != NULL)
    {
        m_pMouseCapture->add_IsEnabledChanged(DependencyPropertyChangedEventHandler::Create(this, &Mouse::OnCaptureIsEnabledChanged));
        m_pMouseCapture->add_IsVisibleChanged(DependencyPropertyChangedEventHandler::Create(this, &Mouse::OnCaptureIsVisibleChanged));
        m_pMouseCapture->add_IsHitTestVisibleChanged(DependencyPropertyChangedEventHandler::Create(this, &Mouse::OnCaptureIsHitTestVisibleChanged));

        m_pMouseCapture->SetValue(UIElement::get_IsMouseCapturedProperty(), BoxValue(true));
    }    


    DeferredElementTreeState* pNewTree = m_pMouseCaptureWithinTreeState;
    UIElement::get_MouseCaptureWithinProperty()->OnOriginValueChanged(pOldCapture, m_pMouseCapture, &pNewTree);
    m_pMouseCaptureWithinTreeState = pNewTree;

}

bool Mouse::IsTreeDescendant(UIElement* pParent, UIElement* pChild)
{
    while(pChild != pParent && pChild != NULL)
    {
        Visual* pVisualParent = pChild->get_VisualParent();

        if(pVisualParent != NULL)
        {
            pChild = object_cast<UIElement>(pVisualParent);
        }
        else
        {
            pChild = object_cast<UIElement>(pChild->get_Parent());
        }
    }

    return pChild == pParent;
}

bool Mouse::UpdateMouseOver()
{
    Point point(m_xPosition * 1.0f, m_yPosition * 1.0f);

    Visual* pStartElement = Application::get_Current()->get_WindowManager()->ResolvePointToWindow(point);

    UIElement* pUIElementOver = NULL;
    if(pStartElement != NULL)
    {
        DependencyObject* pEnabledHit = NULL;
        DependencyObject* pRawHit = NULL;

        point = WindowManager::ToWindowSpace(pStartElement, point);

        ((UIElement*)pStartElement)->InputHitTest(point, &pEnabledHit, &pRawHit);

        if(pEnabledHit != NULL)
        {
            pUIElementOver = ResolveVisualToUIElement(object_cast<Visual>(pEnabledHit));
        }
    }

    if(m_pMouseCapture != NULL)
    {
        if(pUIElementOver != NULL && !IsTreeDescendant(m_pMouseCapture, pUIElementOver))
        {
            pUIElementOver = NULL;
        }

        if(m_captureMode == CaptureMode::Element && pUIElementOver != NULL)
        {
            pUIElementOver = m_pMouseCapture;
        }
    }

    bool isHandled = false;

    if(pUIElementOver != NULL || m_pMouseCapture != NULL)
    {
        MouseEventArgs eventArgsMove(&get_MouseMoveEvent());

        if(m_pMouseCapture != NULL && (pUIElementOver == NULL || m_captureMode == CaptureMode::Element))
        {
            m_pMouseCapture->RaiseEvent(&eventArgsMove);
        }
        else
        {
            pUIElementOver->RaiseEvent(&eventArgsMove);
        }

        InputManager::get_Current()->PostProcessInput(&eventArgsMove);

        isHandled = eventArgsMove.get_IsHandled();
    }

    if(pUIElementOver != m_pMouseOver)
    {
        m_pMouseOver = pUIElementOver;
        UpdateMouseOverHierarchy();
    }

    UpdateCursorPrivate();

    return isHandled;
}

void Mouse::UpdateMouseOverHierarchy()
{
    UIElement* pUIElement = m_pMouseOver;
    while(pUIElement != NULL)
    {
        m_rgElementsOver.AddItem(pUIElement); 
        pUIElement = object_cast<UIElement>(pUIElement->get_VisualParent());
    }

    for(int idx = 0; idx < m_mouseOverHierarchy.get_Count(); idx++)
    {
        if(m_mouseOverHierarchy[idx] != NULL && !m_rgElementsOver.Contains(m_mouseOverHierarchy[idx]))
        {
            m_mouseOverHierarchy[idx]->ClearValue(UIElement::get_IsMouseOverProperty());

            MouseEventArgs eventArgsLeave(&get_MouseLeaveEvent());
            m_mouseOverHierarchy[idx]->RaiseEvent(&eventArgsLeave);
        }
    }

    m_mouseOverHierarchy.Clear();

    for(int idx = 0; idx < m_rgElementsOver.get_Count(); idx++)
    {
        m_mouseOverHierarchy.AddItem(m_rgElementsOver[idx]);
        if(m_rgElementsOver[idx]->get_IsMouseOver() != true)
        {
            m_rgElementsOver[idx]->SetValue(UIElement::get_IsMouseOverProperty(), BoxValue(true));

            MouseEventArgs eventArgsEnter(&get_MouseEnterEvent());
            m_rgElementsOver[idx]->RaiseEvent(&eventArgsEnter);
        }
    }

    m_rgElementsOver.Clear();
}

Visual* Mouse::FindWindowForVisual(Visual* pVisual)
{
    while(pVisual != NULL)
    {
        if(pVisual->IsType(GET_TYPE(Window)) || pVisual->IsType(GET_TYPE(PopupRoot)))
        {
            return pVisual;
        }
        pVisual = pVisual->get_VisualParent();
    }
    return NULL;
}


Point Mouse::get_MousePosition(UIElement* pElementRelativeTo)
{
    Point curPoint((float)m_xPosition, (float)m_yPosition);
    if(pElementRelativeTo == NULL)
    {
        return curPoint;
    }

    Visual* pRootVisual = FindWindowForVisual(pElementRelativeTo);

    if(pRootVisual == NULL)
    {
        return curPoint;
    }

    if(pRootVisual->IsType(GET_TYPE(Window)))
    {
        Point windowLocation = ((Window*)pRootVisual)->get_WindowLocation();
        curPoint = Point(curPoint.get_X() - windowLocation.get_X(), curPoint.get_Y() - windowLocation.get_Y());
    }
    else
    {
        Vector visualOffset = pRootVisual->get_VisualOffset();
        curPoint = Point(curPoint.get_X() - visualOffset.get_X(), curPoint.get_Y() - visualOffset.get_Y());
    }

    return pRootVisual->TransformToDescendant(pElementRelativeTo).Transform(curPoint);
}

bool Mouse::CaptureMouse(UIElement* pUIElementCapture, CaptureMode::Enum captureMode)
{
    if(captureMode == CaptureMode::None || pUIElementCapture == NULL)
    {
        LoseMouseCapture();
        return true;
    }

    if(!ValidateUIElementForCapture(pUIElementCapture))
    {
        return false;
    }

    SetMouseCapture(pUIElementCapture);
    m_captureMode = captureMode;

    MouseEventArgs eventArgsGotCapture(&get_GotMouseCaptureEvent());
    m_pMouseCapture->RaiseEvent(&eventArgsGotCapture);

    UpdateMouseOver();

    return true;
}

void Mouse::ReleaseMouseCapture(UIElement* pUIElementCapture)
{
    LoseMouseCapture();

    UpdateMouseOver();
}

void Mouse::UpdateCursor()
{
    Mouse::get_Current()->UpdateCursorPrivate();
}

void Mouse::UpdateCursorPrivate()
{
    if(m_pMouseOver == NULL)
    {
        if(m_pCurrentCursor != Cursors::get_Arrow())
        {
            m_pCurrentCursor = Cursors::get_Arrow();
            Application::get_Current()->get_ClientRendererInterface()->set_Cursor(m_pCurrentCursor->get_CursorType());
        }
        return;
    }

    if(m_pMouseCapture != NULL)
    {
        return;
    }

    QueryCursorEventArgs input;
    input.set_Cursor(Cursors::get_Arrow());
    input.set_RoutedEvent(&Mouse::get_QueryCursorEvent());

    m_pMouseOver->RaiseEvent(&input);

    if(input.get_Cursor() != m_pCurrentCursor)
    {
        m_pCurrentCursor = input.get_Cursor();

        Application::get_Current()->get_ClientRendererInterface()->set_Cursor(m_pCurrentCursor->get_CursorType());
    }
}

UIElement* Mouse::get_Captured()
{
    return get_Current()->get_MouseCapture();
}

bool Mouse::Capture(UIElement* pUIElementCapture, CaptureMode::Enum captureMode)
{
    return get_Current()->CaptureMouse(pUIElementCapture, captureMode);
}

Mouse* Mouse::get_Current()
{
    return Application::get_Current()->get_Dispatcher()->get_InputManager()->get_Mouse();
}

void Mouse::OnCaptureIsEnabledChanged(Object* pSender, DependencyPropertyChangedEventArgs* pArgs)
{
    ReevaluateCapture(NULL, NULL, true);
}

void Mouse::OnCaptureIsHitTestVisibleChanged(Object* pSender, DependencyPropertyChangedEventArgs* pArgs)
{
    ReevaluateCapture(NULL, NULL, true);
}

void Mouse::OnCaptureIsVisibleChanged(Object* pSender, DependencyPropertyChangedEventArgs* pArgs)
{
    ReevaluateCapture(NULL, NULL, true);
}

void Mouse::ReevaluateCapture(DependencyObject* pElement, DependencyObject* pOldParent, bool isCoreParent)
{
    if (pElement != NULL)
    {
        if (isCoreParent)
        {
            get_MouseCaptureWithinTreeState()->SetCoreParent(pElement, pOldParent);
        }
        else
        {
            get_MouseCaptureWithinTreeState()->SetLogicalParent(pElement, pOldParent);
        }
    }
    /*
    if (this._reevaluateCaptureOperation == null)
    {
        this._reevaluateCaptureOperation = base.Dispatcher.BeginInvoke(DispatcherPriority.Input, this._reevaluateCaptureDelegate, null);
    }
    */

    ReevaluateCaptureAsync();

}
 
DeferredElementTreeState* Mouse::get_MouseCaptureWithinTreeState()
{
    if (m_pMouseCaptureWithinTreeState == NULL)
    {
        m_pMouseCaptureWithinTreeState = object_allocate<DeferredElementTreeState>();
    }
    return m_pMouseCaptureWithinTreeState;
}

void Mouse::ReevaluateCaptureAsync()
{
    //_reevaluateCaptureOperation = null;
    if (m_pMouseCapture != NULL)
    {
        bool flag = false;

        flag = !ValidateUIElementForCapture(m_pMouseCapture);

        if (flag)
        {
            Capture(NULL);
        }
        if (m_pMouseCaptureWithinTreeState != NULL && !m_pMouseCaptureWithinTreeState->get_IsEmpty())
        {
            DeferredElementTreeState* pNewState = m_pMouseCaptureWithinTreeState;

            UIElement::get_MouseCaptureWithinProperty()->OnOriginValueChanged(m_pMouseCapture, m_pMouseCapture, &pNewState);

            m_pMouseCaptureWithinTreeState = pNewState;
        }
    }
}

bool Mouse::ValidateUIElementForCapture(UIElement* pElement)
{
    if (!pElement->get_IsEnabled())
    {
        return false;
    }
    if (!pElement->get_IsVisible())
    {
        return false;
    }
    if (!pElement->get_IsHitTestVisible())
    {
        return false;
    }
    return true;
}


MouseButtonState::Enum MouseButtonEventArgs::get_ButtonState()
{
    MouseButtonState::Enum released = MouseButtonState::Released;
    switch (m_mouseButton)
    {
    case MouseButton::Left:
        return Mouse::get_Current()->get_LeftButton();
    
    case MouseButton::Middle:
        return Mouse::get_Current()->get_MiddleButton();
    
    case MouseButton::Right:
        return Mouse::get_Current()->get_RightButton();
    }
    return released;
}


}; // namespace AVUI
