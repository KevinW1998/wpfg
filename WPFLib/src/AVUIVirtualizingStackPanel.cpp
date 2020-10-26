#include <AVUICommon.h>
#include <AVUIVirtualizingStackPanel.h>
#include <AVUIItemsControl.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemCollection.h>
#include <AVUIScrollViewer.h>
#include <AVUIScrollContentPresenter.h>
#include <AVUIKeyboard.h>
#include <AVUILayoutManager.h>

REGISTER_ELEMENT(AVUI::CleanUpVirtualizedItemEventArgs);

REGISTER_ELEMENT_BEGIN(AVUI::VirtualizingStackPanel)

    REGISTER_INTERFACE(IScrollInfo)
    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()


namespace AVUI
{


Size VirtualizingStackPanel::MeasureOverride(const Size& constraint)
{
    int num;
    float width;
    UIElementCollection* pChildren = get_Children();
    Size size;
    Size availableSize = constraint;

    bool isHorizontal = get_Orientation() == Orientation::Horizontal;
    int num2 = -1;

    ItemContainerGenerator* pGenerator = get_ItemContainerGenerator();
    ItemsControl* pItemsControl = ItemsControl::GetItemsOwner(this);

    int numberOfItems = pItemsControl != NULL ? pItemsControl->get_Items()->get_Count() : 0;
    bool isVirtualizing = (pItemsControl != NULL) ? GetIsVirtualizing(pItemsControl) : true;
    if (isHorizontal)
    {
        availableSize.set_Width(Float::get_PositiveInfinity());
        if (get_IsScrolling() && get_CanVerticallyScroll())
        {
            availableSize.set_Height(Float::get_PositiveInfinity());
        }
        num = get_IsScrolling() ? CoerceOffsetToInteger(m_scrollData.Offset.get_X(), numberOfItems) : 0;
        width = constraint.get_Width();
    }
    else
    {
        availableSize.set_Height(Float::get_PositiveInfinity());
        if (get_IsScrolling() && get_CanHorizontallyScroll())
        {
            availableSize.set_Width(Float::get_PositiveInfinity());
        }
        num = get_IsScrolling() ? CoerceOffsetToInteger(m_scrollData.Offset.get_Y(), numberOfItems) : 0;
        width = constraint.get_Height();
    }

    GeneratorPosition position = IndexToGeneratorPositionForStart(isVirtualizing ? num : 0, &m_firstVisibleChildIndex);
    int index = m_firstVisibleChildIndex;
    bool flag4 = true;
    m_visibleCount = 0;

    if (numberOfItems > 0)
    {
        m_afterTrail = 0;

        TRefCountedPtr<Object> pSentinel = pGenerator->StartAt(position, GeneratorDirection::Forward, true);
        int num7 = isVirtualizing ? num : 0;
        int num8 = numberOfItems;
        while (num7 < num8)
        {
            float height;
            bool isNewlyRealized;
            TRefCountedPtr<UIElement> pChild = object_cast<UIElement>(pGenerator->GenerateNext(&isNewlyRealized));
            if (pChild == NULL)
            {
                break;
            }
            if (isNewlyRealized)
            {
                if (index >= pChildren->get_Count())
                {
                    AddChild(pChild);
                }
                else
                {
                    InsertChild(index, pChild);
                }
                pGenerator->PrepareItemContainer(pChild);
            }
            index++;
            m_visibleCount++;
            if (pChild->get_MeasureDirty())
            {
                m_scrollData.MaxDesiredSize = Size();
            }
            pChild->Measure(availableSize);
            Size desiredSize = pChild->get_DesiredSize();
            if (isHorizontal)
            {
                size.set_Width(size.get_Width() + desiredSize.get_Width());
                size.set_Height(Float::Max(size.get_Height(), desiredSize.get_Height()));
                height = desiredSize.get_Width();
            }
            else
            {
                size.set_Width(Float::Max(size.get_Width(), desiredSize.get_Width()));
                size.set_Height(size.get_Height() + desiredSize.get_Height());
                height = desiredSize.get_Height();
            }
            if (get_IsScrolling() && num2 == -1 && num7 >= num)
            {
                width -= height;
                if (Float::LessThanOrClose(width, 0))
                {
                    num2 = num7;
                }
            }
            if ((get_IsScrolling() && isVirtualizing && num7 > num) && 
                    ((isHorizontal && size.get_Width() - desiredSize.get_Width() > constraint.get_Width()) || 
                        (!isHorizontal && (size.get_Height() - desiredSize.get_Height()) > constraint.get_Height())))
            {
                m_afterTrail++;
                if ((m_afterTrail >= 5) || Keyboard::IsFocusable(pChild))
                {
                    flag4 = false;
                    break;
                }
            }
            num7++;
        }
    }

    m_visibleStart = num;
    if (get_IsScrolling())
    {
        Size viewport = constraint;
        Vector offset = m_scrollData.Offset;                         
        bool inChildInvalidateMeasure = false;

        if (get_ScrollOwner() != NULL)
        {
            inChildInvalidateMeasure = get_ScrollOwner()->get_InChildInvalidateMeasure();
            get_ScrollOwner()->set_InChildInvalidateMeasure(false);
        }
        Size extent;
        if (isHorizontal)
        {
            extent.set_Width((float)numberOfItems);
            extent.set_Height(inChildInvalidateMeasure ? Float::Max(size.get_Height(), m_scrollData.Extent.get_Height()) : size.get_Height());
        }
        else
        {
            extent.set_Width(inChildInvalidateMeasure ? Float::Max(size.get_Width(), m_scrollData.Extent.get_Width()) : size.get_Width());
            extent.set_Height((float)numberOfItems);
        }
        if (num2 == -1)
        {
            num2 = numberOfItems - 1;
        }
        if (flag4)
        {
            index = isVirtualizing ? m_firstVisibleChildIndex : num;
            while (index > 0)
            {
                if (!PreviousChildIsGenerated(index))
                {
                    GeneratePreviousChild(index, availableSize);
                    index++;
                }
                else if (index <= m_firstVisibleChildIndex)
                {
                    (*pChildren)[index - 1]->Measure(availableSize);
                }
                float num9 = width;
                Size size6 = (*pChildren)[index - 1]->get_DesiredSize();
                if (isHorizontal)
                {
                    num9 -= size6.get_Width();
                }
                else
                {
                    num9 -= size6.get_Height();
                }
                if (Float::LessThan(num9, 0))
                {
                    break;
                }
                if (isHorizontal)
                {
                    size.set_Width(size.get_Width() + size6.get_Width());
                    size.set_Height(Float::Max(size.get_Height(), size6.get_Height()));
                }
                else
                {
                    size.set_Width(Float::Max(size.get_Width(), size6.get_Width()));
                    size.set_Height(size.get_Height() + size6.get_Height());
                }
                index--;
                width = num9;
            }
            if (index < m_firstVisibleChildIndex || !isVirtualizing) 
            {
                m_firstVisibleChildIndex = index;
            }
            m_visibleStart = num = (GetGeneratedCount(pChildren) == 0) ? 0 : GetGeneratedIndex(m_firstVisibleChildIndex);
        }
        EnsureTopCapGenerated(availableSize);
        int num10 = numberOfItems;
        int num11 = num2 - num;
        if ((num11 == 0) || Float::GreaterThanOrClose(width, 0))
        {
            num11++;
        }
        if (isHorizontal)
        {
            m_scrollData.PhysicalViewport = viewport.get_Width();
            viewport.set_Width((float)num11);
            offset.set_X((float)num);
            offset.set_Y(Float::Max(0, Float::Min(offset.get_Y(), extent.get_Height() - viewport.get_Height())));
            if (num10 > num11 && !Float::IsPositiveInfinity(constraint.get_Width()))
            {
                size.set_Width(constraint.get_Width());
            }
        }
        else
        {
            m_scrollData.PhysicalViewport = viewport.get_Height();
            viewport.set_Height((float)num11);

            offset.set_Y((float)num);
            offset.set_X(Float::Max(0, Float::Min(offset.get_X(), extent.get_Width() - viewport.get_Width())));
            if (num10 > num11 && !Float::IsPositiveInfinity(constraint.get_Height()))
            {
                size.set_Height(constraint.get_Height());
            }
        }
        size.set_Width(Float::Min(size.get_Width(), constraint.get_Width()));
        size.set_Height(Float::Min(size.get_Height(), constraint.get_Height()));
        m_scrollData.MaxDesiredSize.set_Width(Float::Max(size.get_Width(), m_scrollData.MaxDesiredSize.get_Width()));
        m_scrollData.MaxDesiredSize.set_Height(Float::Max(size.get_Height(), m_scrollData.MaxDesiredSize.get_Height()));
        size = m_scrollData.MaxDesiredSize;
        VerifyScrollingData(viewport, extent, offset);
    }
    if (isVirtualizing)
    {
        EnsureCleanupOperation(false);
    }
    return size;
}

Size VirtualizingStackPanel::ArrangeOverride(const Size& arrangeSize)
{
    UIElementCollection* pChildren = get_Children();
    ItemsControl* pItemsControl = ItemsControl::GetItemsOwner(this);
    bool flag = pItemsControl != NULL ? GetIsVirtualizing(pItemsControl) : true;
    bool isHorizontal = get_Orientation() == Orientation::Horizontal;
    Rect finalRect(arrangeSize);
    float width = 0;
    if (get_IsScrolling())
    {
        if (isHorizontal)
        {
            finalRect.set_X(ComputePhysicalFromLogicalOffset(flag ? m_firstVisibleChildIndex : m_scrollData.ComputedOffset.get_X(), true));
            finalRect.set_Y(-1 * m_scrollData.ComputedOffset.get_Y());
        }
        else
        {
            finalRect.set_X(-1 * m_scrollData.ComputedOffset.get_X());
            finalRect.set_Y(ComputePhysicalFromLogicalOffset(flag ? m_firstVisibleChildIndex : m_scrollData.ComputedOffset.get_Y(), false));
        }
    }
    int count = pChildren->get_Count();
    for (int i = 0; i < count; i++)
    {
        UIElement* pElement = (*pChildren)[i];
        if (pElement != NULL)
        {
            if (isHorizontal)
            {
                finalRect.set_X(finalRect.get_X() + width);
                width = pElement->get_DesiredSize().get_Width();
                finalRect.set_Width(width);
                finalRect.set_Height(Float::Max(arrangeSize.get_Height(), pElement->get_DesiredSize().get_Height()));
            }
            else
            {
                finalRect.set_Y(finalRect.get_Y() + width);
                width = pElement->get_DesiredSize().get_Height();
                finalRect.set_Height(width);
                finalRect.set_Width(Float::Max(arrangeSize.get_Width(), pElement->get_DesiredSize().get_Width()));
            }
            pElement->Arrange(finalRect);
        }
    }
    return arrangeSize;
}

void VirtualizingStackPanel::OnItemsMove(ItemsChangedEventArgs* pArgs)
{
    RemoveChildRange(pArgs->get_OldPosition(), pArgs->get_ItemCount(), pArgs->get_ItemUICount());
}

void VirtualizingStackPanel::OnItemsRemove(ItemsChangedEventArgs* pArgs)
{
    RemoveChildRange(pArgs->get_Position(), pArgs->get_ItemCount(), pArgs->get_ItemUICount());
}

void VirtualizingStackPanel::OnItemsReplace(ItemsChangedEventArgs* pArgs)
{
    RemoveChildRange(pArgs->get_Position(), pArgs->get_ItemCount(), pArgs->get_ItemUICount());
}
 
void VirtualizingStackPanel::RemoveChildRange(const GeneratorPosition& position, int itemCount, int itemUICount)
{
    UIElementCollection* pChildren = get_Children();
    int index = position.Index;
    if (position.Offset > 0)
    {
        index++;
    }
    if (index < GetGeneratedCount(pChildren))
    {
        int range = itemUICount;
        if (range > 0)
        {
            VirtualizingPanel::RemoveInternalChildRange(pChildren, index, range);
        }
    }
}

 


 
bool VirtualizingStackPanel::GetIsVirtualizing(DependencyObject* pDO)
{
    return UnboxValue<bool>(pDO->GetValue(get_IsVirtualizingProperty()));
}


bool VirtualizingStackPanel::get_IsScrolling()
{
    return m_scrollData.pScrollOwner != NULL;
}

int VirtualizingStackPanel::CoerceOffsetToInteger(float offset, int numberOfItems)
{
    if (Float::IsNegativeInfinity(offset))
    {
        return 0;
    }
    if (Float::IsPositiveInfinity(offset))
    {
        return (numberOfItems - 1);
    }
    int num = (int) offset;
    return Int::Max(Int::Min(numberOfItems - 1, num), 0);
}

GeneratorPosition VirtualizingStackPanel::IndexToGeneratorPositionForStart(int index, int* pChildIndex)
{
    ItemContainerGenerator* pGenerator = get_ItemContainerGenerator();
    GeneratorPosition position = (pGenerator != NULL) ? pGenerator->GeneratorPositionFromIndex(index) : GeneratorPosition(-1, index + 1);
    *pChildIndex = (position.Offset == 0) ? position.Index : (position.Index + 1);
    return position;
}

 
float VirtualizingStackPanel::ComputePhysicalFromLogicalOffset(float logicalOffset, bool fHorizontal)
{
    float num = 0;
    UIElementCollection* pChildren = get_Children();

    int count = pChildren->get_Count();
    for (int i = 0; i < logicalOffset; i++)
    {
        num -= fHorizontal ? (*pChildren)[i]->get_DesiredSize().get_Width() : (*pChildren)[i]->get_DesiredSize().get_Height();
    }
    return num;
}

 
void VirtualizingStackPanel::VerifyScrollingData(const Size& viewport, const Size& extent, const Vector& offset)
{
    bool flag = true;
    flag &= Float::AreClose(viewport, m_scrollData.Viewport);
    flag &= Float::AreClose(extent, m_scrollData.Extent);
    flag &= Float::AreClose(offset, m_scrollData.ComputedOffset);
    m_scrollData.Offset = offset;
    if (!flag)
    {
        m_scrollData.Viewport = viewport;
        m_scrollData.Extent = extent;
        m_scrollData.ComputedOffset = offset;
        OnScrollChange();
    }
}

void VirtualizingStackPanel::OnScrollChange()
{
    if (get_ScrollOwner() != NULL)
    {
        get_ScrollOwner()->InvalidateScrollInfo();
    }
}

ScrollViewer* VirtualizingStackPanel::get_ScrollOwner()
{
    EnsureScrollData();
    return m_scrollData.pScrollOwner;
}

void VirtualizingStackPanel::set_ScrollOwner(ScrollViewer* pOwner)
{
    EnsureScrollData();
    if (pOwner != m_scrollData.pScrollOwner)
    {
        ResetScrolling(this);
        m_scrollData.pScrollOwner = pOwner;
    }
}

void VirtualizingStackPanel::ResetScrolling(VirtualizingStackPanel* pElement)
{
    pElement->InvalidateMeasure();
    if (pElement->get_IsScrolling())
    {
        pElement->m_scrollData.ClearLayout();
    }
}

 
void VirtualizingStackPanel::EnsureTopCapGenerated(const Size& layoutSlotSize)
{
    m_beforeTrail = 0;
    if (m_visibleStart > 0)
    {
        int childIndex = m_firstVisibleChildIndex;

        UIElementCollection* pChildren = get_Children();
        while (m_beforeTrail < FocusTrail)
        {
            UIElement* pElement;
            if (PreviousChildIsGenerated(childIndex))
            {
                childIndex--;
                pElement = (*pChildren)[childIndex];
            }
            else
            {
                pElement = GeneratePreviousChild(childIndex, layoutSlotSize);
            }
            if (pElement == NULL || Keyboard::IsFocusable(pElement))
            {
                m_beforeTrail++;
                return;
            }
            m_beforeTrail++;
        }
    }
}

UIElement* VirtualizingStackPanel::GeneratePreviousChild(int childIndex, const Size& layoutSlotSize)
{
    TRefCountedPtr<UIElement> pChild;
    int num2;
    int index = get_ItemContainerGenerator()->IndexFromGeneratorPosition(GeneratorPosition(childIndex, 0)) - 1;
    if (index < 0)
    {
        return NULL;
    }
    ItemContainerGenerator* pGenerator = get_ItemContainerGenerator();

    UIElementCollection* pChildren = get_Children();
    GeneratorPosition position = IndexToGeneratorPositionForStart(index, &num2);

    TRefCountedPtr<Object> pSentinel = pGenerator->StartAt(position, GeneratorDirection::Forward, true);

    bool isNewlyRealized;
    pChild = object_cast<UIElement>(pGenerator->GenerateNext(&isNewlyRealized));

    InsertChild(childIndex, pChild);
    pGenerator->PrepareItemContainer(pChild);

    if (childIndex <= m_firstVisibleChildIndex)
    {
        m_firstVisibleChildIndex++;
    }
    pChild->Measure(layoutSlotSize);

    return pChild;
}

 
bool VirtualizingStackPanel::PreviousChildIsGenerated(int childIndex)
{
    GeneratorPosition position(childIndex, 0);
    position = get_ItemContainerGenerator()->GeneratorPositionFromIndex(get_ItemContainerGenerator()->IndexFromGeneratorPosition(position) - 1);
    if (position.Offset == 0)
    {
        return (position.Index >= 0);
    }
    return false;
}

int VirtualizingStackPanel::GetGeneratedCount(UIElementCollection* pChildren)
{
    if (!get_IsItemsHost())
    {
        return 0;
    }
    return pChildren->get_Count();
}

 
int VirtualizingStackPanel::GetGeneratedIndex(int childIndex)
{
    return get_ItemContainerGenerator()->IndexFromGeneratorPosition(GeneratorPosition(childIndex, 0));
}



void VirtualizingStackPanel::EnsureCleanupOperation(bool delay)
{
    if(!m_isCleanUpQueued)
    {
        get_LayoutManager()->add_LayoutUpdated(EventHandler::Create(this, &VirtualizingStackPanel::OnLayoutUpdated));
        m_isCleanUpQueued = true;
    }

    /*
    if (delay)
    {
        bool flag = true;
        if (this._cleanupOperation != null)
        {
            flag = this._cleanupOperation.Abort();
            if (flag)
            {
                this._cleanupOperation = null;
            }
        }
        if (flag && (this._cleanupDelay == null))
        {
            this._cleanupDelay = new DispatcherTimer();
            this._cleanupDelay.Tick += new EventHandler(this.OnDelayCleanup);
            this._cleanupDelay.Interval = TimeSpan.FromMilliseconds(500);
            this._cleanupDelay.Start();
        }
    }
    else if ((this._cleanupOperation == null) && (this._cleanupDelay == null))
    {
        this._cleanupOperation = base.Dispatcher.BeginInvoke(DispatcherPriority.Background, new DispatcherOperationCallback(this.OnCleanUp), null);
    }
    */
}

void VirtualizingStackPanel::OnLayoutUpdated(Object* pSender, EventArgs* pArgs)
{
    get_LayoutManager()->remove_LayoutUpdated(EventHandler::Create(this, &VirtualizingStackPanel::OnLayoutUpdated));
    m_isCleanUpQueued = false;
    CleanUp();
}


bool VirtualizingStackPanel::CleanUp()
{
    ItemsControl* pControl = ItemsControl::GetItemsOwner(this);
    if (pControl != NULL && !GetIsVirtualizing(pControl))
    {
        return false;
    }

//    int tickCount = Environment.TickCount;

    bool flag = false;
    UIElementCollection* pChildren = get_Children();
    int minDesiredGenerated = get_MinDesiredGenerated();
    int maxDesiredGenerated = get_MaxDesiredGenerated();
    int num4 = maxDesiredGenerated - minDesiredGenerated;
    int num5 = GetGeneratedCount(pChildren) - num4;
    if (num5 > (num4 * 2))
    {
        /*
        if ((Mouse.LeftButton == MouseButtonState.Pressed) && (num5 < 0x3e8))
        {
            return true;
        }
        bool isKeyboardFocusWithin = get_IsKeyboardFocusWithin();
        */
        bool flag3 = false;
        int num6 = -1;
        ItemContainerGenerator* pGenerator = get_ItemContainerGenerator();
        int cleanupRangeStart = 0;
        int cleanupCount = 0;
        int lastGeneratedIndex = -1;
        for (int i = 0; i < GetGeneratedCount(pChildren); i++)
        {
            int counterAdjust;
/*
            int num12 = Environment.TickCount - tickCount;
            if ((num12 > 50) && (cleanupCount > 0))
            {
                flag = true;
                break;
            }
*/            
            int childIndex = i;
            /*
            if (isKeyboardFocusWithin)
            {
                UIElement element = children[i];
                if (element.IsKeyboardFocusWithin)
                {
                    isKeyboardFocusWithin = false;
                    flag3 = true;
                    num6 = i;
                    if (i <= 0)
                    {
                        goto Label_0187;
                    }
                    int num14 = i - 1;
                    int num15 = Math.Max(0, i - 5);
                    while (num14 >= num15)
                    {
                        element = children[num14];
                        if (Keyboard.IsFocusable(element))
                        {
                            num14--;
                            break;
                        }
                        num14--;
                    }
                    for (childIndex = num15; childIndex <= num14; childIndex++)
                    {
                        this.ManageCleanup(children, o, generator, childIndex, minDesiredGenerated, maxDesiredGenerated, ref childIndex, ref cleanupRangeStart, ref cleanupCount, ref lastGeneratedIndex, out counterAdjust);
                        if (counterAdjust > 0)
                        {
                            i -= counterAdjust;
                            num14 -= counterAdjust;
                        }
                    }
                    if (cleanupCount > 0)
                    {
                        this.CleanupRange(children, generator, cleanupRangeStart, cleanupCount);
                        i -= cleanupCount;
                        cleanupCount = 0;
                    }
                    cleanupRangeStart = i + 1;
                    continue;
                }
                if (i < 5)
                {
                    continue;
                }
                childIndex = i - 5;
            }
            
        Label_0187:
        */
            if (flag3)
            {
                if (childIndex <= (num6 + 5))
                {
                    UIElement* pElement2 = (*pChildren)[childIndex];
                    if (Keyboard::IsFocusable(pElement2))
                    {
                        flag3 = false;
                        cleanupRangeStart = childIndex + 1;
                        cleanupCount = 0;
                    }
                    continue;
                }
                flag3 = false;
            }
            ManageCleanup(pChildren, pControl, pGenerator, childIndex, minDesiredGenerated, maxDesiredGenerated, &i, &cleanupRangeStart, &cleanupCount, &lastGeneratedIndex, &counterAdjust);
        }
        if (cleanupCount > 0)
        {
            CleanupRange(pChildren, pGenerator, cleanupRangeStart, cleanupCount);
        }
    }
    return flag;
}


int VirtualizingStackPanel::get_MinDesiredGenerated() { return Int::Max(0, m_visibleStart - m_beforeTrail); }
int VirtualizingStackPanel::get_MaxDesiredGenerated() { return Int::Min(get_ItemCount(), (m_visibleStart + m_visibleCount) + m_afterTrail); }
 
int VirtualizingStackPanel::get_ItemCount()
{
    ItemsControl* pItemsOwner = ItemsControl::GetItemsOwner(this);
    if (pItemsOwner == NULL)
    {
        return 0;
    }
    return pItemsOwner->get_Items()->get_Count();

}
 
void VirtualizingStackPanel::CleanupRange(UIElementCollection* pChildren, ItemContainerGenerator* pGenerator, int startIndex, int count)
{
    VirtualizingPanel::RemoveChildRange(startIndex, count);
    pGenerator->Remove(GeneratorPosition(startIndex, 0), count);
    if (startIndex < m_firstVisibleChildIndex)
    {
        int num = (startIndex + count) - 1;
        if (num < m_firstVisibleChildIndex)
        {
            m_firstVisibleChildIndex -= count;
        }
        else
        {
            m_firstVisibleChildIndex = startIndex;
        }
    }
}

 
void VirtualizingStackPanel::ManageCleanup(UIElementCollection* pChildren, ItemsControl* pItemsControl, ItemContainerGenerator* pGenerator, int childIndex, int minDesiredGenerated, int maxDesiredGenerated, int *pCounter, int *pCleanupRangeStart, int* pCleanupCount, int *pLastGeneratedIndex, int* pCounterAdjust)
{
    *pCounterAdjust = 0;
    bool flag = false;
    bool flag2 = false;
    int i = GetGeneratedIndex(childIndex);
    if (OutsideMinMax(i, minDesiredGenerated, maxDesiredGenerated) && NotifyCleanupItem(childIndex, pChildren, pItemsControl, pGenerator))
    {
        if ((i - *pLastGeneratedIndex) == 1)
        {
            (*pCleanupCount)++;
        }
        else
        {
            flag = flag2 = true;
        }
    }
    else
    {
        flag = true;
    }
    if (flag)
    {
        if (*pCleanupCount > 0)
        {
            CleanupRange(pChildren, pGenerator, *pCleanupRangeStart, *pCleanupCount);
            *pCounterAdjust = *pCleanupCount;
            *pCounter -= *pCounterAdjust;
            childIndex -= *pCounterAdjust;
            *pCleanupCount = 0;
        }
        if (flag2)
        {
            *pCleanupRangeStart = childIndex;
            *pCleanupCount = 1;
        }
        else
        {
            *pCleanupRangeStart = childIndex + 1;
        }
    }
    *pLastGeneratedIndex = i;
}

bool VirtualizingStackPanel::OutsideMinMax(int i, int min, int max)
{
    if (i >= min)
    {
        return (i > max);
    }
    return true;
}

bool VirtualizingStackPanel::NotifyCleanupItem(int childIndex, UIElementCollection* pChildren, ItemsControl* pItemsControl, ItemContainerGenerator* pGenerator)
{
    UIElement* pElement = (*pChildren)[childIndex];
    CleanUpVirtualizedItemEventArgs e(pItemsControl->get_ItemContainerGenerator()->ItemFromContainer(pElement), pElement);
    e.set_Source(this);
    OnCleanUpVirtualizedItem(&e);
    return !e.Cancel;
}


void VirtualizingStackPanel::OnCleanUpVirtualizedItem(CleanUpVirtualizedItemEventArgs* pArgs)
{
    ItemsControl* pItemsOwner = ItemsControl::GetItemsOwner(this);
    if (pItemsOwner != NULL)
    {
        pItemsOwner->RaiseEvent(pArgs);
    }
}



bool VirtualizingStackPanel::get_CanHorizontallyScroll()
{
    return m_scrollData.AllowHorizontal;
}
void VirtualizingStackPanel::set_CanHorizontallyScroll(bool canHorizontallyScroll)
{
    if(m_scrollData.AllowHorizontal != canHorizontallyScroll)
    {
        m_scrollData.AllowHorizontal = canHorizontallyScroll;
        InvalidateMeasure();
    }
}

bool VirtualizingStackPanel::get_CanVerticallyScroll()
{
    return m_scrollData.AllowVertical;
}
void VirtualizingStackPanel::set_CanVerticallyScroll(bool canVerticallyScroll)
{
    if(m_scrollData.AllowVertical != canVerticallyScroll)
    {
        m_scrollData.AllowVertical = canVerticallyScroll;
        InvalidateMeasure();
    }
}

float VirtualizingStackPanel::get_ExtentHeight()
{
    return m_scrollData.Extent.get_Height();
}

float VirtualizingStackPanel::get_ExtentWidth()
{
    return m_scrollData.Extent.get_Width();
}

float VirtualizingStackPanel::get_HorizontalOffset()
{
    return m_scrollData.ComputedOffset.get_X();
}

void VirtualizingStackPanel::set_HorizontalOffset(float horizontalOffset)
{
    EnsureScrollData();
    float num = ScrollContentPresenter::ValidateInputOffset(horizontalOffset);
    if (!Float::AreClose(num, m_scrollData.Offset.get_X()))
    {
        m_scrollData.Offset.set_X(num);
        InvalidateMeasure();
    }
}

float VirtualizingStackPanel::get_VerticalOffset()
{
    return m_scrollData.ComputedOffset.get_Y();
}

void VirtualizingStackPanel::set_VerticalOffset(float verticalOffset)
{
    EnsureScrollData();
    float num = ScrollContentPresenter::ValidateInputOffset(verticalOffset);
    if (!Float::AreClose(num, m_scrollData.Offset.get_Y()))
    {
        m_scrollData.Offset.set_Y(num);
        InvalidateMeasure();
    }
}

float VirtualizingStackPanel::get_ViewportWidth()
{
    return m_scrollData.Viewport.get_Width();
}

float VirtualizingStackPanel::get_ViewportHeight()
{
    return m_scrollData.Viewport.get_Height();
}


void VirtualizingStackPanel::OnItemsChanged(Object* pSender, ItemsChangedEventArgs* pArgs)
{
    VirtualizingPanel::OnItemsChanged(pSender, pArgs);

    bool flag = false;
    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Remove:
        OnItemsRemove(pArgs);
        flag = true;
        break;

    case NotifyCollectionChangedAction::Replace:
        OnItemsReplace(pArgs);
        flag = true;
        break;

    case NotifyCollectionChangedAction::Move:
        OnItemsMove(pArgs);
        break;

    case NotifyCollectionChangedAction::Reset:
        flag = true;
        break;
    }

