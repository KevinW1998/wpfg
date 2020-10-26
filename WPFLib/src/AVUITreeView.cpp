#include <AVUICommon.h>
#include <AVUITreeView.h>
#include <AVUITreeViewItem.h>
#include <AVUIRoutedPropertyChanged.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemCollection.h>
#include <AVUIScrollViewer.h>
#include <AVUISelector.h>
#include <AVUIBinding.h>
#include <AVUIPropertyPath.h>


REGISTER_ELEMENT_BEGIN(AVUI::TreeView)
    REGISTER_DEFAULT_FACTORY()       
    REGISTER_PROPERTY(SelectedItem)
    REGISTER_PROPERTY(SelectedValue)
    REGISTER_PROPERTY(SelectedValuePath)

    REGISTER_ROUTED_EVENT(SelectedItemChanged)

REGISTER_ELEMENT_END()

namespace AVUI {

BindingExpressionUncommonField TreeView::SelectedValuePathBindingExpression;  


void TreeView::ChangeSelection(Object* pData, TreeViewItem* pContainer, bool selected)
{
    if (!get_IsSelectionChangeActive())
    {
        Object* pOldValue = NULL;
        Object* pNewValue = NULL;
        bool flag = false;
        TreeViewItem* pElement = m_pSelectedContainer;
        set_IsSelectionChangeActive(true);
        if (selected)
        {
            if (pContainer != m_pSelectedContainer)
            {
                pOldValue = get_SelectedItem();
                pNewValue = pData;
                if (m_pSelectedContainer != NULL)
                {
                    m_pSelectedContainer->set_IsSelected(false);
                    m_pSelectedContainer->UpdateContainsSelection(false);
                }
                m_pSelectedContainer = pContainer;
                m_pSelectedContainer->UpdateContainsSelection(true);
                SetSelectedItem(pData);
                UpdateSelectedValue(pData);
                flag = true;
            }
        }
        else if (pContainer == m_pSelectedContainer)
        {
            m_pSelectedContainer->UpdateContainsSelection(false);
            m_pSelectedContainer = NULL;
            SetSelectedItem(NULL);
            pOldValue = pData;
            flag = true;
        }
        if (pContainer->get_IsSelected() != selected)
        {
            pContainer->set_IsSelected(selected);
        }
        set_IsSelectionChangeActive(false);

        if (flag)
        {
            RoutedPropertyChangedObjectEventArgs args(pOldValue, pNewValue, &get_SelectedItemChangedEvent());
            OnSelectedItemChanged(&args);
        }
    }
}

DependencyObject* TreeView::FindParent(DependencyObject* pDO)
{
    Visual* pReference = object_cast<Visual>(pDO);
    /*
    ContentElement element = (reference == null) ? (o as ContentElement) : null;
    if (element != null)
    {
        o = ContentOperations.GetParent(element);
        if (o != null)
        {
            return o;
        }
        FrameworkContentElement element2 = element as FrameworkContentElement;
        if (element2 != null)
        {
            return element2.Parent;
        }
    }
    else 
    */
    if (pReference != NULL)
    {
        return pReference->get_VisualParent();
    }
    return NULL;
}

bool TreeView::FocusFirstItem()
{
    TreeViewItem* pItem = object_cast<TreeViewItem>(get_ItemContainerGenerator()->ContainerFromIndex(0));
    if (pItem == NULL)
    {
        return false;
    }
    return ((pItem->get_IsEnabled() && pItem->Focus()) || pItem->FocusDown());
}

 
bool TreeView::FocusLastItem()
{
    for (int i = get_Items()->get_Count() - 1; i >= 0; i--)
    {
        TreeViewItem* pItem = object_cast<TreeViewItem>(get_ItemContainerGenerator()->ContainerFromIndex(i));
        if (pItem != NULL && pItem->get_IsEnabled())
        {
            return TreeViewItem::FocusIntoItem(pItem);
        }
    }
    return false;
}

TRefCountedPtr<DependencyObject> TreeView::GetContainerForItemOverride()
{
    return (DependencyObject*)object_allocate<TreeViewItem>();
}

bool TreeView::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem != NULL && pItem->IsType(GET_TYPE(TreeViewItem));
}
 
bool TreeView::GetFirstItem(Object** ppItem, TreeViewItem** ppContainer)
{
    if (get_HasItems())
    {
        *ppItem = get_Items()->GetItemAt(0);
        *ppContainer = object_cast<TreeViewItem>(get_ItemContainerGenerator()->ContainerFromIndex(0));
        return (*ppItem) != NULL && (*ppContainer) != NULL;
    }
    *ppItem = NULL;
    *ppContainer = NULL;

    return false;
}

