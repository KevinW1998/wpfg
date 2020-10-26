#pragma once

namespace AVUI
{

class ScrollViewer;

class IScrollInfo
{
public:

    virtual void LineUp() = 0;
    virtual void LineDown() = 0;
    virtual void LineLeft() = 0;
    virtual void LineRight() = 0;
    virtual void PageUp() = 0;
    virtual void PageDown() = 0;
    virtual void PageLeft() = 0;
    virtual void PageRight() = 0;
    virtual Rect MakeVisible(Visual* pVisual, const Rect& rect) = 0;
    virtual void MouseWheelUp() = 0;
    virtual void MouseWheelDown() = 0;
    virtual void MouseWheelLeft() = 0;
    virtual void MouseWheelRight() = 0;


    virtual bool get_CanHorizontallyScroll() = 0;
    virtual void set_CanHorizontallyScroll(bool canHorizontallyScroll) = 0;

    virtual bool get_CanVerticallyScroll() = 0;
    virtual void set_CanVerticallyScroll(bool canVerticallyScroll) = 0;

    virtual float get_ExtentHeight() = 0;
    virtual float get_ExtentWidth() = 0;
    virtual float get_HorizontalOffset() = 0;
    virtual void set_HorizontalOffset(float horizontalOffset) = 0;

    virtual float get_VerticalOffset() = 0;
    virtual void set_VerticalOffset(float verticalOffset) = 0;

    virtual float get_ViewportWidth() = 0;
    virtual float get_ViewportHeight() = 0;

    virtual void set_ScrollOwner(ScrollViewer* pScrollViewer) = 0;
    virtual ScrollViewer* get_ScrollOwner() = 0;
};




};