    if (flag && get_IsScrolling())
    {
        m_scrollData.MaxDesiredSize = Size();
    }
}



Rect VirtualizingStackPanel::MakeVisible(Visual* pVisual, const Rect& rectangleIn)
{
    Vector newOffset;
    Rect newRect;
    Rect rect = rectangleIn;


    if(pVisual == NULL || pVisual == this || !IsAncestorOf(pVisual))
    {
        return Rect();
    }

    // deviation - support rect.empty?
    /*
    if (rectangle.get_IsEmpty() || (visual == null)) || ((visual == this) || !base.IsAncestorOf(visual)))
    {
        return Rect.Empty;
    }
    */

    Rect rectangleTransformed = pVisual->TransformToAncestor(this).TransformBounds(rectangleIn);
    if (!get_IsScrolling())
    {
        return rectangleTransformed;
    }
    MakeVisiblePhysicalHelper(rectangleTransformed, &newOffset, &newRect);

    int childIndex = FindChildIndexThatParentsVisual(pVisual);

    MakeVisibleLogicalHelper(childIndex, rectangleTransformed, &newOffset, &newRect);

    newOffset.set_X(ScrollContentPresenter::CoerceOffset(newOffset.get_X(), m_scrollData.Extent.get_Width(), m_scrollData.Viewport.get_Width()));
    newOffset.set_Y(ScrollContentPresenter::CoerceOffset(newOffset.get_Y(), m_scrollData.Extent.get_Height(), m_scrollData.Viewport.get_Height()));
    if (!Float::AreClose(newOffset, m_scrollData.Offset))
    {
        m_scrollData.Offset = newOffset;
        InvalidateMeasure();
        OnScrollChange();
        if (get_ScrollOwner() != NULL)
        {
            get_ScrollOwner()->MakeVisible(pVisual, rect);
        }
    }
    return newRect;
}