void TreeView::HandleMouseButtonDown()
{
    if (!get_IsKeyboardFocusWithin())
    {
        if (m_pSelectedContainer != NULL)
        {
            if (!m_pSelectedContainer->get_IsKeyboardFocused())
            {
                m_pSelectedContainer->Focus();
            }
        }
        else
        {
            Focus();
        }
    }
}

bool TreeView::HandleScrollByPage(bool up)
{
    ScrollViewer* pScrollHost = get_ScrollHost();
    if (pScrollHost != NULL)
    {
        float num2 = 0.0f;
        float num3 = 0.0f;
        float viewportHeight = pScrollHost->get_ViewportHeight();
        m_pSelectedContainer->GetTopAndBottom(pScrollHost, &num2, &num3);
        TreeViewItem* pItem = NULL;
        TreeViewItem* pContainer = m_pSelectedContainer;
        ItemsControl* pParentItemsControl = m_pSelectedContainer->get_ParentItemsControl();
        if (pParentItemsControl != NULL)
        {
            if (up)
            {
                while (pParentItemsControl != this)
                {
                    ItemsControl* pControl2 = ItemsControl::ItemsControlFromItemContainer(pParentItemsControl);
                    if (pControl2 == NULL)
                    {
                        break;
                    }
                    pContainer = (TreeViewItem*) pParentItemsControl;
                    pParentItemsControl = pControl2;
                }
            }
            int index = pParentItemsControl->get_ItemContainerGenerator()->IndexFromContainer(pContainer);
            int count = pParentItemsControl->get_Items()->get_Count();
            while (pParentItemsControl != NULL && pContainer != NULL)
            {
                ItemsControl* pControl3;
                if (pContainer->get_IsEnabled())
                {
                    float num6;
                    if (pContainer->HandleScrollByPage(up, pScrollHost, viewportHeight, num2, num3, &num6))
                    {
                        return true;
                    }
                    if (Float::GreaterThan(num6, viewportHeight))
                    {
                        if ((pItem == m_pSelectedContainer) || (pItem == NULL))
                        {
                            if (!up)
                            {
                                return m_pSelectedContainer->HandleDownKey();
                            }
                            return m_pSelectedContainer->HandleUpKey();
                        }
                        break;
                    }
                    pItem = pContainer;
                }
                index += up ? -1 : 1;
                if ((0 <= index) && (index < count))
                {
                    pContainer = object_cast<TreeViewItem>(pParentItemsControl->get_ItemContainerGenerator()->ContainerFromIndex(index));
                    continue;
                }
                else
                {
                    if (pParentItemsControl == this)
                    {
                        pContainer = NULL;
                        continue;
                    }
                }

                while (pParentItemsControl != NULL)
                {
                    pControl3 = pParentItemsControl;
                    pParentItemsControl = ItemsControl::ItemsControlFromItemContainer(pParentItemsControl);
                    if (pParentItemsControl != NULL)
                    {
                        count = pParentItemsControl->get_Items()->get_Count();
                        index = pParentItemsControl->get_ItemContainerGenerator()->IndexFromContainer(pControl3) + (up ? -1 : 1);
                        if ((0 <= index) && (index < count))
                        {
                            pContainer = object_cast<TreeViewItem>(pParentItemsControl->get_ItemContainerGenerator()->ContainerFromIndex(index));
                            break;
                        }
                        if (pParentItemsControl == this)
                        {
                            pParentItemsControl = (ItemsControl*) (pContainer = NULL);
                        }
                    }
                }
            }
            if (pItem != NULL)
            {
                if (!up)
                {
                    return TreeViewItem::FocusIntoItem(pItem);
                }
                if (pItem != m_pSelectedContainer)
                {
                    return pItem->Focus();
                }
            }
        }
    }
    return false;
}
 
bool TreeView::HandleScrollKeys(Key::Enum key)
{
    ScrollViewer* pScrollHost = get_ScrollHost();
    if (pScrollHost != NULL)
    {
        bool flag = get_FlowDirection() == FlowDirection::RightToLeft;
        switch (key)
        {
        case Key::Prior:
                if (!Float::GreaterThan(pScrollHost->get_ExtentHeight(), pScrollHost->get_ViewportHeight()))
                {
                    pScrollHost->PageLeft();
                }
                else
                {
                    pScrollHost->PageUp();
                }
                return true;

        case Key::Next:
                if (!Float::GreaterThan(pScrollHost->get_ExtentHeight(), pScrollHost->get_ViewportHeight()))
                {
                    pScrollHost->PageRight();
                }
                else
                {
                    pScrollHost->PageDown();
                }
                return true;

        case Key::End:
                pScrollHost->ScrollToBottom();
                return true;

        case Key::Home:
                pScrollHost->ScrollToTop();
                return true;

        case Key::Left:
                if (!flag)
                {
                    pScrollHost->LineLeft();
                }
                else
                {
                    pScrollHost->LineRight();
                }
                return true;

        case Key::Up:
                pScrollHost->LineUp();
                return true;

        case Key::Right:
                if (!flag)
                {
                    pScrollHost->LineRight();
                }
                else
                {
                    pScrollHost->LineLeft();
                }
                return true;

        case Key::Down:
                pScrollHost->LineDown();
                return true;
        }
    }
    return false;
}
 
