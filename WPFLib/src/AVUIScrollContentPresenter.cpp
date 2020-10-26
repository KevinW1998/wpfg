#include <AVUICommon.h>
#include <AVUIScrollContentPresenter.h>
#include <AVUIScrollViewer.h>
#include <AVUIItemsPresenter.h>

REGISTER_ELEMENT_BEGIN(AVUI::ScrollContentPresenter)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_INTERFACE(IScrollInfo)
    REGISTER_PROPERTY(Content);
    REGISTER_PROPERTY(CanContentScroll);
REGISTER_ELEMENT_END()

REGISTER_INTERFACE_TYPE(AVUI::IScrollInfo);

namespace AVUI {

ScrollContentPresenter::~ScrollContentPresenter()
{
    if(m_pContentCache != NULL)
    {
        RemoveVisualChild(m_pContentCache);
        m_pContentCache = NULL;
    }
    ClearScrollData();
}


void ScrollContentPresenter::OnContentChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    if(e.pOldValue == e.pNewValue)
    {
        return;
    }

    ScrollContentPresenter* pSCP = object_cast<ScrollContentPresenter>(pObject);

    UIElement* pOldValue = object_cast<UIElement>(e.pOldValue);
    UIElement* pNewValue = object_cast<UIElement>(e.pNewValue);

    if(pOldValue != NULL && pOldValue->get_VisualParent() == pSCP)
    {
        pSCP->RemoveVisualChild(pOldValue);
    }
    if(pNewValue != NULL)
    {
        pSCP->AddVisualChild(pNewValue);
    }

    pSCP->m_pContentCache = pNewValue;
}

unsigned int ScrollContentPresenter::get_VisualChildCount()
{
    if(m_pContentCache == NULL)
    {
        return 0;
    }
    return 1;
}

float ScrollContentPresenter::ComputeScrollOffsetWithMinimalScroll(float topView, float bottomView, float topChild, float bottomChild)
{
    bool flag1 = Float::LessThan(topChild, topView) && Float::LessThan(bottomChild, bottomView);
    bool flag2 = Float::GreaterThan(bottomChild, bottomView) && Float::GreaterThan(topChild, topView);
    bool flag3 = (bottomChild - topChild) > (bottomView - topView);
    if (flag1 && !flag3)
    {
        return topChild;
    }
    if (flag2 && flag3)
    {
        return topChild;
    }
    if (!flag1 && !flag2)
    {
        return topView;
    }
    return (bottomChild - (bottomView - topView));
}

 


Visual* ScrollContentPresenter::get_VisualChild(unsigned int idx)
{
    return m_pContentCache;
}

Size ScrollContentPresenter::MeasureOverride(const Size& constraintSize)
 {
    Size desiredSize;

    if(m_pContentCache != NULL)
    {
        if(!get_IsScrollClient())
        {
            m_pContentCache->Measure(constraintSize);
        }
        else
        {
            Size childConstraint(constraintSize);
            if(get_CanHorizontallyScroll())
            {
                childConstraint.set_Width(Float::get_PositiveInfinity());
            }
            if(get_CanVerticallyScroll())
            {
                childConstraint.set_Height(Float::get_PositiveInfinity());
            }
            m_pContentCache->Measure(childConstraint);
        }
        desiredSize = m_pContentCache->get_DesiredSize();
    }

    if(get_IsScrollClient())
    {
        VerifyScrollData(constraintSize, desiredSize);
    }

    desiredSize.set_Width(Float::Min(constraintSize.get_Width(), desiredSize.get_Width()));
    desiredSize.set_Height(Float::Min(constraintSize.get_Height(), desiredSize.get_Height()));

    return desiredSize;
}

