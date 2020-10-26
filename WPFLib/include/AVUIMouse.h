#pragma once

#include <AVUIEventableObject.h>
#include <AVUIRoutedEventArgs.h>

namespace AVUI {

class UIElement;
class Visual;
class Cursor;
class DeferredElementTreeState;

class MouseEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(MouseEventArgs, RoutedEventArgs);

    MouseEventArgs(const RoutedEvent* pRoutedEvent) : RoutedEventArgs(pRoutedEvent) { }

    Point GetPosition(UIElement* pRelativeTo);

};

class MouseButtonEventArgs : public MouseEventArgs
{
public:

    DECLARE_ELEMENT(MouseButtonEventArgs, MouseEventArgs);

    MouseButtonEventArgs(const RoutedEvent* pRoutedEvent, MouseButton::Enum mouseButton, bool isDoubleClick) : MouseEventArgs(pRoutedEvent), m_mouseButton(mouseButton), m_isDoubleClick(isDoubleClick) { }

    MouseButton::Enum get_ChangedButton() { return m_mouseButton; }
    int get_ClickCount() { return m_isDoubleClick ? 2 : 1; }

    MouseButtonState::Enum get_ButtonState();

private:
    bool m_isDoubleClick;
    MouseButton::Enum m_mouseButton;
};


class QueryCursorEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(QueryCursorEventArgs, RoutedEventArgs);

    QueryCursorEventArgs() { }

    Cursor* get_Cursor() { return m_pCursor; }
    void set_Cursor(Cursor* pCursor) { m_pCursor = pCursor; }

private:

    TRefCountedPtr<Cursor> m_pCursor;
};

class CaptureMode
{
public:
    enum Enum
    {
        None,
        Element,
        SubTree
    };
};



class Mouse : public EventableObject
{
    friend class InputManager;
public:
    Mouse() : m_pMouseCapture(NULL), m_pMouseOver(NULL), m_captureMode(CaptureMode::None), m_isLeftDown(false), m_isMiddleDown(false), m_isRightDown(false) { }

    UIElement* get_MouseCapture() { return m_pMouseCapture; }
    UIElement* get_MouseOver() { return m_pMouseOver; }
    UIElement* get_DirectlyOver() { return m_pMouseOver; }

    bool MessageMouseMove(int xDelta, int yDelta, int zDelta);
    bool MessageMousePosition(int xPosition, int yPosition, int zPosition);
    bool MessageMouseButtonDown(MouseButton::Enum buttonDown);
    bool MessageMouseButtonUp(MouseButton::Enum buttonUp);

    DECLARE_ELEMENT(Mouse, Object);

    IMPLEMENT_ROUTED_EVENT(GotMouseCapture, MouseEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(LostMouseCapture, MouseEventArgs, RoutingStrategy::Bubble);

    IMPLEMENT_ROUTED_EVENT(PreviewMouseMove,  MouseEventArgs, RoutingStrategy::Tunnel);
    IMPLEMENT_ROUTED_EVENT(MouseMove,  MouseEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(PreviewMouseDown,  MouseButtonEventArgs, RoutingStrategy::Tunnel);
    IMPLEMENT_ROUTED_EVENT(PreviewMouseDownOutsideCapturedElement,  MouseButtonEventArgs, RoutingStrategy::Tunnel);
    IMPLEMENT_ROUTED_EVENT(MouseDown,  MouseButtonEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(PreviewMouseUp,  MouseButtonEventArgs, RoutingStrategy::Tunnel);
    IMPLEMENT_ROUTED_EVENT(PreviewMouseUpOutsideCapturedElement,  MouseButtonEventArgs, RoutingStrategy::Tunnel);
    IMPLEMENT_ROUTED_EVENT(MouseUp,    MouseButtonEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(MouseWheel, MouseEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(MouseEnter, MouseEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(MouseLeave, MouseEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(QueryCursor, QueryCursorEventArgs, RoutingStrategy::Bubble);

    Point get_MousePosition(UIElement* pElementRelativeTo);
    bool CaptureMouse(UIElement* pUIElementCapture) { return CaptureMouse(pUIElementCapture, CaptureMode::Element); }
    bool CaptureMouse(UIElement* pUIElementCapture, CaptureMode::Enum captureMode);
    void ReleaseMouseCapture(UIElement* pUIElementCapture);   
    bool UpdateMouseOver();

    static void UpdateCursor();

    static UIElement* get_Captured();
    static bool Capture(UIElement* pUIElementCapture, CaptureMode::Enum captureMode);
    static bool Capture(UIElement* pUIElementCapture) { return Capture(pUIElementCapture, CaptureMode::Element); }

    void set_OverrideCursor(Cursor* pCursor);

    void ReevaluateCapture(DependencyObject* pElement, DependencyObject* pOldParent, bool isCoreParent);

    static Mouse* get_Current();

    MouseButtonState::Enum get_LeftButton() { return m_isLeftDown ? MouseButtonState::Pressed : MouseButtonState::Released; }
    MouseButtonState::Enum get_MiddleButton() { return m_isMiddleDown ? MouseButtonState::Pressed : MouseButtonState::Released; }
    MouseButtonState::Enum get_RightButton() { return m_isRightDown ? MouseButtonState::Pressed : MouseButtonState::Released; }

private:
    void ReevaluateCaptureAsync();
    static bool ValidateUIElementForCapture(UIElement* pElement);
    bool IsTreeDescendant(UIElement* pParent, UIElement* pChild);
    Visual* FindWindowForVisual(Visual* pVisual);

    void OnCaptureIsEnabledChanged(Object* pSender, DependencyPropertyChangedEventArgs* pArgs);
    void OnCaptureIsHitTestVisibleChanged(Object* pSender, DependencyPropertyChangedEventArgs* pArgs);
    void OnCaptureIsVisibleChanged(Object* pSender, DependencyPropertyChangedEventArgs* pArgs);
    void SetMouseCapture(UIElement* pMouseCapture);
    DeferredElementTreeState* get_MouseCaptureWithinTreeState();

    void UpdateCursorPrivate();
    void LoseMouseCapture();
    bool IsDescendantOf(UIElement* pParent, UIElement* pChild);

    void UpdateMouseOverHierarchy();
    UIElement* ResolveVisualToUIElement(Visual* pVisual);

    TWeakPtr<UIElement> m_pMouseCapture;
    CaptureMode::Enum m_captureMode;

    TWeakPtr<UIElement> m_pMouseOver;

    ItemStructList<TWeakPtr<UIElement> > m_mouseOverHierarchy;

    int m_xPosition;
    int m_yPosition;
    int m_zPosition;

    bool m_isLeftDown : 1;
    bool m_isMiddleDown : 1;
    bool m_isRightDown : 1;

    bool m_hasLastClickTime;
    unsigned int m_dwLastClickTime;

    TRefCountedPtr<Cursor> m_pCurrentCursor;
    TRefCountedPtr<Cursor> m_pOverrideCursor;

    TRefCountedPtr<DeferredElementTreeState> m_pMouseCaptureWithinTreeState;

    ItemStructList<UIElement*> m_rgElementsOver;

};

DEFINE_EVENTHANDLER(Mouse);
DEFINE_EVENTHANDLER(MouseButton);
DEFINE_EVENTHANDLER(QueryCursor);


}; // namespace AVUI
