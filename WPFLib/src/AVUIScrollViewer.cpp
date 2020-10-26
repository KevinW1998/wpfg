#include <AVUICommon.h>
#include <AVUIScrollViewer.h>
#include <AVUIScrollBar.h>
#include <AVUIScrollContentPresenter.h>
#include <AVUIControlTemplate.h>
#include <AVUIDispatcher.h>
#include <AVUILayoutManager.h>
#include <AVUIRequestBringIntoViewEventArgs.h>
#include <AVUIKeyboardNavigation.h>
#include <AVUICommandManager.h>
#include <AVUICommandBinding.h>
#include <AVUIBinding.h>
#include <AVUIRelativeSource.h>
#include <AVUIPropertyPath.h>

REGISTER_ELEMENT_BEGIN(AVUI::ScrollViewer)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Content)
    REGISTER_PROPERTY(HorizontalScrollBarVisibility)
    REGISTER_PROPERTY(VerticalScrollBarVisibility)
    REGISTER_PROPERTY(ComputedHorizontalScrollBarVisibility)
    REGISTER_PROPERTY(ComputedVerticalScrollBarVisibility)
    REGISTER_PROPERTY(CanContentScroll)

    REGISTER_PROPERTY(VerticalOffset)
    REGISTER_PROPERTY(HorizontalOffset)
    REGISTER_PROPERTY(ExtentWidth)
    REGISTER_PROPERTY(ExtentHeight)
    REGISTER_PROPERTY(ScrollableWidth)
    REGISTER_PROPERTY(ScrollableHeight)
    REGISTER_PROPERTY(ViewportWidth)
    REGISTER_PROPERTY(ViewportHeight)

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor);

REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::ScrollChangedEventArgs)


namespace AVUI {

ScrollViewer::ScrollViewer() : m_isInMeasure(false), m_scrollVisibilityX(Visibility::Visible), m_scrollVisibilityY(Visibility::Visible),
                               m_xExtent(0.0f), m_yExtent(0.0f), m_xPositionISI(0.0f), m_yPositionISI(0.0f), m_xSize(0.0f), m_ySize(0.0f),
                               m_isRegisteredForLayoutUpdated(false), m_inChildInvalidateMeasure(false)
{
}

ScrollViewer::~ScrollViewer() 
{ 
    ClearLayoutUpdatedHandler(); 
    m_pScrollVisual = NULL;
};


void ScrollViewer::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(ScrollViewer), get_RequestBringIntoViewEvent(), RequestBringIntoViewEventHandler::Create(&ScrollViewer::OnRequestBringIntoViewThunk), false);
    CommandManager::RegisterClassHandler(GET_TYPE(ScrollViewer), object_allocate<CommandBinding>(ScrollBar::get_PageUpCommand(), ExecutedRoutedEventHandler::Create(&ScrollViewer::OnScrollCommand)));
    CommandManager::RegisterClassHandler(GET_TYPE(ScrollViewer), object_allocate<CommandBinding>(ScrollBar::get_PageDownCommand(), ExecutedRoutedEventHandler::Create(&ScrollViewer::OnScrollCommand)));
}


void ScrollViewer::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    ControlTemplate* pTemplate = get_Template();
    if(pTemplate != NULL)
    {
        ScrollContentPresenter* pPresenter = object_cast<ScrollContentPresenter>(pTemplate->FindName(String(L"PART_ScrollContentPresenter"),  this));

        if(pPresenter != NULL)
        {
            pPresenter->HookupScrollingComponents();
        }

        ScrollBar* pHorizSB = object_cast<ScrollBar>(pTemplate->FindName(String(L"PART_HorizontalScrollBar"),  this));
        if(pHorizSB != NULL)
        {
            pHorizSB->set_IsStandalone(false);
        }

        ScrollBar* pVertSB = object_cast<ScrollBar>(pTemplate->FindName(String(L"PART_VerticalScrollBar"),  this));
        if(pVertSB != NULL)
        {
            pVertSB->set_IsStandalone(false);
        }
    }
}

