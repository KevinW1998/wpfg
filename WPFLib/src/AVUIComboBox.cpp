#include <AVUICommon.h>
#include <AVUIComboBox.h>
#include <AVUIComboBoxItem.h>
#include <AVUISeparator.h>
#include <AVUIDataTemplate.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemCollection.h>
#include <AVUIPopupRoot.h>
#include <AVUIDispatcherTimer.h>
#include <AVUIPopup.h>
#include <AVUIPanel.h>
#include <AVUIMenuBase.h>

REGISTER_ELEMENT_BEGIN(AVUI::ComboBox)
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(SelectionBoxItem)
    REGISTER_PROPERTY(SelectionBoxItemTemplate)
    REGISTER_PROPERTY(Text)
    REGISTER_PROPERTY(IsDropDownOpen)
    REGISTER_PROPERTY(IsEditable)
    REGISTER_PROPERTY(IsReadOnly)
    REGISTER_PROPERTY(IsSelectionBoxHighlighted)
    REGISTER_PROPERTY(MaxDropDownHeight)
    REGISTER_PROPERTY(StaysOpenOnEdit)
REGISTER_ELEMENT_END()

namespace AVUI {

void ComboBox::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(ComboBox), Mouse::get_LostMouseCaptureEvent(), MouseEventHandler::Create(OnLostMouseCapture), false);
    EventManager::RegisterClassHandler(GET_TYPE(ComboBox), Mouse::get_MouseDownEvent(), MouseButtonEventHandler::Create(OnMouseButtonDown), false);
    EventManager::RegisterClassHandler(GET_TYPE(ComboBox), Mouse::get_MouseMoveEvent(), MouseEventHandler::Create(OnMouseMove), false);
}


void ComboBox::PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem)
{
    Selector::PrepareContainerForItemOverride(pElement, pItem);

    if(pItem != NULL && pItem->IsType(GET_TYPE(Separator)))
    {
        Separator::PrepareContainer(object_cast<Control>(pElement));
    }
}

bool ComboBox::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem != NULL && pItem->IsType(GET_TYPE(ComboBoxItem));
}

TRefCountedPtr<DependencyObject> ComboBox::GetContainerForItemOverride()
{
    return (DependencyObject*)object_allocate<ComboBoxItem>();
}


void ComboBox::OnApplyTemplate()
{
    Selector::OnApplyTemplate();

    /*
    if (this._dropDownPopup != null)
    {
        this._dropDownPopup.Closed -= new EventHandler(this.OnPopupClosed);
    }
    this.EditableTextBoxSite = base.GetTemplateChild("PART_EditableTextBox") as TextBox;
    this._dropDownPopup = base.GetTemplateChild("PART_Popup") as Popup;
    if (this.EditableTextBoxSite != null)
    {
        this.EditableTextBoxSite.TextChanged += new TextChangedEventHandler(this.OnEditableTextBoxTextChanged);
        this.EditableTextBoxSite.SelectionChanged += new RoutedEventHandler(this.OnEditableTextBoxSelectionChanged);
    }
    if (this._dropDownPopup != null)
    {
        this._dropDownPopup.Closed += new EventHandler(this.OnPopupClosed);
    }
    */

    Update();
}

void ComboBox::Update()
{
    /*
    if (this.IsEditable)
    {
        this.UpdateEditableTextBox();
    }
    else
    */
    {
        UpdateSelectionBoxItem();
    }
}


