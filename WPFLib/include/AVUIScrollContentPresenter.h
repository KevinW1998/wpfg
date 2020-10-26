#pragma once
#include <AVUIFrameworkElement.h>
#include <AVUIIScrollInfo.h>

namespace AVUI {


class ScrollContentPresenter : public FrameworkElement, public IScrollInfo
{
    friend class ScrollViewer;
public:

    DECLARE_ELEMENT(ScrollContentPresenter, FrameworkElement);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Content, UIElement, NULL, DependencyPropertyMetadata(PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure, ScrollContentPresenter::OnContentChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CanContentScroll, bool, false, PropertyFlag::AffectsMeasure);

    ScrollContentPresenter() : m_pContentCache(NULL), m_pScrollInfo(NULL), m_pScrollData(NULL) { }
    virtual ~ScrollContentPresenter();


    virtual void LineUp();
    virtual void LineDown();
    virtual void LineLeft();
    virtual void LineRight();
    virtual void PageUp();
    virtual void PageDown();
    virtual void PageLeft();
    virtual void PageRight();
    virtual Rect MakeVisible(Visual* pVisual, const Rect& rect);
    virtual void MouseWheelUp();
    virtual void MouseWheelDown();
    virtual void MouseWheelLeft();
    virtual void MouseWheelRight();

    virtual bool get_CanHorizontallyScroll();
    virtual void set_CanHorizontallyScroll(bool canHorizontallyScroll);

    virtual bool get_CanVerticallyScroll();
    virtual void set_CanVerticallyScroll(bool canVerticallyScroll);

    virtual float get_ExtentHeight();
    virtual float get_ExtentWidth();
    virtual float get_HorizontalOffset();
    virtual void set_HorizontalOffset(float horizontalOffset);
    virtual float get_VerticalOffset();
    virtual void set_VerticalOffset(float verticalOffset);

    virtual float get_ViewportWidth();
    virtual float get_ViewportHeight();

    virtual void set_ScrollOwner(ScrollViewer* pScrollViewer);
    virtual ScrollViewer* get_ScrollOwner();

    static float ValidateInputOffset(float offset);
    static float CoerceOffset(float offset, float extent, float viewport);
    static float ComputeScrollOffsetWithMinimalScroll(float topView, float bottomView, float topChild, float bottomChild);

protected:

    virtual unsigned int get_VisualChildCount();
    virtual Visual* get_VisualChild(unsigned int idx);

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    virtual bool GetLayoutClip(const Size& layoutSlotSize, Rect* pRect);

private:

    bool get_IsScrollClient() { return m_pScrollInfo == this; }
    void HookupScrollingComponents();
    static void OnContentChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e);
    void ClearScrollData();
    void VerifyScrollData(Size viewport, Size extent);
    bool CoerceOffsets();

 

 

    class ScrollData
    {
    public:
        ScrollData() : CanHorizontallyScroll(false), CanVerticallyScroll(false), pScrollOwner(NULL) { }
        bool CanHorizontallyScroll;
        bool CanVerticallyScroll;
        Vector ComputedOffset;
        Size Extent;
        Vector Offset;
        ScrollViewer* pScrollOwner;
        Size Viewport;
    };

    ScrollData* EnsureScrollData();

    TWeakPtr<UIElement> m_pContentCache;
    IScrollInfo* m_pScrollInfo;
    ScrollData* m_pScrollData;

};


}; // namespace AVUI