bool ScrollContentPresenter::CoerceOffsets()
{
    Vector offset(CoerceOffset(m_pScrollData->Offset.get_X(), m_pScrollData->Extent.get_Width(), m_pScrollData->Viewport.get_Width()), 
                  CoerceOffset(m_pScrollData->Offset.get_Y(), m_pScrollData->Extent.get_Height(), m_pScrollData->Viewport.get_Height()));

    if(!Float::AreClose(m_pScrollData->ComputedOffset, offset))
    {
        m_pScrollData->ComputedOffset = offset;
        return false;
    }

    return true;
}

float ScrollContentPresenter::CoerceOffset(float offset, float extent, float viewport)
{
    if(offset > (extent - viewport))
    {
        offset = extent - viewport;
    }
    if(offset < 0)
    {
        offset = 0;
    }
    return offset;
}

void ScrollContentPresenter::VerifyScrollData(Size viewport, Size extent)
{
    if(Float::IsInfinity(viewport.get_Width()))
    {
        viewport.set_Width(extent.get_Width());
    }
    if(Float::IsInfinity(viewport.get_Height()))
    {
        viewport.set_Height(extent.get_Height());
    }

    bool isScrollDataValid = true;

    if(!Float::AreClose(viewport, m_pScrollData->Viewport))
    {
        isScrollDataValid = false;
        m_pScrollData->Viewport = viewport;
    }

    if(!Float::AreClose(extent, m_pScrollData->Extent))
    {
        isScrollDataValid = false;
        m_pScrollData->Extent = extent;
    }

    if(!CoerceOffsets())
    {
        isScrollDataValid = false;
    }

    if(!isScrollDataValid)
    {
        get_ScrollOwner()->InvalidateScrollInfo();
    }

}

Rect ScrollContentPresenter::MakeVisible(Visual* pVisual, const Rect& rect)
{
    if(pVisual == NULL || !IsAncestorOf(pVisual))
    {
        return Rect();
    }

    if(pVisual == this)
    {
        return rect;
    }

    Rect transformedRect = pVisual->TransformToAncestor(this).TransformBounds(rect);

    if(get_IsScrollClient())
    {
        Rect rect1(get_HorizontalOffset(), get_VerticalOffset(), get_ViewportWidth(), get_ViewportHeight());

        transformedRect.set_X(transformedRect.get_X() + rect1.get_X());
        transformedRect.set_Y(transformedRect.get_Y() + rect1.get_Y());

        float horizOffset = ComputeScrollOffsetWithMinimalScroll(rect1.get_Left(), rect1.get_Right(), transformedRect.get_Left(), transformedRect.get_Right());
        float vertOffset =  ComputeScrollOffsetWithMinimalScroll(rect1.get_Top(), rect1.get_Bottom(), transformedRect.get_Top(), transformedRect.get_Bottom());

        set_HorizontalOffset(horizOffset);
        set_VerticalOffset(vertOffset);

        rect1.set_X(horizOffset);
        rect1.set_Y(vertOffset);

        if(Rect::Intersect(transformedRect, rect1, &transformedRect))
        {
            transformedRect.set_X(transformedRect.get_X() - rect1.get_X());
            transformedRect.set_Y(transformedRect.get_Y() - rect1.get_Y());
        }
    }

    return transformedRect;
}


Size ScrollContentPresenter::ArrangeOverride(const Size& constraintSize)
{
    if(get_IsScrollClient())
    {
        VerifyScrollData(constraintSize, m_pScrollData->Extent);
    }

    if(m_pContentCache != NULL)
    {
        Rect arrangeRect(m_pContentCache->get_DesiredSize());
        if(get_IsScrollClient())
        {
            arrangeRect.set_X(-get_HorizontalOffset());
            arrangeRect.set_Y(-get_VerticalOffset());
        }
        arrangeRect.set_Width(Float::Max(arrangeRect.get_Width(), constraintSize.get_Width()));
        arrangeRect.set_Height(Float::Max(arrangeRect.get_Height(), constraintSize.get_Height()));
        m_pContentCache->Arrange(arrangeRect);
    }

    return constraintSize;
}