void ComboBox::UpdateSelectionBoxItem()
{
    Object* pInternalSelectedItem = get_InternalSelectedItem();
    DataTemplate* pItemTemplate = get_ItemTemplate();
    ContentControl* pControl = pInternalSelectedItem != NULL ? object_cast<ContentControl>(pInternalSelectedItem) : NULL;
    if (pControl != NULL)
    {
        pInternalSelectedItem = pControl->get_Content();
        pItemTemplate = pControl->get_ContentTemplate();
    }
    /*
    if (this._clonedElement != null)
    {
        this._clonedElement.LayoutUpdated -= new EventHandler(this.CloneLayoutUpdated);
        this._clonedElement = null;
    }
    */

    /*
    if ((itemTemplate == null) && (base.ItemTemplateSelector == null))
    {
        DependencyObject d = internalSelectedItem as DependencyObject;
        if (d != null)
        {
            this._clonedElement = d as UIElement;
            if (this._clonedElement != null)
            {
                VisualBrush brush = new VisualBrush(this._clonedElement) {
                    Stretch = Stretch.None,
                    ViewboxUnits = BrushMappingMode.Absolute,
                    Viewbox = new Rect(this._clonedElement.RenderSize),
                    ViewportUnits = BrushMappingMode.Absolute,
                    Viewport = new Rect(this._clonedElement.RenderSize)
                };
                FlowDirection direction = (FlowDirection) this._clonedElement.GetValue(FrameworkElement.FlowDirectionProperty);
                DependencyObject parent = VisualTreeHelper.GetParent(this._clonedElement);
                FlowDirection direction2 = (parent == null) ? base.FlowDirection : ((FlowDirection) parent.GetValue(FrameworkElement.FlowDirectionProperty));
                if ((direction != base.FlowDirection) != (direction != direction2))
                {
                    brush.Transform = new MatrixTransform(new Matrix(-1.0, 0.0, 0.0, 1.0, this._clonedElement.RenderSize.Width, 0.0));
                }
                Rectangle rectangle = new Rectangle {
                    Fill = brush,
                    Width = this._clonedElement.RenderSize.Width,
                    Height = this._clonedElement.RenderSize.Height
                };
                this._clonedElement.LayoutUpdated += new EventHandler(this.CloneLayoutUpdated);
                internalSelectedItem = rectangle;
                itemTemplate = null;
            }
            else
            {
                internalSelectedItem = ExtractString(d);
                itemTemplate = ContentPresenter.StringContentTemplate;
            }
        }
    }

    if (internalSelectedItem == null)
    {
        internalSelectedItem = string.Empty;
        itemTemplate = ContentPresenter.StringContentTemplate;
    }
    */


    SetValue(get_SelectionBoxItemProperty(), pInternalSelectedItem);
    SetValue(get_SelectionBoxItemTemplateProperty(), pItemTemplate);
}

void ComboBox::KeyDownHandler(KeyEventArgs* pArgs)
{
    bool flag = false;
    Key::Enum key = pArgs->get_Key();


    switch (key)
    {
    case Key::Escape:
        if (get_IsDropDownOpen())
        {
            KeyboardCloseDropDown(false);
            flag = true;
        }
        break;

    case Key::Prior:
        if (get_IsItemsHostVisible())
        {
            ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());
            NavigateByPage(get_HighlightedItem(), FocusNavigationDirection::Up, &itemNavArgs);
            flag = true;
        }
        break;

    case Key::Next:
        if (get_IsItemsHostVisible())
        {
            ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());
            NavigateByPage(get_HighlightedItem(), FocusNavigationDirection::Down, &itemNavArgs);
            flag = true;
        }
        break;

    case Key::End:
    {
        if (((pArgs->get_Device()->get_KeyboardModifiers() & ModifierKeys::Alt) != ModifierKeys::Alt) && !get_IsEditable())
        {
            if (!get_IsItemsHostVisible())
            {
                SelectLast();
            }
            else
            {
                ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());
                NavigateToEnd(&itemNavArgs);
            }
            flag = true;
        }
        break;
    }

    case Key::Home:
    {
            if (((pArgs->get_Device()->get_KeyboardModifiers() & ModifierKeys::Alt) == ModifierKeys::Alt) || get_IsEditable())
            {
                break;
            }
            if (!get_IsItemsHostVisible())
            {
                SelectFirst();
                flag = true;
                break;
            }

            ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());

            NavigateToStart(&itemNavArgs);
            flag = true;

            break;
    }

    case Key::Left:
    {
        if(!get_IsItemsHostLogicalHorizontal())
        {
            break;
        }

        if (((pArgs->get_Device()->get_KeyboardModifiers() & ModifierKeys::Alt) != ModifierKeys::Alt) && !get_IsEditable())
        {
            if (!get_IsItemsHostVisible())
            {
                SelectPrev();
            }
            else
            {
                ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());

                NavigateByLine(get_HighlightedItem(), FocusNavigationDirection::Left, &itemNavArgs);
            }
            flag = true;
        }
        break;
    }

    case Key::Up:
    {
        if(!get_IsItemsHostLogicalVertical())
        {
            break;
        }

        flag = true;
        if ((pArgs->get_Device()->get_KeyboardModifiers() & ModifierKeys::Alt) != ModifierKeys::Alt)
        {
            if (get_IsItemsHostVisible())
            {
                ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());
                NavigateByLine(get_HighlightedItem(), FocusNavigationDirection::Up, &itemNavArgs);
            }
            else
            {
                SelectPrev();
            }
        }
        else
        {
            KeyboardToggleDropDown(true);
        }
        break;
    }

    case Key::Right:
    {
        if(!get_IsItemsHostLogicalHorizontal())
        {
            break;
        }

        if (((pArgs->get_Device()->get_KeyboardModifiers() & ModifierKeys::Alt) != ModifierKeys::Alt) && !get_IsEditable())
        {
            if (!get_IsItemsHostVisible())
            {
                SelectNext();
            }
            else
            {
                ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());
                NavigateByLine(get_HighlightedItem(), FocusNavigationDirection::Right, &itemNavArgs);
            }
            flag = true;
        }
        break;
    }

    case Key::Down:
    {
        if(!get_IsItemsHostLogicalVertical())
        {
            break;
        }

        flag = true;
        if ((pArgs->get_Device()->get_KeyboardModifiers() & ModifierKeys::Alt) != ModifierKeys::Alt)
        {
            if (get_IsItemsHostVisible())
            {
                ItemNavigateArgs itemNavArgs(pArgs->get_Device(), pArgs->get_Device()->get_KeyboardModifiers());
                NavigateByLine(get_HighlightedItem(), FocusNavigationDirection::Down, &itemNavArgs);
            }
            else
            {
                SelectNext();
            }
        }
        else
        {
            KeyboardToggleDropDown(true);
        }
        break;
    }

    case Key::Return:
        if (get_IsDropDownOpen())
        {
            KeyboardCloseDropDown(true);
            flag = true;
        }
        break;

    default:
        flag = false;
    }

    if (flag)
    {
        pArgs->set_IsHandled(true);
    }
}