void ScrollViewer::set_ScrollInfo(IScrollInfo* pScrollInfo)
{
    m_pScrollInfo = pScrollInfo;
    if(pScrollInfo != NULL)
    {
        pScrollInfo->set_CanHorizontallyScroll(get_HorizontalScrollBarVisibility() != ScrollBarVisibility::Disabled);
        pScrollInfo->set_CanVerticallyScroll(get_VerticalScrollBarVisibility() != ScrollBarVisibility::Disabled);
    } 
}


void ScrollViewer::InvalidateScrollInfo()
{
    if(!m_isInMeasure)
    {
        float extentWidth = get_ScrollInfo()->get_ExtentWidth();
        float viewportWidth = get_ScrollInfo()->get_ViewportWidth();
        if(get_HorizontalScrollBarVisibility() == ScrollBarVisibility::Auto && 
           ((m_scrollVisibilityX == Visibility::Collapsed && Float::GreaterThan(extentWidth, viewportWidth)) ||
            (m_scrollVisibilityX == Visibility::Visible && Float::LessThanOrClose(extentWidth, viewportWidth))
            )
           )
        {
            InvalidateMeasure();
        }
        else
        {
            float extentHeight = get_ScrollInfo()->get_ExtentHeight();
            float viewportHeight = get_ScrollInfo()->get_ViewportHeight();

            if(get_VerticalScrollBarVisibility() == ScrollBarVisibility::Auto && 
               ((m_scrollVisibilityY == Visibility::Collapsed && Float::GreaterThan(extentHeight, viewportHeight)) ||
                (m_scrollVisibilityY == Visibility::Visible && Float::LessThanOrClose(extentHeight, viewportHeight))
                )
               )
            {
                InvalidateMeasure();
            }
        }
    }

    if(!Float::AreClose(get_HorizontalOffset(), get_ScrollInfo()->get_HorizontalOffset()) ||
       !Float::AreClose(get_VerticalOffset(), get_ScrollInfo()->get_VerticalOffset()) ||
       !Float::AreClose(get_ViewportWidth(), get_ScrollInfo()->get_ViewportWidth()) ||
       !Float::AreClose(get_ViewportHeight(), get_ScrollInfo()->get_ViewportHeight()) ||
       !Float::AreClose(get_ExtentWidth(), get_ScrollInfo()->get_ExtentWidth()) ||
       !Float::AreClose(get_ExtentHeight(), get_ScrollInfo()->get_ExtentHeight()))

    {
        EnsureLayoutUpdatedHandler();
    }
}