void TreeView::HandleSelectionAndCollapsed(TreeViewItem* pCollapsed)
{
    if (m_pSelectedContainer != NULL && m_pSelectedContainer != pCollapsed)
    {
        TreeViewItem* pParentTreeViewItem = m_pSelectedContainer;
        do
        {
            pParentTreeViewItem = pParentTreeViewItem->get_ParentTreeViewItem();
            if (pParentTreeViewItem == pCollapsed)
            {
                ChangeSelection(pCollapsed->get_ParentItemsControl()->get_ItemContainerGenerator()->ItemFromContainer(pCollapsed), pCollapsed, true);
                return;
            }
        }
        while (pParentTreeViewItem != NULL);
    }
}

void TreeView::OnFocusEnterMainFocusScope(Object* pSender, EventArgs* pArgs)
{
    if (!get_IsKeyboardFocusWithin())
    {
        ClearValue(Selector::get_IsSelectionActiveProperty());
    }
}

void TreeView::OnIsKeyboardFocusWithinChanged(DependencyPropertyChangedEventArgs* pArgs)
{
    ItemsControl::OnIsKeyboardFocusWithinChanged(pArgs);
    /*
    bool flag = false;
    bool isKeyboardFocusWithin = get_IsKeyboardFocusWithin();
    if (isKeyboardFocusWithin)
    {
        flag = true;
    }
    else
    {
        DependencyObject focusedElement = Keyboard.FocusedElement as DependencyObject;
        if (focusedElement != null)
        {
            UIElement visualRoot = KeyboardNavigation.GetVisualRoot(this) as UIElement;
            if (((visualRoot != null) && visualRoot.IsKeyboardFocusWithin) && (FocusManager.GetFocusScope(focusedElement) != visualRoot))
            {
                flag = true;
            }
        }
    }
    if (((bool) base.GetValue(Selector.IsSelectionActiveProperty)) != flag)
    {
        base.SetValue(Selector.IsSelectionActivePropertyKey, BooleanBoxes.Box(flag));
    }
    if ((isKeyboardFocusWithin && base.IsKeyboardFocused) && ((this._selectedContainer != null) && !this._selectedContainer.IsKeyboardFocusWithin))
    {
        this._selectedContainer.Focus();
    }
    */
}
 
void TreeView::OnItemsChanged(CollectionChangedEventArgs* pArgs)
{
    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Add:
    case NotifyCollectionChangedAction::Move:
            break;

    case NotifyCollectionChangedAction::Remove:
    case NotifyCollectionChangedAction::Reset:
            if (get_SelectedItem() == NULL || get_IsSelectedContainerHookedUp())
            {
                break;
            }
            SelectFirstItem();
            return;

    case NotifyCollectionChangedAction::Replace:
        {
            Object* pSelectedItem = get_SelectedItem();
            if (pSelectedItem == NULL || !pSelectedItem->Equals(pArgs->get_OldItem()))
            {
                break;
            }
            ChangeSelection(pSelectedItem, m_pSelectedContainer, false);
            return;
        }
    default:
        LibraryCriticalError();
    }
}

void TreeView::OnKeyDown(KeyEventArgs* pArgs)
{
    ItemsControl::OnKeyDown(pArgs);
    if (!pArgs->get_IsHandled())
    {
        if (get_IsControlKeyDown())
        {
            switch (pArgs->get_Key())
            {
            case Key::Prior:
                case Key::Next:
                case Key::End:
                case Key::Home:
                case Key::Left:
                case Key::Up:
                case Key::Right:
                case Key::Down:
                    if (HandleScrollKeys(pArgs->get_Key()))
                    {
                        pArgs->set_IsHandled(true);
                    }
                    return;
            }
        }
        else
        {
            switch (pArgs->get_Key())
            {
            case Key::Prior:
            case Key::Next:
                if (m_pSelectedContainer != NULL)
                {
                    if (HandleScrollByPage(pArgs->get_Key() == Key::Prior))
                    {
                        pArgs->set_IsHandled(true);
                    }
                    return;
                }
                if (FocusFirstItem())
                {
                    pArgs->set_IsHandled(true);
                }
                return;

            case Key::End:
                if (FocusLastItem())
                {
                    pArgs->set_IsHandled(true);
                }
                return;

            case Key::Home:
                if (FocusFirstItem())
                {
                    pArgs->set_IsHandled(true);
                }
                return;

            case Key::Left:
            case Key::Right:
                return;

            case Key::Up:
            case Key::Down:
                if (m_pSelectedContainer == NULL && FocusFirstItem())
                {
                    pArgs->set_IsHandled(true);
                }
                return;

            case Key::Tab:
                /*
                if ((IsShiftKeyDown && base.IsKeyboardFocusWithin) && this.MoveFocus(new TraversalRequest(FocusNavigationDirection.Previous)))
                {
                    e.Handled = true;
                }
                */
                return;

            default:
                return;
            }
        }
    }
}
 
