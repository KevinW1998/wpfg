#include <AVUICommon.h>
#include <AVUIWindowFrame.h>
#include <AVUISolidColorBrush.h>
#include <AVUIPen.h>
#include <AVUIInputManager.h>
#include <AVUIDispatcher.h>
#include <AVUICursors.h>
#include <AVUIWindow.h>

REGISTER_ELEMENT_BEGIN(AVUI::WindowFrame)
REGISTER_ELEMENT_END()
 
namespace AVUI {

WindowFrame::WindowFrame(Window* pAttachedWindow)
{
    m_titleBarHeight = 20.0f;
    m_frameThickness = 5.0f;
    m_pBorderPen = object_allocate<Pen>();
    m_pBorderPen->set_Brush(object_allocate<SolidColorBrush>(KnownColor::Gray));
    m_pBorderPen->set_Thickness(m_frameThickness);
    m_pTitleBarBrush = object_allocate<SolidColorBrush>(KnownColor::Blue);
    m_isDragging = false;
    m_pAttachedWindow = pAttachedWindow;


    m_resizeMode = ResizeMode::CanResize;
    m_windowState = WindowState::Normal;
    m_windowStyle = WindowStyle::SingleBorderWindow;
}



void WindowFrame::OnRender(DrawingContext& drawingContext)
{
    if(get_IsWindowFrameVisible())
    {
        if(get_IsTitleBarVisible())
        {
            drawingContext.DrawRoundedRectangle(m_pBorderPen->get_Brush(), NULL,
                                                Rect(Point(0, 0), Size(get_RenderSize().get_Width(), m_titleBarHeight + m_frameThickness)),
                                                0.0f, 0.0f);
        }

        drawingContext.DrawRoundedRectangle(NULL, m_pBorderPen, 
                                            Rect(Point(m_frameThickness / 2.0f, m_frameThickness / 2.0f), Size(get_RenderSize().get_Width() - m_frameThickness, get_RenderSize().get_Height() - m_frameThickness)),
                                            0.0f, 0.0f);
    }
}

Size WindowFrame::MeasureOverride(const Size& constraintSize)
{
    return constraintSize;
}

Size WindowFrame::ArrangeOverride(const Size& constraintSize)
{
    return constraintSize;
}

Rect WindowFrame::get_ContentArea(const Size& parentSize)
{
    if(get_IsWindowFrameVisible())
    {
        float titleBarHeight = 0.0f;

        if(get_IsTitleBarVisible())
        {
            titleBarHeight = m_titleBarHeight;
        }

        return Rect(m_frameThickness, m_frameThickness + titleBarHeight, parentSize.get_Width() - (2.0f * m_frameThickness), parentSize.get_Height() - (2.0f * m_frameThickness) - titleBarHeight);
    }

    return Rect(parentSize);
}

void WindowFrame::OnQueryCursor(QueryCursorEventArgs* pArgs)
{
    UIElement::OnQueryCursor(pArgs);

    if(get_IsResizable() || get_IsMoveable())
    {
        Point mousePoint = Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_MousePosition(this);
    
        switch(GetResizeDirectionForMousePosition(mousePoint))
        {
        case WindowResizeDirection::TopLeft:
        case WindowResizeDirection::BottomRight:
            pArgs->set_Cursor(Cursors::get_SizeNWSE());
            pArgs->set_IsHandled(true);
            break;
    
        case WindowResizeDirection::TopRight:
        case WindowResizeDirection::BottomLeft:
            pArgs->set_Cursor(Cursors::get_SizeNESW());
            pArgs->set_IsHandled(true);
            break;
    
        case WindowResizeDirection::Right:
        case WindowResizeDirection::Left:
            pArgs->set_Cursor(Cursors::get_SizeWE());
            pArgs->set_IsHandled(true);
            break;
    
        case WindowResizeDirection::Top:
        case WindowResizeDirection::Bottom:
            pArgs->set_Cursor(Cursors::get_SizeNS());
            pArgs->set_IsHandled(true);
            break;

        case WindowResizeDirection::TitleBar:
            pArgs->set_Cursor(Cursors::get_Arrow());
            pArgs->set_IsHandled(true);
            break;
        }
    }
}

void WindowFrame::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    UIElement::OnMouseLeftButtonDown(pArgs);

    Point localMousePoint = Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_MousePosition(this);