void VirtualizingStackPanel::MakeVisibleLogicalHelper(int childIndex, const Rect& r, Vector* pNewOffset, Rect* pNewRect)
{
    int x;
    int width;
    bool flag = get_Orientation() == Orientation::Horizontal;
    float y = r.get_Y();

    if (flag)
    {
        x = (int) m_scrollData.ComputedOffset.get_X();
        width = (int) m_scrollData.Viewport.get_Width();
    }
    else
    {
        x = (int) m_scrollData.ComputedOffset.get_Y();
        width = (int) m_scrollData.Viewport.get_Height();
    }
    int num2 = x;
    if (childIndex < x)
    {
        y = 0;
        num2 = childIndex;
    }
    else if (childIndex > ((x + width) - 1))
    {
        num2 = (childIndex - width) + 1;
        float num5 = flag ? get_RenderSize().get_Width() : get_RenderSize().get_Height();
        y = num5 * (1.0f - (1.0f / ((float)width)));
    }
    if (flag)
    {
        pNewOffset->set_X((float) num2);
        pNewRect->set_X(y);
        pNewRect->set_Width(r.get_Width());
    }
    else
    {
        pNewOffset->set_Y((float)num2);
        pNewRect->set_Y(y);
        pNewRect->set_Height(r.get_Height());
    }
}