float ScrollContentPresenter::ValidateInputOffset(float offset)
{
   if (Float::IsNaN(offset))
    {
       LibraryCriticalError();
    }
    return Float::Max(0, offset);
}


void ScrollContentPresenter::HookupScrollingComponents()
{
    ScrollViewer* pScrollViewer = object_cast<ScrollViewer>(get_TemplatedParent());
    if (pScrollViewer != NULL)
    {
        IScrollInfo* pScrollInfo = NULL;
        if (get_CanContentScroll())
        {
            pScrollInfo = interface_cast<IScrollInfo>(get_Content());
            if(pScrollInfo == NULL)
            {
                ItemsPresenter* pItemsPresenter = object_cast<ItemsPresenter>(get_Content());
                if(pItemsPresenter != NULL)
                {
                    pItemsPresenter->ApplyTemplate();
                    if(pItemsPresenter->get_VisualChildCount() > 0)
                    {
                        pScrollInfo = interface_cast<IScrollInfo>(pItemsPresenter->get_VisualChild(0));
                    }
                }
            }
        }
        if (pScrollInfo == NULL)
        {
            pScrollInfo = this;
            EnsureScrollData();
        }
        if (pScrollInfo != m_pScrollInfo && m_pScrollInfo != NULL)
        {
            if (get_IsScrollClient())
            {
                ClearScrollData();
            }
            else
            {
                m_pScrollInfo->set_ScrollOwner(NULL);
            }
        }
        if (pScrollInfo != NULL)
        {
            m_pScrollInfo = pScrollInfo;
            m_pScrollInfo->set_ScrollOwner(pScrollViewer);
            pScrollViewer->set_ScrollInfo(pScrollInfo);
            pScrollViewer->set_ScrollVisual(this);
        }
    }
    else if (m_pScrollInfo != NULL)
    {
        if (m_pScrollInfo->get_ScrollOwner() != NULL)
        {
            m_pScrollInfo->get_ScrollOwner()->set_ScrollInfo(NULL);
        }
        m_pScrollInfo->set_ScrollOwner(NULL);
        m_pScrollInfo = NULL;

        ClearScrollData();
    }
}

void ScrollContentPresenter::ClearScrollData()
{
    if(m_pScrollData != NULL)
    {
        delete m_pScrollData;
        m_pScrollData = NULL;
    }
}

ScrollContentPresenter::ScrollData* ScrollContentPresenter::EnsureScrollData()
{
    if(m_pScrollData == NULL)
    {
        m_pScrollData = new ScrollData();
    }
    return m_pScrollData;
}




bool ScrollContentPresenter::get_CanHorizontallyScroll()
{
    if(!get_IsScrollClient()) 
        return false;

    return EnsureScrollData()->CanHorizontallyScroll;
}
void ScrollContentPresenter::set_CanHorizontallyScroll(bool value)
{
    if(get_IsScrollClient() && EnsureScrollData()->CanHorizontallyScroll != value)
    {
        EnsureScrollData()->CanHorizontallyScroll = value;
        InvalidateMeasure();
    }
}

bool ScrollContentPresenter::get_CanVerticallyScroll()
{
    if(!get_IsScrollClient()) 
        return false;

    return EnsureScrollData()->CanVerticallyScroll;
}

void ScrollContentPresenter::set_CanVerticallyScroll(bool value)
{
    if(get_IsScrollClient() && EnsureScrollData()->CanVerticallyScroll != value)
    {
        EnsureScrollData()->CanVerticallyScroll = value;
        InvalidateMeasure();
    }
}

bool ScrollContentPresenter::GetLayoutClip(const Size& layoutSlotSize, Rect* pRect)
{
    *pRect = Rect(get_RenderSize());
    return true;
}