bool ComboBox::get_IsItemsHostVisible()
{
    UIElement* pElement = (UIElement*)get_ItemsHost();

    while(pElement != NULL)
    {
        if(pElement->IsType(GET_TYPE(PopupRoot)))
        {
            return ((PopupRoot*) pElement)->get_IsShowing();
        }

        pElement = object_cast<UIElement>(pElement->get_VisualParent());

    }
    return false;
}


Object* ComboBox::get_HighlightedItem()
{
    ComboBoxItem* pHighlightedElement = get_HighlightedElement();
    if (pHighlightedElement != NULL)
    {
        return get_ItemContainerGenerator()->ItemFromContainer(pHighlightedElement);
    }
    return NULL;
}

ComboBoxItem* ComboBox::get_HighlightedElement()
{
    return m_pHighlightedElement;
}

void ComboBox::set_HighlightedElement(ComboBoxItem* pHighlightedElement)
{
    if(m_pHighlightedElement != NULL)
    {
        m_pHighlightedElement->SetIsHighlighted(false);
    }

    m_pHighlightedElement = pHighlightedElement;

    if(m_pHighlightedElement)
    {
        m_pHighlightedElement->SetIsHighlighted(true);
    }

    CoerceValue(get_IsSelectionBoxHighlightedProperty());
}


void ComboBox::KeyboardToggleDropDown(bool commitSelection)
{
    KeyboardToggleDropDown(!get_IsDropDownOpen(), commitSelection);
}

void ComboBox::KeyboardToggleDropDown(bool openDropDown, bool commitSelection)
{
    Object* pItem = NULL;
    if (commitSelection)
    {
        pItem = get_HighlightedItem();
    }
    set_IsDropDownOpen(openDropDown);
    if (!openDropDown && commitSelection && pItem != NULL)
    {
        get_SelectionChange()->SelectJustThisItem(pItem, true);
    }
}

void ComboBox::KeyboardCloseDropDown(bool commitSelection)
{
    KeyboardToggleDropDown(false, commitSelection);
}

void ComboBox::SelectFirst()
{
    SelectItemHelper(0, 1, get_Items()->get_Count());
}

void ComboBox::SelectLast()
{
    SelectItemHelper(get_Items()->get_Count() - 1, -1, -1);
}

void ComboBox::SelectNext()
{
    int count = get_Items()->get_Count();
    if (count > 0)
    {
        int internalSelectedIndex = get_InternalSelectedIndex();
        if (internalSelectedIndex < (count - 1))
        {
            SelectItemHelper(internalSelectedIndex + 1, 1, count);
        }
    }
}

