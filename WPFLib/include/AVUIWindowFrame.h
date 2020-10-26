#pragma once

#include <AVUIFrameworkElement.h>

namespace AVUI {

class Brush;
class Pen;

class WindowFrame : public FrameworkElement
{
    friend class Window;
public:
    WindowFrame(Window* pAttachedWindow);

    DECLARE_ELEMENT(WindowFrame, FrameworkElement);

    void set_ResizeMode(ResizeMode::Enum resizeMode);
    void set_WindowState(WindowState::Enum windowState);
    void set_WindowStyle(WindowStyle::Enum windowStyle);

protected:
    virtual void OnRender(DrawingContext& drawingContext);

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    virtual void OnQueryCursor(QueryCursorEventArgs* pArgs);

    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs);
    virtual void OnMouseMove(MouseEventArgs* pArgs);

private:

    bool get_IsResizable();
    bool get_IsMoveable();
    bool get_IsWindowFrameVisible();
    bool get_IsTitleBarVisible();

    class WindowResizeDirection
    {
    public:

        enum Enum
        {
            None,
            TopLeft,
            Top,
            TopRight,
            Left,
            Right,
            BottomLeft,
            Bottom,
            BottomRight,
            TitleBar
        };
    };

    WindowResizeDirection::Enum GetResizeDirectionForMousePosition(Point& pt);


    Rect get_ContentArea(const Size& parentSize);

    TRefCountedPtr<Pen> m_pBorderPen;
    TRefCountedPtr<Brush> m_pTitleBarBrush;

    float m_frameThickness;
    float m_titleBarHeight;
    bool m_isDragging;
    WindowResizeDirection::Enum m_resizeDirection;
    Point m_originDragPoint;
    Point m_previousPosition;
    Window* m_pAttachedWindow;

    ResizeMode::Enum m_resizeMode;
    WindowState::Enum m_windowState;
    WindowStyle::Enum m_windowStyle;

};


}; // namespace AVUI
