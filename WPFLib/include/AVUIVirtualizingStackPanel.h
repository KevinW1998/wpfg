#pragma once

#include <AVUIVirtualizingPanel.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIIScrollInfo.h>

namespace AVUI {

class ScrollViewer;

class CleanUpVirtualizedItemEventArgs : public RoutedEventArgs
{
public:

    CleanUpVirtualizedItemEventArgs(Object* pValue, UIElement* pElement);

    DECLARE_ELEMENT(CleanUpVirtualizedItemEventArgs, RoutedEventArgs);


    bool Cancel;
    UIElement* pElement;
    Object* pValue;
};

DEFINE_EVENTHANDLER(CleanUpVirtualizedItem);

class VirtualizingStackPanel : public VirtualizingPanel, public IScrollInfo
{
public:

    VirtualizingStackPanel() : m_isCleanUpQueued(false), m_afterTrail(0), m_beforeTrail(0), m_firstVisibleChildIndex(0),
                               m_visibleCount(0), m_visibleStart(0) { };
    ~VirtualizingStackPanel() { };

    DECLARE_ELEMENT(VirtualizingStackPanel, VirtualizingPanel);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Orientation, Orientation, Orientation::Vertical, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsVirtualizing, bool, true, PropertyFlag::None);
    IMPLEMENT_ROUTED_EVENT(CleanUpVirtualizedItem, CleanUpVirtualizedItemEventArgs, RoutingStrategy::Direct);

 

 

    static bool GetIsVirtualizing(DependencyObject* pDO);


// IScrollInfo implementation
    virtual void LineUp() { set_VerticalOffset(get_VerticalOffset() - ((get_Orientation() == Orientation::Vertical) ? 1 : 16)); }
    virtual void LineDown() { set_VerticalOffset(get_VerticalOffset() + ((get_Orientation() == Orientation::Vertical) ? 1 : 16)); }
    virtual void LineLeft() { set_HorizontalOffset(get_HorizontalOffset() - ((get_Orientation() == Orientation::Horizontal) ? 1 : 16)); }
    virtual void LineRight() { set_HorizontalOffset(get_HorizontalOffset() + ((get_Orientation() == Orientation::Horizontal) ? 1 : 16)); }
    virtual void PageUp() {  set_VerticalOffset(get_VerticalOffset() - get_ViewportHeight()); }
    virtual void PageDown() { set_VerticalOffset(get_VerticalOffset() + get_ViewportHeight()); }
    virtual void PageLeft() { set_HorizontalOffset(get_HorizontalOffset() - get_ViewportWidth()); }
    virtual void PageRight() { set_HorizontalOffset(get_HorizontalOffset() + get_ViewportWidth()); }
    virtual Rect MakeVisible(Visual* pVisual, const Rect& rect);
    virtual void MouseWheelUp() { set_VerticalOffset(get_VerticalOffset() - ((get_Orientation() == Orientation::Vertical) ? 3 : 48)); }
    virtual void MouseWheelDown() { set_VerticalOffset(get_VerticalOffset() + ((get_Orientation() == Orientation::Vertical) ? 3 : 48)); }
    virtual void MouseWheelLeft() { set_HorizontalOffset(get_HorizontalOffset() - ((get_Orientation() == Orientation::Horizontal) ? 3 : 48)); }
    virtual void MouseWheelRight() { set_HorizontalOffset(get_HorizontalOffset() - ((get_Orientation() == Orientation::Horizontal) ? 3 : 48)); }


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



protected:

    Size MeasureOverride(const Size& constraint);
    Size ArrangeOverride(const Size& constraint);

    virtual void OnCleanUpVirtualizedItem(CleanUpVirtualizedItemEventArgs* pArgs);
    virtual void OnItemsChanged(Object* pSender, ItemsChangedEventArgs* pArgs);

    virtual bool get_HasLogicalOrientation() { return true; }
    virtual Orientation::Enum get_LogicalOrientation() { return get_Orientation(); }

private:
    void OnLayoutUpdated(Object* pSender, EventArgs* pArgs);
    void OnItemsMove(ItemsChangedEventArgs* pArgs);
    void OnItemsRemove(ItemsChangedEventArgs* pArgs);
    void OnItemsReplace(ItemsChangedEventArgs* pArgs);
    void RemoveChildRange(const GeneratorPosition& position, int itemCount, int itemUICount);

 

    void OnScrollChange();

    bool get_IsScrolling();
    int get_MinDesiredGenerated();
    int get_MaxDesiredGenerated();
    int get_ItemCount();


    GeneratorPosition IndexToGeneratorPositionForStart(int index, int* pChildIndex);
    float ComputePhysicalFromLogicalOffset(float logicalOffset, bool fHorizontal);
    void VerifyScrollingData(const Size& viewport, const Size& extent, const Vector& offset);
    void EnsureTopCapGenerated(const Size& layoutSlotSize);
    UIElement* GeneratePreviousChild(int childIndex, const Size& layoutSlotSize);
    bool PreviousChildIsGenerated(int childIndex);
    int GetGeneratedCount(UIElementCollection* pChildren);
    int GetGeneratedIndex(int childIndex);
    void EnsureCleanupOperation(bool delay);
    void CleanupRange(UIElementCollection* pChildren, ItemContainerGenerator* pGenerator, int startIndex, int count);
    void ManageCleanup(UIElementCollection* pChildren, ItemsControl* pItemsControl, ItemContainerGenerator* pGenerator, int childIndex, int minDesiredGenerated, int maxDesiredGenerated, int *pCounter, int *pCleanupRangeStart, int* pCleanupCount, int *pLastGeneratedIndex, int* pCounterAdjust);
    bool CleanUp();
    void MakeVisibleLogicalHelper(int childIndex, const Rect& r, Vector* pNewOffset, Rect* pNewRect);
    void MakeVisiblePhysicalHelper(const Rect& r, Vector* pNewOffset, Rect* pNewRect);
    int FindChildIndexThatParentsVisual(Visual* pVisual);

    static bool OutsideMinMax(int i, int min, int max);
    bool NotifyCleanupItem(int childIndex, UIElementCollection* pChildren, ItemsControl* pItemsControl, ItemContainerGenerator* pGenerator);

 

    static int CoerceOffsetToInteger(float offset, int numberOfItems);
    static void ResetScrolling(VirtualizingStackPanel* pElement);

    void EnsureScrollData() { }

    struct ScrollData
    {
        ScrollData();
        void ClearLayout();

        ScrollViewer* pScrollOwner;
        bool AllowHorizontal;
        bool AllowVertical;
        Vector ComputedOffset;
        Size Extent;
        Size MaxDesiredSize;
        Vector Offset;
        float PhysicalViewport;
        Size Viewport;
    };


    int m_afterTrail;
    int m_beforeTrail;
    //TRefCountedPtr<DispatcherTimer> m_pCleanupDelay;
    //m_pDispatcherOperation
    bool m_isCleanUpQueued : 1;
    int m_firstVisibleChildIndex;
    ScrollData m_scrollData;
    int m_visibleCount;
    int m_visibleStart;
    static const int FocusTrail = 5;


};

}; // namespace AVUI