Size ScrollViewer::MeasureOverride(const Size& constraintSize)
{
    set_InChildInvalidateMeasure(false);

    UIElement* pChild = (get_VisualChildCount() > 0) ? object_cast<UIElement>(get_VisualChild(0)) : NULL;
    if(pChild == NULL)
    {
        return Size();
    }
    IScrollInfo* pScrollInfo = get_ScrollInfo();
    ScrollBarVisibility::Enum verticalScrollBarVisibility = get_VerticalScrollBarVisibility();
    ScrollBarVisibility::Enum horizontalScrollBarVisibility = get_HorizontalScrollBarVisibility();


    bool isVerticalScrollBarAuto = verticalScrollBarVisibility == ScrollBarVisibility::Auto;
    bool isHorizontalScrollBarAuto = horizontalScrollBarVisibility == ScrollBarVisibility::Auto;
    bool isVerticalScrollBarDisabled = verticalScrollBarVisibility == ScrollBarVisibility::Disabled;
    bool isHorizontalScrollBarDisabled = horizontalScrollBarVisibility == ScrollBarVisibility::Disabled;
    Visibility::Enum verticalComputedVisibility = (verticalScrollBarVisibility == ScrollBarVisibility::Visible) ? Visibility::Visible : Visibility::Collapsed;
    Visibility::Enum horizontalComputedVisibility = (horizontalScrollBarVisibility == ScrollBarVisibility::Visible) ? Visibility::Visible : Visibility::Collapsed;

    if (m_scrollVisibilityY != verticalComputedVisibility)
    {
        m_scrollVisibilityY = verticalComputedVisibility;
        set_ComputedVerticalScrollBarVisibility(m_scrollVisibilityY);
    }
    if (m_scrollVisibilityX != horizontalComputedVisibility)
    {
        m_scrollVisibilityX = horizontalComputedVisibility;
        set_ComputedHorizontalScrollBarVisibility(m_scrollVisibilityX);
    }

    float horizontalOffset = 0;
    float verticalOffset = 0;
    if (pScrollInfo != NULL)
    {
        pScrollInfo->set_CanHorizontallyScroll(!isHorizontalScrollBarDisabled);
        pScrollInfo->set_CanVerticallyScroll(!isVerticalScrollBarDisabled);
    }

    pChild->Measure(constraintSize);

    pScrollInfo = get_ScrollInfo();

    if (pScrollInfo != NULL && (isHorizontalScrollBarAuto || isVerticalScrollBarAuto))
    {
        bool needsHorizontalScrollBar = isHorizontalScrollBarAuto && Float::GreaterThan(pScrollInfo->get_ExtentWidth(), pScrollInfo->get_ViewportWidth());
        bool needsVerticalScrollBar = isVerticalScrollBarAuto && Float::GreaterThan(pScrollInfo->get_ExtentHeight(), pScrollInfo->get_ViewportHeight());
        if (needsHorizontalScrollBar && m_scrollVisibilityX != Visibility::Visible)
        {
            m_scrollVisibilityX = Visibility::Visible;
            set_ComputedHorizontalScrollBarVisibility(m_scrollVisibilityX);
        }
        if (needsVerticalScrollBar && m_scrollVisibilityY != Visibility::Visible)
        {
            m_scrollVisibilityY = Visibility::Visible;
            set_ComputedVerticalScrollBarVisibility(m_scrollVisibilityY);
        }
        if(needsHorizontalScrollBar || needsVerticalScrollBar)
        {
            set_InChildInvalidateMeasure(true);
            pChild->InvalidateMeasure();
            pChild->Measure(constraintSize);
        }

        if(isVerticalScrollBarAuto && isHorizontalScrollBarAuto && (needsHorizontalScrollBar != needsVerticalScrollBar))
        {
            bool reallyNeedsHorizontalScrollbar = !needsHorizontalScrollBar && Float::GreaterThan(pScrollInfo->get_ExtentWidth(), pScrollInfo->get_ViewportWidth());
            bool reallyNeedsVerticalScrollbar = !needsVerticalScrollBar && Float::GreaterThan(pScrollInfo->get_ExtentHeight(), pScrollInfo->get_ViewportHeight());

            if(reallyNeedsHorizontalScrollbar)
            {
                if(m_scrollVisibilityX != Visibility::Visible)
                {
                    m_scrollVisibilityX = Visibility::Visible;
                    set_ComputedHorizontalScrollBarVisibility(m_scrollVisibilityX);
                }
            }
            else if(reallyNeedsVerticalScrollbar && m_scrollVisibilityY != Visibility::Visible)
            {
                m_scrollVisibilityY = Visibility::Visible;
                set_ComputedVerticalScrollBarVisibility(m_scrollVisibilityY);
            }
            
            if (reallyNeedsHorizontalScrollbar || reallyNeedsVerticalScrollbar)
            {
                set_InChildInvalidateMeasure(true);
                pChild->InvalidateMeasure();
                pChild->Measure(constraintSize);
            }
        }
        
    }

    m_isInMeasure = false;

    return pChild->get_DesiredSize();
}

void ScrollViewer::MakeVisible(Visual* pVisual, const Rect& rect)
{
    MakeVisibleParams mvp(pVisual, rect);
    EnqueueCommand(Commands::MakeVisible, 0, mvp);
}



void ScrollViewer::EnqueueCommand(Commands::Enum command, float param, const MakeVisibleParams& makeVisibleParams)
{
    m_commandQueue.AddItem(EnqueuedCommand(command, param, makeVisibleParams));
    EnsureQueueProcessing();
}

void ScrollViewer::EnsureQueueProcessing()
{
    if(m_commandQueue.get_Count() > 0)
    {
        EnsureLayoutUpdatedHandler();
    }
}