void ComboBox::SelectPrev()
{
    if(!get_Items()->get_IsEmpty())
    {
        int internalSelectedIndex = get_InternalSelectedIndex();

        if(internalSelectedIndex > 0)
        {
            SelectItemHelper(internalSelectedIndex - 1, -1, -1);
        }
    }
}


void ComboBox::SelectItemHelper(int startIndex, int increment, int stopIndex)
{
    for (int i = startIndex; i != stopIndex; i += increment)
    {
        Object* pO = get_Items()->GetItemAt(i);
        if (IsSelectableHelper(pO) && IsSelectableHelper(get_ItemContainerGenerator()->ContainerFromIndex(i)))
        {
            get_SelectionChange()->SelectJustThisItem(pO, true);
            return;
        }
    }
}
 
bool ComboBox::IsSelectableHelper(Object* pO)
{
    DependencyObject* pDO = object_cast<DependencyObject>(pO);
    return pDO == NULL || UnboxValue<bool>(pDO->GetValue(UIElement::get_IsEnabledProperty()));
}

void ComboBox::FocusItem(Object* pItem, ItemNavigateArgs* pItemNavigateArgs)
{
    if (!get_IsEditable())
    {
        Selector::FocusItem(pItem, pItemNavigateArgs);
    }
    ComboBoxItem* pContainer = object_cast<ComboBoxItem>(get_ItemContainerGenerator()->ContainerFromItem(pItem));
    set_HighlightedElement(pContainer);
    if ((get_IsEditable() || !get_IsDropDownOpen()) && (pItemNavigateArgs->pInputDevice != NULL))
    {
        if (pContainer != NULL)
        {
            set_SelectedIndex(get_ItemContainerGenerator()->IndexFromContainer(pContainer));
        }
        else
        {
            set_SelectedIndex(get_Items()->IndexOf(pItem));
        }
    }
}
 
void ComboBox::OnSelectionChanged(SelectionChangedEventArgs* pArgs)
{
    Selector::OnSelectionChanged(pArgs);
    SelectedItemUpdated();
    if (get_IsDropDownOpen())
    {
        Object* pInternalSelectedItem = get_InternalSelectedItem();
        if (pInternalSelectedItem != NULL)
        {
            NavigateToItem(pInternalSelectedItem, NULL);
        }
    }
}

void ComboBox::SelectedItemUpdated()
{
    m_updatingSelectedItem = true;

    /*
    if (!get_UpdatingText())
    {
        string primaryTextFromItem = TextSearch.GetPrimaryTextFromItem(this, base.InternalSelectedItem);
        if (this.Text != primaryTextFromItem)
        {
            this.Text = primaryTextFromItem;
        }
    }
    */
    Update();


    m_updatingSelectedItem = false;
}
 
void ComboBox::NotifyComboBoxItemEnter(ComboBoxItem* pItem)
{
    if (get_IsDropDownOpen() && Mouse::get_Captured() == this && DidMouseMove())
    {
        set_HighlightedElement(pItem);
        if (!get_IsEditable() && !pItem->get_IsKeyboardFocusWithin())
        {
            pItem->Focus();
        }
    }
}

void ComboBox::AsyncNavigateToSelectedItem(Object* pSender, EventArgs* pArgs)
{
    if(get_IsItemsHostVisible())
    {
        NavigateToItem(get_InternalSelectedItem(), NULL, true);
        ((DispatcherTimer*)pSender)->Stop();
    }
}

void ComboBox::OnIsDropDownOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ComboBox* pElement = (ComboBox*) pDO;

    pElement->m_hasMouseEnteredItemsHost = false;
    bool newValue = UnboxValue<bool>(e.pNewValue);
    bool oldValue = !newValue;

    if (newValue)
    {
        Mouse::Capture(pElement, CaptureMode::SubTree);
        /*
        if (element.IsEditable && (element.EditableTextBoxSite != null))
        {
            element.EditableTextBoxSite.SelectAll();
        }
        */

        TRefCountedPtr<DispatcherTimer> pTimer = object_allocate<DispatcherTimer>(TimeSpan::get_Zero(), EventHandler::Create(pElement, &ComboBox::AsyncNavigateToSelectedItem));
        pTimer->Start();
        
        /*
        element.OnDropDownOpened(EventArgs.Empty);
        */
    }
    else
    {
        if (pElement->get_IsKeyboardFocusWithin())
        {
/*
            if (element.IsEditable)
            {
                if ((element.EditableTextBoxSite != null) && !element.EditableTextBoxSite.IsKeyboardFocusWithin)
                {
                    element.Focus();
                }
            }
            else
*/
            {
                pElement->Focus();
            }
        }

        // Use previous implementation, not this one
        pElement->Focus();

        pElement->set_HighlightedElement(NULL);

        if (pElement->get_HasCapture())
        {
            Mouse::Capture(NULL);
        }

        /*
        if (element._dropDownPopup == null)
        {
            element.OnDropDownClosed(EventArgs.Empty);
        }
        */

    }
    pElement->CoerceValue(get_IsSelectionBoxHighlightedProperty());
    //element.CoerceValue(ToolTipService.IsEnabledProperty);
}
 