int VirtualizingStackPanel::FindChildIndexThatParentsVisual(Visual* pVisual)
{
    Visual* pReference = pVisual;
    for (Visual* pVisual2 = pReference->get_VisualParent(); pVisual2 != this; pVisual2 = pReference->get_VisualParent())
    {
        pReference = pVisual2;
    }
    UIElementCollection* pChildren = get_Children();
    for (int i = 0; i < pChildren->get_Count(); i++)
    {
        if ((*pChildren)[i] == pReference)
        {
            return GetGeneratedIndex(i);
        }
    }
    return -1;
}

void VirtualizingStackPanel::MakeVisiblePhysicalHelper(const Rect& r, Vector* pNewOffset, Rect* pNewRect)
{
    float topView;
    float viewportHeight;
    float topChild;
    float height;
    bool flag = get_Orientation() == Orientation::Horizontal;
    if (flag)
    {
        topView = m_scrollData.ComputedOffset.get_Y();
        viewportHeight = get_ViewportHeight();
        topChild = r.get_Y();
        height = r.get_Height();
    }
    else
    {
        topView = m_scrollData.ComputedOffset.get_X();
        viewportHeight = get_ViewportWidth();
        topChild = r.get_X();
        height = r.get_Width();
    }
    topChild += topView;
    float num5 = ScrollContentPresenter::ComputeScrollOffsetWithMinimalScroll(topView, topView + viewportHeight, topChild, topChild + height);
    float num6 = Float::Max(topChild, num5);
    height = Float::Max(Float::Min(height + topChild, num5 + viewportHeight - num6), 0);
    topChild = num6;
    topChild -= topView;
    if (flag)
    {
        pNewOffset->set_Y(num5);
        pNewRect->set_Y(topChild);
        pNewRect->set_Height(height);
    }
    else
    {
        pNewOffset->set_X(num5);
        pNewRect->set_X(topChild);
        pNewRect->set_Width(height);
    }
}

 

 
 






 




VirtualizingStackPanel::ScrollData::ScrollData()
{  
    pScrollOwner = NULL;
    ClearLayout();
}

void VirtualizingStackPanel::ScrollData::ClearLayout()
{
    Offset = Vector();
    Size size;
    MaxDesiredSize = size;
    Viewport = Extent = size;
    PhysicalViewport = 0;
}



CleanUpVirtualizedItemEventArgs::CleanUpVirtualizedItemEventArgs(Object* pValueIn, UIElement* pElementIn) : RoutedEventArgs(&VirtualizingStackPanel::get_CleanUpVirtualizedItemEvent())
{
    Cancel = false;
    pElement = pElementIn;
    pValue = pValueIn;

}


 
 

 


 

 

 

 

 

 


 

 

 
 

 

 


 

 



};