    if(pArgs->get_ClickCount() == 2)
    {
        if(Rect(m_frameThickness, m_frameThickness, get_RenderSize().get_Width() - m_frameThickness, m_titleBarHeight).Contains(localMousePoint))
        {
            if(m_windowState == WindowState::Maximized)
            {
                m_pAttachedWindow->set_WindowState(WindowState::Normal);
            }
            else if(m_windowState == WindowState::Normal)
            {
                m_pAttachedWindow->set_WindowState(WindowState::Maximized);
            }
        }       
    }
    else
    {
        if((get_IsResizable() || get_IsMoveable()) && !m_isDragging)
        {
            m_originDragPoint = Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_MousePosition(NULL);
            m_previousPosition = Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_MousePosition(NULL);

            m_resizeDirection = GetResizeDirectionForMousePosition(localMousePoint);
            if(m_resizeDirection != WindowResizeDirection::None)
            {
                pArgs->set_IsHandled(true);
                CaptureMouse();
                m_isDragging = true;
            }

            /*
            set_IsDragging(true);

            Point originThumbPoint = get_InputManager()->get_MousePosition(this);
            Point previousPosition = get_InputManager()->get_MousePosition(NULL);

            DragStartedEventArgs dragStartedEventArgs(m_originThumbPoint.get_X(), m_originThumbPoint.get_Y());
            RaiseEvent(&dragStartedEventArgs);
            */
        }
    }

}

void WindowFrame::OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs)
{
    UIElement::OnMouseLeftButtonUp(pArgs);

    if((get_IsResizable() || get_IsMoveable()) && get_IsMouseCaptured() && m_isDragging)
    {
        pArgs->set_IsHandled(true);
        m_isDragging = false;
        ReleaseMouseCapture();

        /*
        Point currentPosition = get_InputManager()->get_MousePosition(this);

        DragCompletedEventArgs dragCompletedEventArgs(currentPosition.get_X() - m_originThumbPoint.get_X(),
                                                      currentPosition.get_Y() - m_originThumbPoint.get_Y(), false);
        RaiseEvent(&dragCompletedEventArgs);
        */
    }
}

void WindowFrame::OnMouseMove(MouseEventArgs* pArgs)
{
    if(!m_isDragging || (!get_IsResizable() && !get_IsMoveable()))
    {
        return;
    }

    pArgs->set_IsHandled(true);

    Point currentDragPoint = Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_MousePosition(NULL);
    Point windowLocation = m_pAttachedWindow->get_WindowLocation();
    Size windowSize = m_pAttachedWindow->get_WindowSize();

    switch(m_resizeDirection)
    {
    case WindowResizeDirection::Right:
        {
            m_pAttachedWindow->set_WindowSize(Size(currentDragPoint.get_X() - windowLocation.get_X(), windowSize.get_Height()));
            break;
        }

    case WindowResizeDirection::Left:
        {
            float xDiff = windowLocation.get_X() - currentDragPoint.get_X();

            m_pAttachedWindow->set_WindowLocation(Point(currentDragPoint.get_X(), windowLocation.get_Y()));
            m_pAttachedWindow->set_WindowSize(Size(windowSize.get_Width() + xDiff, windowSize.get_Height()));
            break;
        }

    case WindowResizeDirection::Top:
        {

            float yDiff = windowLocation.get_Y() - currentDragPoint.get_Y();

            m_pAttachedWindow->set_WindowLocation(Point(windowLocation.get_X(), currentDragPoint.get_Y()));
            m_pAttachedWindow->set_WindowSize(Size(windowSize.get_Width(), windowSize.get_Height() + yDiff));
            break;
        }

    case WindowResizeDirection::TopLeft:
        {
            float xDiff = windowLocation.get_X() - currentDragPoint.get_X();
            float yDiff = windowLocation.get_Y() - currentDragPoint.get_Y();

            m_pAttachedWindow->set_WindowLocation(Point(currentDragPoint.get_X(), currentDragPoint.get_Y()));
            m_pAttachedWindow->set_WindowSize(Size(windowSize.get_Width() + xDiff, windowSize.get_Height() + yDiff));
            break;
        }

    case WindowResizeDirection::TopRight:
        {
            float yDiff = windowLocation.get_Y() - currentDragPoint.get_Y();
    
            m_pAttachedWindow->set_WindowLocation(Point(windowLocation.get_X(), currentDragPoint.get_Y()));
            m_pAttachedWindow->set_WindowSize(Size(currentDragPoint.get_X() - windowLocation.get_X(), windowSize.get_Height() + yDiff));
            break;
        }

    case WindowResizeDirection::BottomLeft:
        {
            float xDiff = windowLocation.get_X() - currentDragPoint.get_X();

            m_pAttachedWindow->set_WindowLocation(Point(currentDragPoint.get_X(), windowLocation.get_Y()));
            m_pAttachedWindow->set_WindowSize(Size(windowSize.get_Width() + xDiff, currentDragPoint.get_Y() - windowLocation.get_Y()));
            break;
        }

    case WindowResizeDirection::Bottom:
        {
            m_pAttachedWindow->set_WindowSize(Size(windowSize.get_Width(), currentDragPoint.get_Y() - windowLocation.get_Y()));
            break;
        }
    case WindowResizeDirection::BottomRight:
        {
            m_pAttachedWindow->set_WindowSize(Size(currentDragPoint.get_X() - windowLocation.get_X(), currentDragPoint.get_Y() - windowLocation.get_Y()));
            break;
        }
    case WindowResizeDirection::TitleBar:
        {
            float xDelta = currentDragPoint.get_X() - m_previousPosition.get_X();
            float yDelta = currentDragPoint.get_Y() - m_previousPosition.get_Y();

            m_pAttachedWindow->set_WindowLocation(Point(windowLocation.get_X() + xDelta, windowLocation.get_Y() + yDelta));
            break;
        }
    }

    m_previousPosition = currentDragPoint;
}

