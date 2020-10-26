
#pragma once

#include <AVUIControl.h>

namespace AVUI {

class IScrollInfo;
class ScrollChangedEventArgs;
class ScrollViewer : public Control
{
    friend class ScrollBar;
    friend class VirtualizingStackPanel;
public:
    ScrollViewer();
    ~ScrollViewer();

    DECLARE_ELEMENT(ScrollViewer, Control);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Content, UIElement, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure);

    IScrollInfo* get_ScrollInfo() { return m_pScrollInfo; }
    void set_ScrollInfo(IScrollInfo* pScrollInfo);

    Visual* get_ScrollVisual() { return m_pScrollVisual; }
    void set_ScrollVisual(Visual* pVisual) { m_pScrollVisual = pVisual; }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(HorizontalScrollBarVisibility, ScrollBarVisibility, ScrollBarVisibility::Disabled, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(VerticalScrollBarVisibility, ScrollBarVisibility, ScrollBarVisibility::Visible, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(ComputedHorizontalScrollBarVisibility, Visibility, Visibility::Visible, PropertyFlag::None);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(ComputedVerticalScrollBarVisibility, Visibility, Visibility::Visible, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CanContentScroll, bool, false, PropertyFlag::AffectsMeasure);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(VerticalOffset, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(HorizontalOffset, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ExtentWidth, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ExtentHeight, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ScrollableWidth, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ScrollableHeight, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ViewportWidth, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ViewportHeight, float, 0.0f, PropertyFlag::None);

    IMPLEMENT_ROUTED_EVENT(ScrollChanged, ScrollChangedEventArgs, RoutingStrategy::Bubble);

    void InvalidateScrollInfo();
    void MakeVisible(Visual* pVisual, const Rect& rect);

    void ScrollToHorizontalOffset(float offset);
    void ScrollToVerticalOffset(float offset);

    void PageUp();
    void PageDown();
    void PageLeft();
    void PageRight();

    void LineUp() { };
    void LineDown() { };
    void LineLeft() { };
    void LineRight() { };

    void ScrollToTop() { };
    void ScrollToBottom() { };

protected:

    virtual void OnApplyTemplate();
    virtual void OnPreApplyTemplate();
    virtual Size MeasureOverride(const Size& constraintSize);

private:

    class Commands
    {
    public:
        enum Enum
        {
            Invalid,
            LineUp,
            LineDown,
            LineLeft,
            LineRight,
            PageUp,
            PageDown,
            PageLeft,
            PageRight,
            SetHorizontalOffset,
            SetVerticalOffset,
            MakeVisible
        };
    };

    struct MakeVisibleParams
    {
        MakeVisibleParams() { }
        MakeVisibleParams(Visual* pVisual) : Child(pVisual), HasTargetRect(false) { }
        MakeVisibleParams(Visual* pVisual, const Rect& rect) : Child(pVisual), TargetRect(rect), HasTargetRect(true) { }

        TRefCountedPtr<Visual> Child;
        Rect TargetRect;
        bool HasTargetRect;
    };

    struct EnqueuedCommand
    {
        EnqueuedCommand() { }
        EnqueuedCommand(Commands::Enum command, float param, const MakeVisibleParams& makeVisibleParams) : Command(command), Param(param), MakeVisibleParamsCommand(makeVisibleParams) { }
        Commands::Enum Command;
        float Param;
        MakeVisibleParams MakeVisibleParamsCommand;
    };

    ItemStructList<EnqueuedCommand> m_commandQueue;

    void EnsureLayoutUpdatedHandler();
    void ClearLayoutUpdatedHandler();
    bool ExecuteNextCommand();
    void EnqueueCommand(Commands::Enum command, float param, const MakeVisibleParams& makeVisibleParams);
    void EnsureQueueProcessing();

    float ComputeScrollableWidth() { return Float::Max(0, m_xExtent - m_xSize); }
    float ComputeScrollableHeight() { return Float::Max(0, m_yExtent - m_ySize); }

    void OnLayoutUpdated(Object* pSender, EventArgs* pArgs);
    static void OnRequestBringIntoViewThunk(Object* pSender, RequestBringIntoViewEventArgs* pArgs);
    static void OnScrollCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs);


    bool get_InChildInvalidateMeasure() { return m_inChildInvalidateMeasure; }
    void set_InChildInvalidateMeasure(bool inChildInvalidateMeasure) { m_inChildInvalidateMeasure = inChildInvalidateMeasure; }

    void BindToTemplatedParent(const DependencyProperty& property);

    static void StaticConstructor();

    IScrollInfo* m_pScrollInfo;
    TRefCountedPtr<Visual> m_pScrollVisual;
    Visibility::Enum m_scrollVisibilityX;
    Visibility::Enum m_scrollVisibilityY;
    float m_xExtent;
    float m_yExtent;
    float m_xSize;
    float m_ySize;
    float m_xPositionISI;
    float m_yPositionISI;

    bool m_isRegisteredForLayoutUpdated : 1;
    bool m_inChildInvalidateMeasure : 1;
    bool m_isInMeasure : 1;
};



class ScrollChangedEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(ScrollChangedEventArgs, RoutedEventArgs);

    ScrollChangedEventArgs(Vector offset, Vector offsetChange, Size extent, Vector extentChange, Size viewport, Vector viewportChange) : 
                            RoutedEventArgs(&ScrollViewer::get_ScrollChangedEvent()),
                            m_offset(offset), m_offsetChange(offsetChange), m_extent(extent), m_extentChange(extentChange), 
                            m_viewport(viewport), m_viewportChange(viewportChange) { }

    // Properties
    float get_ExtentHeight() { return m_extent.get_Height(); }
    float get_ExtentHeightChange() { return m_extentChange.get_Y(); }
    float get_ExtentWidth() { return m_extent.get_Width(); }
    float get_ExtentWidthChange() { return m_extentChange.get_X(); }
    float get_HorizontalChange() { return m_offsetChange.get_X(); }
    float get_HorizontalOffset() { return m_offset.get_X(); }
    float get_VerticalChange() { return m_offsetChange.get_Y(); }
    float get_VerticalOffset() { return m_offset.get_Y(); }
    float get_ViewportHeight() { return m_viewport.get_Height(); }
    float get_ViewportHeightChange() { m_viewportChange.get_Y(); }
    float get_ViewportWidth() { return m_viewport.get_Width(); }
    float get_ViewportWidthChange() { m_viewportChange.get_X(); }


private:
    Size m_extent;
    Vector m_extentChange;
    Vector m_offset;
    Vector m_offsetChange;
    Size m_viewport;
    Vector m_viewportChange;
};





DEFINE_EVENTHANDLER(ScrollChanged);



}; // namespace AVUI