void ComboBox::NotifyComboBoxItemMouseUp(ComboBoxItem* pComboBoxItem)
{
    Object* pItem = get_ItemContainerGenerator()->ItemFromContainer(pComboBoxItem);
    if (pItem != NULL)
    {
        get_SelectionChange()->SelectJustThisItem(pItem, true);
    }
    Close();
}
 
void ComboBox::Close()
{
    if (get_IsDropDownOpen())
    {
        ClearValue(get_IsDropDownOpenProperty());
        if (get_IsDropDownOpen())
        {
            set_IsDropDownOpen(false);
        }
    }
}

void ComboBox::OnLostMouseCapture(Object* pSender, MouseEventArgs* pArgs)
{
    ComboBox* pReference = (ComboBox*) pSender;
    if (Mouse::get_Captured() != pReference)
    {
        if (pArgs->get_OriginalSource() == pReference)
        {
            if (Mouse::get_Captured() == NULL || !MenuBase::IsDescendant(pReference, Mouse::get_Captured()))
            {
                pReference->Close();
            }
        }
        else if (MenuBase::IsDescendant(pReference, object_cast<DependencyObject>(pArgs->get_OriginalSource())))
        {
            if (pReference->get_IsDropDownOpen() && Mouse::get_Captured() == NULL)
            {
                Mouse::Capture(pReference, CaptureMode::SubTree);
                pArgs->set_IsHandled(true);
            }
        }
        else
        {
            pReference->Close();
        }
    }
}
 

void ComboBox::OnMouseButtonDown(Object* pSender, MouseButtonEventArgs* pArgs)
{
    ComboBox* pBox = (ComboBox*) pSender;

    if (!pBox->m_isContextMenuOpen && !pBox->get_IsKeyboardFocusWithin())
    {
        pBox->Focus();
    }
    pArgs->set_IsHandled(true);
    if ((Mouse::get_Captured() == pBox) && pArgs->get_OriginalSource() == pBox)
    {
        pBox->Close();
    }
}

void ComboBox::OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs)
{
    if (get_HasMouseEnteredItemsHost() && !get_IsMouseOverItemsHost() && get_IsDropDownOpen())
    {
        Close();
        pArgs->set_IsHandled(true);
    }
    Selector::OnMouseLeftButtonUp(pArgs);
}

 
void ComboBox::OnMouseMove(Object* pSender, MouseEventArgs* pArgs)
{
    ComboBox* pBox = (ComboBox*) pSender;
    if (pBox->get_IsDropDownOpen())
    {
        bool flag = (pBox->get_ItemsHost() != NULL) ? pBox->get_ItemsHost()->get_IsMouseOver() : false;
        if (flag && !pBox->get_HasMouseEnteredItemsHost())
        {
            pBox->SetInitialMousePosition();
        }
        pBox->set_IsMouseOverItemsHost(flag);
        pBox->set_HasMouseEnteredItemsHost(pBox->get_HasMouseEnteredItemsHost() | flag);
    }
    /*
    if (Mouse::get_LeftButton() == MouseButtonState::Pressed) && box.HasMouseEnteredItemsHost) && (Mouse.Captured == box))
    {
        if (Mouse.LeftButton == MouseButtonState.Pressed)
        {
            box.DoAutoScroll(box.HighlightedItem);
        }
        else
        {
            box.ReleaseMouseCapture();
            box.ResetLastMousePosition();
        }
        e.Handled = true;
    }
    */
}

 
bool ComboBox::get_HasCapture()
{
    return Mouse::get_Captured() == this;
}
 

 


 

 
 

 




}; // namespace AVUI