WindowFrame::WindowResizeDirection::Enum WindowFrame::GetResizeDirectionForMousePosition(Point& pt)
{
    Size renderSize = get_RenderSize();

    if(get_IsMoveable())
    {
        if(Rect(m_frameThickness, m_frameThickness, renderSize.get_Width() - m_frameThickness, m_titleBarHeight).Contains(pt))
        {
            return WindowResizeDirection::TitleBar;
        }
    }

    if(Rect(0, 0, m_titleBarHeight, m_titleBarHeight).Contains(pt))
    {
        return WindowResizeDirection::TopLeft;
    }

    if(Rect(renderSize.get_Width() - m_titleBarHeight, 0, m_titleBarHeight, m_titleBarHeight).Contains(pt))
    {
        return WindowResizeDirection::TopRight;
    }

    if(Rect(renderSize.get_Width() - m_titleBarHeight, renderSize.get_Height() - m_titleBarHeight, m_titleBarHeight, m_titleBarHeight).Contains(pt))
    {
        return WindowResizeDirection::BottomRight;
    }

    if(Rect(0, renderSize.get_Height() - m_titleBarHeight, m_titleBarHeight, m_titleBarHeight).Contains(pt))
    {
        return WindowResizeDirection::BottomLeft;
    }

    if(Rect(0, 0, renderSize.get_Width(), m_frameThickness).Contains(pt))
    {
        return WindowResizeDirection::Top;
    }

    if(Rect(0, 0, m_frameThickness, renderSize.get_Height()).Contains(pt))
    {
        return WindowResizeDirection::Left;
    }

    if(Rect(renderSize.get_Width() - m_frameThickness, 0, m_frameThickness, renderSize.get_Height()).Contains(pt))
    {
        return WindowResizeDirection::Right;
    }

    if(Rect(0, renderSize.get_Height() - m_frameThickness, renderSize.get_Width(), m_frameThickness).Contains(pt))
    {
        return WindowResizeDirection::Bottom;
    }

    return WindowResizeDirection::None;
}

void WindowFrame::set_ResizeMode(ResizeMode::Enum resizeMode)
{
    m_resizeMode = resizeMode;
    InvalidateMeasure();
}

void WindowFrame::set_WindowState(WindowState::Enum windowState)
{
    m_windowState = windowState;
    InvalidateMeasure();
}

void WindowFrame::set_WindowStyle(WindowStyle::Enum windowStyle)
{
    m_windowStyle = windowStyle;
    InvalidateMeasure();
}

bool WindowFrame::get_IsResizable()
{
    return m_windowState == WindowState::Normal && 
               (m_resizeMode == ResizeMode::CanResize || m_resizeMode == ResizeMode::CanResizeWithGrip);
}

bool WindowFrame::get_IsWindowFrameVisible()
{
    if(m_resizeMode == ResizeMode::CanResize || m_resizeMode == ResizeMode::CanResizeWithGrip)
    {
        return true;
    }

    if(m_windowStyle != WindowStyle::None)
    {
        return true;
    }

    return false;
}

bool WindowFrame::get_IsTitleBarVisible()
{
    return m_windowStyle != WindowStyle::None;
}

bool WindowFrame::get_IsMoveable()
{
    return (m_windowStyle != WindowStyle::None) && (m_windowState != WindowState::Maximized);
}



}; // namespace AVUI
