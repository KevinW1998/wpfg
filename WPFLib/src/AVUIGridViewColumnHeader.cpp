#include <AVUICommon.h>
#include <AVUIGridViewColumnHeader.h>
#include <AVUIGridViewColumn.h>
#include <AVUIGridViewHeaderRowPresenter.h>
#include <AVUIThumb.h>
#include <AVUICursors.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridViewColumnHeader)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI
{



void GridViewColumnHeader::UpdateProperty(const DependencyProperty* pDP, Object* pValue)
{
    Flags::Enum ignoreFlag = Flags::None;
    if (!get_IsInternalGenerated())
    {
        Flags::Enum flag;
        PropertyToFlags(pDP, &flag, &ignoreFlag);
        if (GetFlag(flag))
        {
            return;
        }
        SetFlag(ignoreFlag, true);
    }
    if (pValue != NULL)
    {
        SetValue(*pDP, pValue);
    }
    else
    {
        ClearValue(*pDP);
    }
    SetFlag(ignoreFlag, false);
}

 
void GridViewColumnHeader::PropertyToFlags(const DependencyProperty* pDP, Flags::Enum *pFlag, Flags::Enum* pIgnoreFlag)
{
   if (pDP == &UIElement::get_StyleProperty())
    {
        *pFlag = Flags::StyleSetByUser;
        *pIgnoreFlag = Flags::IgnoreStyle;
    }
    else if (pDP == &ContentControl::get_ContentTemplateProperty())
    {
        *pFlag = Flags::ContentTemplateSetByUser;
        *pIgnoreFlag = Flags::IgnoreContentTemplate;
    }
    else if (pDP == &ContentControl::get_ContentTemplateSelectorProperty())
    {
        *pFlag = Flags::ContentTemplateSelectorSetByUser;
        *pIgnoreFlag = Flags::IgnoreContentTemplateSelector;
    }
    else if (pDP == &UIElement::get_ContextMenuProperty())
    {
        *pFlag = Flags::ContextMenuSetByUser;
        *pIgnoreFlag = Flags::IgnoreContextMenu;
    }
    else if (pDP == &UIElement::get_ToolTipProperty())
    {
        *pFlag = Flags::ToolTipSetByUser;
        *pIgnoreFlag = Flags::IgnoreToolTip;
    }
    else
    {
        *pFlag = *pIgnoreFlag = Flags::None;
    }
}

void GridViewColumnHeader::OnColumnHeaderKeyDown(Object* pSender, KeyEventArgs* pArgs)
{
    // TODO -- key handling
    /*
    if (((e.Key == Key.Escape) && (this._headerGripper != null)) && this._headerGripper.IsDragging)
    {
        this._headerGripper.CancelDrag();
        e.Handled = true;
    }
    */
}

 
void GridViewColumnHeader::CheckWidthForPreviousHeaderGripper()
{
    bool hide = false;
    if (m_pHeaderGripper != NULL)
    {
        hide = Float::LessThan(get_RenderSize().get_Width(), m_pHeaderGripper->get_Width());
    }
    if (m_pPreviousHeader != NULL)
    {
        m_pPreviousHeader->HideGripperRightHalf(hide);
    }
    UpdateGripperCursor();
}

 
void GridViewColumnHeader::HideGripperRightHalf(bool hide)
{
    if (m_pHeaderGripper != NULL)
    {
        UIElement* pElement = object_cast<UIElement>(m_pHeaderGripper->get_VisualParent());
        if (pElement != NULL)
        {
            pElement->set_ClipToBounds(hide);
        }
    }
}

void GridViewColumnHeader::UpdateGripperCursor()
{
    // TODO -- Cursor
    /*
    if ((this._headerGripper != null) && !this._headerGripper.IsDragging)
    {
        Cursor splitOpenCursor;
        if (DoubleUtil.IsZero(base.ActualWidth))
        {
            splitOpenCursor = this.SplitOpenCursor;
        }
        else
        {
            splitOpenCursor = this.SplitCursor;
        }
        if (splitOpenCursor != null)
        {
            this._headerGripper.Cursor = splitOpenCursor;
        }
    }
    */
}

 
void GridViewColumnHeader::OnApplyTemplate()
{
    ButtonBase::OnApplyTemplate();
    switch (get_Role())
    {
    case GridViewColumnHeaderRole::Normal:
        HookupGripperEvents();
        break;

    case GridViewColumnHeaderRole::Floating:
//        m_pFloatingHeaderCanvas = object_cast<Canvas>(GetTemplateChild(L"PART_FloatingHeaderCanvas"));
//        UpdateFloatingHeaderCanvas();
        break;
    }
}

 
void GridViewColumnHeader::UpdateFloatingHeaderCanvas()
{
    /*
    if (m_pFloatingHeaderCanvas != NULL && get_FloatSourceHeader() != NULL)
    {
        Vector offset = VisualTreeHelper::GetOffset(get_FloatSourceHeader());
        TRefCountedPtr<VisualBrush> pBrush = object_allocate<VisualBrush>(get_FloatSourceHeader());
        brush.ViewboxUnits = BrushMappingMode.Absolute;
        brush.Viewbox = new Rect(offset.X, offset.Y, this.FloatSourceHeader.ActualWidth, this.FloatSourceHeader.ActualHeight);
        this._floatingHeaderCanvas.Background = brush;
        this.FloatSourceHeader = null;
    }
    */
}


void GridViewColumnHeader::HookupGripperEvents()
{
    UnhookGripperEvents();
    m_pHeaderGripper = object_cast<Thumb>(GetTemplateChild(L"PART_HeaderGripper"));
    if (m_pHeaderGripper != NULL)
    {
        m_pHeaderGripper->add_DragStarted(DragStartedEventHandler::Create(this, &GridViewColumnHeader::OnColumnHeaderGripperDragStarted));
        m_pHeaderGripper->add_DragDelta(DragDeltaEventHandler::Create(this, &GridViewColumnHeader::OnColumnHeaderResize));
        m_pHeaderGripper->add_DragCompleted(DragCompletedEventHandler::Create(this, &GridViewColumnHeader::OnColumnHeaderGripperDragCompleted));
        m_pHeaderGripper->add_MouseDoubleClick(MouseButtonEventHandler::Create(this, &GridViewColumnHeader::OnGripperDoubleClicked));
        m_pHeaderGripper->add_MouseEnter(MouseEventHandler::Create(this, &GridViewColumnHeader::OnGripperMouseEnterLeave));
        m_pHeaderGripper->add_MouseLeave(MouseEventHandler::Create(this, &GridViewColumnHeader::OnGripperMouseEnterLeave));
        m_pHeaderGripper->set_Cursor(get_SplitCursor());
    }
}

 
void GridViewColumnHeader::OnColumnHeaderGripperDragStarted(Object* pSender, DragStartedEventArgs* pArgs)
{
    MakeParentGotFocus();
    m_originalWidth = get_ColumnActualWidth();
    pArgs->set_IsHandled(true);
}

void GridViewColumnHeader::OnColumnHeaderResize(Object* pSender, DragDeltaEventArgs* pArgs)
{
    float num = get_ColumnActualWidth() + pArgs->get_HorizontalChange();
    if (Float::LessThanOrClose(num, 0.0f))
    {
        num = 0.0f;
    }
    UpdateColumnHeaderWidth(num);
    pArgs->set_IsHandled(true);
}

void GridViewColumnHeader::OnColumnHeaderGripperDragCompleted(Object* pSender, DragCompletedEventArgs* pArgs)
{
    if (pArgs->get_Canceled())
    {
        UpdateColumnHeaderWidth(m_originalWidth);
    }
    UpdateGripperCursor();
    pArgs->set_IsHandled(true);
}

void GridViewColumnHeader::OnGripperMouseEnterLeave(Object* pSender, MouseEventArgs* pArgs)
{
    HandleIsMouseOverChanged();
}
 

void GridViewColumnHeader::OnGripperDoubleClicked(Object* pSender, MouseButtonEventArgs* pArgs)
{
    if (get_Column() != NULL)
    {
        if (Float::IsNaN(get_Column()->get_Width()))
        {
            get_Column()->set_Width(get_Column()->get_ActualWidth());
        }
        get_Column()->set_Width(Float::get_NaN());
        pArgs->set_IsHandled(true);
    }
}
 
float GridViewColumnHeader::get_ColumnActualWidth()
{
    if (get_Column() == NULL)
    {
        return 0.0f;
        //return get_ActualWidth();
    }

    return get_Column()->get_ActualWidth();
}
 

 
bool GridViewColumnHeader::HandleIsMouseOverChanged()
{
    if (get_ClickMode() != ClickMode::Hover)
    {
        return false;
    }
    if (get_IsMouseOver() && (m_pHeaderGripper == NULL || !m_pHeaderGripper->get_IsMouseOver()))
    {
        set_IsPressed(true);
        OnClick();
    }
    else
    {
        ClearValue(ButtonBase::get_IsPressedProperty());
    }

    return true;
}

void GridViewColumnHeader::MakeParentGotFocus()
{
    GridViewHeaderRowPresenter* pParent = object_cast<GridViewHeaderRowPresenter>(get_Parent());
    if (pParent != NULL)
    {
        pParent->MakeParentItemsControlGotFocus();
    }
}

 
void GridViewColumnHeader::UpdateColumnHeaderWidth(float width)
{
    if (get_Column() != NULL)
    {
        get_Column()->set_Width(width);
    }
    else
    {
        set_Width(width);
    }
}

void GridViewColumnHeader::UnhookGripperEvents()
{
    if (m_pHeaderGripper != NULL)
    {
        m_pHeaderGripper->remove_DragStarted(DragStartedEventHandler::Create(this, &GridViewColumnHeader::OnColumnHeaderGripperDragStarted));
        m_pHeaderGripper->remove_DragDelta(DragDeltaEventHandler::Create(this, &GridViewColumnHeader::OnColumnHeaderResize));
        m_pHeaderGripper->remove_DragCompleted(DragCompletedEventHandler::Create(this, &GridViewColumnHeader::OnColumnHeaderGripperDragCompleted));
        m_pHeaderGripper->remove_MouseDoubleClick(MouseButtonEventHandler::Create(this, &GridViewColumnHeader::OnGripperDoubleClicked));
        m_pHeaderGripper->remove_MouseEnter(MouseEventHandler::Create(this, &GridViewColumnHeader::OnGripperMouseEnterLeave));
        m_pHeaderGripper->remove_MouseLeave(MouseEventHandler::Create(this, &GridViewColumnHeader::OnGripperMouseEnterLeave));

        m_pHeaderGripper = NULL;
    }
}

 
Cursor* GridViewColumnHeader::get_SplitCursor()
{
    return Cursors::get_Hand();
}

 
 

 
 
 

 

 

 

 




};