void TreeView::OnSelectedItemChanged(RoutedPropertyChangedObjectEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void TreeView::OnSelectedValuePathChanged(Object* pDO, const DependencyPropertyChangedEventArgs& args)
{
    TreeView* pInstance = (TreeView*) pDO;
    SelectedValuePathBindingExpression.ClearValue(pInstance);
    pInstance->UpdateSelectedValue(pInstance->get_SelectedItem());
}

TRefCountedPtr<BindingExpression> TreeView::PrepareSelectedValuePathBindingExpression(Object* pItem)
{
    if (pItem == NULL)
    {
        return NULL;
    }
    bool flag = false; //XmlHelper.IsXmlNode(item);
    TRefCountedPtr<BindingExpression> pBindingExpr = SelectedValuePathBindingExpression.GetValue(this);
    if (pBindingExpr != NULL)
    {
        bool flag2 = pBindingExpr->get_ParentBinding()->get_XPath() != NULL;
        if (flag2 != flag)
        {
            pBindingExpr = NULL;
        }
    }
    if (pBindingExpr == NULL)
    {
        TRefCountedPtr<Binding> pBinding = object_allocate<Binding>();
        pBinding->set_Source(pItem);
        /*
        if (flag)
        {
            binding.XPath = this.SelectedValuePath;
            binding.Path = new PropertyPath("/InnerText", new object[0]);
        }
        else
        */
        {
            if(get_SelectedValuePath() != NULL)
            {
                pBinding->set_Path(object_allocate<PropertyPath>(*get_SelectedValuePath())); //, new object[0]);
            }
            else
            {
                pBinding->set_Path(object_allocate<PropertyPath>()); //, new object[0]);
            }
        }
        pBindingExpr = (BindingExpression*) (BindingExpressionBase*)BindingExpressionBase::CreateUntargetedBindingExpression(this, pBinding);
        SelectedValuePathBindingExpression.SetValue(this, pBindingExpr);
    }
    return pBindingExpr;
}

void TreeView::SelectFirstItem()
{
    Object* pSelectedItem;
    TreeViewItem* pItem;
    bool firstItem = GetFirstItem(&pSelectedItem, &pItem);
    if (!firstItem)
    {
        pSelectedItem = get_SelectedItem();
        pItem = m_pSelectedContainer;
    }
    ChangeSelection(pSelectedItem, pItem, firstItem);
}

void TreeView::SetSelectedItem(Object* pData)
{
    if (get_SelectedItem() != pData)
    {
        SetValue(get_SelectedItemProperty(), pData);
    }
}

void TreeView::SetSelectedValue(Object* pData)
{
    if (get_SelectedValue() != pData)
    {
        SetValue(get_SelectedValueProperty(), pData);
    }
}

void TreeView::UpdateSelectedValue(Object* pSelectedItem)
{
    TRefCountedPtr<BindingExpression> pExpression = PrepareSelectedValuePathBindingExpression(pSelectedItem);
    if (pExpression != NULL)
    {
        pExpression->Activate(pSelectedItem);
        Object* pObj2 = pExpression->get_Value();
        pExpression->Deactivate();
        SetValue(get_SelectedValueProperty(), pObj2);
    }
    else
    {
        ClearValue(get_SelectedValueProperty());
    }
}

bool TreeView::get_IsControlKeyDown()
{
    return (Keyboard::get_Current()->get_KeyboardModifiers() & ModifierKeys::Control) == ModifierKeys::Control;
}

bool TreeView::get_IsShiftKeyDown()
{
    return (Keyboard::get_Current()->get_KeyboardModifiers() & ModifierKeys::Shift) == ModifierKeys::Shift;
}

bool TreeView::get_IsSelectedContainerHookedUp()
{
    return m_pSelectedContainer != NULL && m_pSelectedContainer->get_ParentTreeView() == this;
}
 

 
 

 
 

 
 
 
 

 
 

 

 

 
 

 



}; // namespace AVUI