void ScrollViewer::OnLayoutUpdated(Object* pSender, EventArgs* pArgs)
{
    if (ExecuteNextCommand())
    {
        InvalidateArrange();
    }
    else
    {
        float oldHorizontalOffset = m_xPositionISI;
        float oldVerticalOffset = m_yPositionISI;
        float oldViewportWidth = m_xSize;
        float oldViewportHeight = m_ySize;
        float oldExtentWidth = m_xExtent;
        float oldExtentHeight = m_yExtent;
        float oldScrollableWidth = ComputeScrollableWidth();
        float oldScrollableHeight = ComputeScrollableHeight();

        bool isChanged = false;

        if(m_pScrollInfo != NULL)
        {
            if (!Float::AreClose(oldHorizontalOffset, m_pScrollInfo->get_HorizontalOffset()))
            {
                m_xPositionISI = m_pScrollInfo->get_HorizontalOffset();
                set_HorizontalOffset(m_xPositionISI);
                isChanged = true;
            }
            if (!Float::AreClose(oldVerticalOffset, m_pScrollInfo->get_VerticalOffset()))
            {
                m_yPositionISI = m_pScrollInfo->get_VerticalOffset();
                set_VerticalOffset(m_yPositionISI);
                isChanged = true;
            }
            if (!Float::AreClose(oldViewportWidth, m_pScrollInfo->get_ViewportWidth()))
            {
                m_xSize = m_pScrollInfo->get_ViewportWidth();
                set_ViewportWidth(m_xSize);
                isChanged = true;
            }
            if (!Float::AreClose(oldViewportHeight, m_pScrollInfo->get_ViewportHeight()))
            {
                m_ySize = m_pScrollInfo->get_ViewportHeight();
                set_ViewportHeight(m_ySize);
                isChanged = true;
            }
            if (!Float::AreClose(oldExtentWidth, m_pScrollInfo->get_ExtentWidth()))
            {
                m_xExtent = m_pScrollInfo->get_ExtentWidth();
                set_ExtentWidth(m_xExtent);
                isChanged = true;
            }
            if (!Float::AreClose(oldExtentHeight, m_pScrollInfo->get_ExtentHeight()))
            {
                m_yExtent = m_pScrollInfo->get_ExtentHeight();
                set_ExtentHeight(m_yExtent);
                isChanged = true;
            }
        }

        if (!Float::AreClose(oldScrollableWidth, ComputeScrollableWidth()))
        {
            set_ScrollableWidth(ComputeScrollableWidth());
            isChanged = true;
        }

        if (!Float::AreClose(oldScrollableHeight, ComputeScrollableHeight()))
        {
            set_ScrollableHeight(ComputeScrollableHeight());
            isChanged = true;
        }

        if (isChanged)
        {
            ScrollChangedEventArgs eventArgs(Vector(m_xPositionISI, m_yPositionISI), Vector(m_xPositionISI - oldHorizontalOffset, m_yPositionISI - oldVerticalOffset), Size(m_xExtent, m_yExtent), Vector(m_xExtent - oldExtentWidth, m_yExtent - oldExtentHeight), Size(m_xSize, m_ySize), Vector(m_xSize - oldViewportWidth, m_ySize - oldViewportHeight));

            RaiseEvent(&eventArgs);
        }

        ClearLayoutUpdatedHandler();
    }
}


void ScrollViewer::ClearLayoutUpdatedHandler()
{
    if(m_isRegisteredForLayoutUpdated)
    {
        Dispatcher::get_CurrentDispatcher()->get_LayoutManager()->remove_LayoutUpdated(EventHandler::Create(this, &ScrollViewer::OnLayoutUpdated));
        m_isRegisteredForLayoutUpdated = false;
    }
}

bool ScrollViewer::ExecuteNextCommand()
{
    if(m_pScrollInfo == NULL || m_commandQueue.get_Count() == 0)
    {
        return false;
    }

    EnqueuedCommand cmd = m_commandQueue[0];
    m_commandQueue.RemoveAt(0);

    if(cmd.Command == Commands::MakeVisible)
    {
        Visual* pVisual1 = cmd.MakeVisibleParamsCommand.Child;
        Visual* pVisual2 = m_pScrollVisual;

        if(pVisual1 != NULL && pVisual2 != NULL && pVisual2->IsAncestorOf(pVisual1))
        {
            Rect rect = cmd.MakeVisibleParamsCommand.TargetRect;
    
            if(!cmd.MakeVisibleParamsCommand.HasTargetRect)
            {
                UIElement* pUIE = object_cast<UIElement>(pVisual1);
                if(pUIE != NULL)
                {
                    rect = Rect(pUIE->get_RenderSize());
                }
                else
                {
                    rect = Rect();
                }
            }
    
            Rect rect2 = m_pScrollInfo->MakeVisible(pVisual1, rect);
    
            rect2 = pVisual2->TransformToAncestor(this).TransformBounds(rect2);
    
            BringIntoView(rect2);
        }
    }

    return true;
}