void ScrollContentPresenter::LineUp() { if(get_IsScrollClient()) set_VerticalOffset(get_VerticalOffset() - 16); };
void ScrollContentPresenter::LineDown() { if(get_IsScrollClient()) set_VerticalOffset(get_VerticalOffset() + 16); };
void ScrollContentPresenter::LineLeft() { if(get_IsScrollClient()) set_HorizontalOffset(get_HorizontalOffset() - 16); };
void ScrollContentPresenter::LineRight() { if(get_IsScrollClient()) set_HorizontalOffset(get_HorizontalOffset() + 16); };
void ScrollContentPresenter::PageUp() { if(get_IsScrollClient()) set_VerticalOffset(get_VerticalOffset() - get_ViewportHeight()); };
void ScrollContentPresenter::PageDown() { if(get_IsScrollClient()) set_VerticalOffset(get_VerticalOffset() + get_ViewportHeight()); };
void ScrollContentPresenter::PageLeft() { if(get_IsScrollClient()) set_HorizontalOffset(get_HorizontalOffset() - get_ViewportWidth()); };
void ScrollContentPresenter::PageRight() { if(get_IsScrollClient()) set_HorizontalOffset(get_HorizontalOffset() + get_ViewportWidth()); };
void ScrollContentPresenter::MouseWheelUp() { if(get_IsScrollClient()) set_VerticalOffset(get_VerticalOffset() - 48); };
void ScrollContentPresenter::MouseWheelDown() { if(get_IsScrollClient()) set_VerticalOffset(get_VerticalOffset() + 48); };
void ScrollContentPresenter::MouseWheelLeft() { if(get_IsScrollClient()) set_HorizontalOffset(get_HorizontalOffset() - 48); };
void ScrollContentPresenter::MouseWheelRight() { if(get_IsScrollClient()) set_HorizontalOffset(get_HorizontalOffset() - 48); };
float ScrollContentPresenter::get_ExtentHeight() { return get_IsScrollClient() ? EnsureScrollData()->Extent.get_Height() : 0.0f; }
float ScrollContentPresenter::get_ExtentWidth() { return get_IsScrollClient() ? EnsureScrollData()->Extent.get_Width() : 0.0f; }
float ScrollContentPresenter::get_HorizontalOffset() { return get_IsScrollClient() ? EnsureScrollData()->ComputedOffset.get_X() : 0.0f; }

void ScrollContentPresenter::set_HorizontalOffset(float horizontalOffset) 
{ 
    if(get_IsScrollClient())
    {
        horizontalOffset = Float::Max(horizontalOffset, 0);
        if(!Float::AreClose(EnsureScrollData()->Offset.get_X(), horizontalOffset))
        {
            m_pScrollData->Offset.set_X(horizontalOffset);
            InvalidateArrange();
        }
    }
}                                               

float ScrollContentPresenter::get_VerticalOffset() { return get_IsScrollClient() ? EnsureScrollData()->ComputedOffset.get_Y() : 0.0f; }
void ScrollContentPresenter::set_VerticalOffset(float verticalOffset) 
{ 
    if(get_IsScrollClient())
    {
        verticalOffset = Float::Max(verticalOffset, 0);
        if(!Float::AreClose(EnsureScrollData()->Offset.get_Y(), verticalOffset))
        {
            m_pScrollData->Offset.set_Y(verticalOffset);
            InvalidateArrange();
        }
    }
}


float ScrollContentPresenter::get_ViewportWidth() { return get_IsScrollClient() ? EnsureScrollData()->Viewport.get_Width() : 0.0f; };
float ScrollContentPresenter::get_ViewportHeight() { return get_IsScrollClient() ? EnsureScrollData()->Viewport.get_Height() : 0.0f; }

void ScrollContentPresenter::set_ScrollOwner(ScrollViewer* pScrollViewer) { if(get_IsScrollClient()) EnsureScrollData()->pScrollOwner = pScrollViewer; }
ScrollViewer* ScrollContentPresenter::get_ScrollOwner() { return get_IsScrollClient() ? EnsureScrollData()->pScrollOwner : NULL; }


}; // namespace AVUI