void ScrollViewer::EnsureLayoutUpdatedHandler()
{
    if(!m_isRegisteredForLayoutUpdated)
    {
        m_isRegisteredForLayoutUpdated = true;
        Dispatcher::get_CurrentDispatcher()->get_LayoutManager()->add_LayoutUpdated(EventHandler::Create(this, &ScrollViewer::OnLayoutUpdated));

        InvalidateArrange();
    }
}

void ScrollViewer::OnRequestBringIntoViewThunk(Object* pSender, RequestBringIntoViewEventArgs* pArgs)
{
    ScrollViewer* pScrollViewer = (ScrollViewer*) pSender;
    Visual* pVisual = pArgs->get_Target();
    if(pVisual != NULL && pVisual != pScrollViewer)
    {
        pArgs->set_IsHandled(true);

        if(pArgs->get_HasTargetRect())
        {
            pScrollViewer->EnqueueCommand(Commands::MakeVisible, 0, MakeVisibleParams(pVisual, pArgs->get_TargetRect())); 
        }
        else
        {
            pScrollViewer->EnqueueCommand(Commands::MakeVisible, 0, MakeVisibleParams(pVisual)); 
        }
    }
}

void ScrollViewer::OnScrollCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    ScrollViewer* pViewer = (ScrollViewer*) pSender;
    if(pArgs->get_Command() == ScrollBar::get_PageDownCommand())
    {
        pViewer->m_pScrollInfo->set_VerticalOffset(pViewer->m_pScrollInfo->get_VerticalOffset() + 20);
    }
    else if(pArgs->get_Command() == ScrollBar::get_PageUpCommand())
    {
        pViewer->m_pScrollInfo->set_VerticalOffset(pViewer->m_pScrollInfo->get_VerticalOffset() - 20);
    }
}

void ScrollViewer::ScrollToHorizontalOffset(float offset)
{
    // TODO -- fix scroll viewer offset / command handling
    LibraryCriticalError();
//    EnqueueCommand(Commands::SetHorizontalOffset, offset, NULL);
}

void ScrollViewer::ScrollToVerticalOffset(float offset)
{
    // TODO -- fix scroll viewer offset / command handling
    LibraryCriticalError();
//    EnqueueCommand(Commands::SetVerticalOffset, offset, NULL);
}

void ScrollViewer::PageUp()
{
// TODO -- fix scroll viewer offset / command handling
    LibraryCriticalError();
//    EnqueueCommand(Commands::PageUp);

}

void ScrollViewer::PageDown()
{
// TODO -- fix scroll viewer offset / command handling
    LibraryCriticalError();
//    EnqueueCommand(Commands::PageDown);

}

void ScrollViewer::PageLeft()
{
// TODO -- fix scroll viewer offset / command handling
    LibraryCriticalError();
//    EnqueueCommand(Commands::PageLeft);

}

void ScrollViewer::PageRight()
{
// TODO -- fix scroll viewer offset / command handling
    LibraryCriticalError();
//    EnqueueCommand(Commands::PageRight);

}

void ScrollViewer::OnPreApplyTemplate()
{
    Control::OnPreApplyTemplate();
    if(get_TemplatedParent() != NULL)
    {
        BindToTemplatedParent(get_HorizontalScrollBarVisibilityProperty());
        BindToTemplatedParent(get_VerticalScrollBarVisibilityProperty());
        BindToTemplatedParent(get_CanContentScrollProperty());
    }
}

void ScrollViewer::BindToTemplatedParent(const DependencyProperty& property)
{
    if (!HasNonDefaultValue(property))
    {
        TRefCountedPtr<Binding> pBinding = object_allocate<Binding>();

        pBinding->set_RelativeSource(RelativeSource::get_TemplatedParent());
        pBinding->set_Path(object_allocate<PropertyPath>((Object*) &property));
        SetBinding(property, pBinding);
    }
}

 

 



}; // namespace AVUI
