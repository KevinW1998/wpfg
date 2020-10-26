#include <AVUICommon.h>
#include <AVUITreeViewItem.h>
#include <AVUITreeView.h>
#include <AVUIINotifyCollectionChanged.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemCollection.h>
#include <AVUIScrollViewer.h>
#include <AVUIRequestBringIntoViewEventArgs.h>

REGISTER_ELEMENT_BEGIN(AVUI::TreeViewItem)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(IsExpanded)
    REGISTER_PROPERTY(IsSelected)
    REGISTER_PROPERTY(IsSelectionActive)

    REGISTER_ROUTED_EVENT(Expanded);
    REGISTER_ROUTED_EVENT(Collapsed);
    REGISTER_ROUTED_EVENT(Selected);
    REGISTER_ROUTED_EVENT(Unselected);

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
REGISTER_ELEMENT_END()

namespace AVUI {

void TreeViewItem::StaticConstructor()
{
    /*
    KeyboardNavigation.DirectionalNavigationProperty.OverrideMetadata(typeof(TreeViewItem), new FrameworkPropertyMetadata(KeyboardNavigationMode.Continue));
    KeyboardNavigation.TabNavigationProperty.OverrideMetadata(typeof(TreeViewItem), new FrameworkPropertyMetadata(KeyboardNavigationMode.None));
    Control.IsTabStopProperty.OverrideMetadata(typeof(TreeViewItem), new FrameworkPropertyMetadata(BooleanBoxes.FalseBox));
    EventManager.RegisterClassHandler(typeof(TreeViewItem), FrameworkElement.RequestBringIntoViewEvent, new RequestBringIntoViewEventHandler(TreeViewItem.OnRequestBringIntoView));
    EventManager.RegisterClassHandler(typeof(TreeViewItem), Mouse.MouseDownEvent, new MouseButtonEventHandler(TreeViewItem.OnMouseButtonDown), true);
    */
}

bool TreeViewItem::get_IsControlKeyDown()
{
    return (Keyboard::get_Current()->get_KeyboardModifiers() & ModifierKeys::Control) == ModifierKeys::Control;
}

TreeView* TreeViewItem::get_ParentTreeView()
{
    for (ItemsControl* pControl = get_ParentItemsControl(); pControl != NULL; pControl = ItemsControl::ItemsControlFromItemContainer(pControl))
    {
        TreeView* pView = object_cast<TreeView>(pControl);
        if (pView != NULL)
        {
            return pView;
        }
    }
    return NULL;
}

TRefCountedPtr<DependencyObject> TreeViewItem::GetContainerForItemOverride() { return (DependencyObject*) object_allocate<TreeViewItem>(); }
bool TreeViewItem::IsItemItsOwnContainerOverride(Object* pItem) { return pItem != NULL && pItem->IsType(GET_TYPE(TreeViewItem)); }

void TreeViewItem::OnItemsChanged(CollectionChangedEventArgs* pArgs)
{
    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Add:
    case NotifyCollectionChangedAction::Move:
            break;

    case NotifyCollectionChangedAction::Remove:
    case NotifyCollectionChangedAction::Reset:
        {
            if (!get_ContainsSelection())
            {
                break;
            }
            TreeView* pParentTreeView = get_ParentTreeView();
            if (pParentTreeView == NULL || pParentTreeView->get_IsSelectedContainerHookedUp())
            {
                break;
            }
            set_ContainsSelection(false);
            Select(true);
            return;
        }
    case NotifyCollectionChangedAction::Replace:
        {
            if (!get_ContainsSelection())
            {
                break;
            }
            TreeView* pView2 = get_ParentTreeView();
            if (pView2 == NULL)
            {
                break;
            }
            Object* pSelectedItem = pView2->get_SelectedItem();
            if (pSelectedItem == NULL || !pSelectedItem->Equals(pArgs->get_OldItem()))
            {
                break;
            }
            pView2->ChangeSelection(pSelectedItem, pView2->get_SelectedContainer(), false);
            return;
        }
    default:
        LibraryCriticalError();
    }
}

void TreeViewItem::OnKeyDown(KeyEventArgs* pArgs)
{
    HeaderedItemsControl::OnKeyDown(pArgs);
    if (!pArgs->get_IsHandled())
    {
        switch (pArgs->get_Key())
        {
        case Key::Left:
        case Key::Right:
                if (!LogicalLeft(pArgs->get_Key()))
                {
                    if (!get_IsControlKeyDown() && get_CanExpandOnInput())
                    {
                        if (!get_IsExpanded())
                        {
                            set_IsExpanded(true);
                            pArgs->set_IsHandled(true);
                            return;
                        }
                        if (HandleDownKey())
                        {
                            pArgs->set_IsHandled(true);
                            return;
                        }
                    }
                    return;
                }
                if ((get_IsControlKeyDown() || !get_CanExpandOnInput()) || !get_IsExpanded())
                {
                    return;
                }
                if (!get_IsFocused())
                {
                    Focus();
                    break;
                }
                set_IsExpanded(false);
                break;

        case Key::Up:
                if (!get_IsControlKeyDown() && HandleUpKey())
                {
                    pArgs->set_IsHandled(true);
                }
                return;

        case Key::Down:
                if (!get_IsControlKeyDown() && HandleDownKey())
                {
                    pArgs->set_IsHandled(true);
                }
                return;

        case Key::Add:
                if (get_CanExpandOnInput() && !get_IsExpanded())
                {
                    set_IsExpanded(true);
                    pArgs->set_IsHandled(true);
                }
                return;

        case Key::Separator:
                return;

        case Key::Subtract:
                if (get_CanExpandOnInput() && get_IsExpanded())
                {
                    set_IsExpanded(false);
                    pArgs->set_IsHandled(true);
                }
                return;

            default:
                return;
        }
        pArgs->set_IsHandled(true);
    }
}
void TreeViewItem::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled() && get_IsEnabled())
    {
        if (Focus())
        {
            pArgs->set_IsHandled(true);
        }
        if ((pArgs->get_ClickCount() % 2) == 0)
        {
            set_IsExpanded(!get_IsExpanded());
            pArgs->set_IsHandled(true);
        }
    }
    HeaderedItemsControl::OnMouseLeftButtonDown(pArgs);
}
void TreeViewItem::OnVisualParentChanged(DependencyObject* pOldParent)
{
    if (get_VisualParent() != NULL && get_IsSelected())
    {
        Select(true);
    }
    HeaderedItemsControl::OnVisualParentChanged(pOldParent);
}

bool TreeViewItem::AllowHandleKeyEvent(FocusNavigationDirection::Enum direction)
{
    if (!get_IsSelected())
    {
        return false;
    }
    /*
    DependencyObject focusedElement = Keyboard.FocusedElement as DependencyObject;
    if ((focusedElement != null) && UIElementHelper.IsUIElementOrUIElement3D(focusedElement))
    {
        DependencyObject reference = UIElementHelper.PredictFocus(focusedElement, direction);
        if (reference != focusedElement)
        {
            while (reference != null)
            {
                TreeViewItem item = reference as TreeViewItem;
                if (item == this)
                {
                    return false;
                }
                if ((item != null) || (reference is TreeView))
                {
                    return true;
                }
                reference = VisualTreeHelper.GetParent(reference);
            }
        }
    }
    */
    return true;
}

float TreeViewItem::CalculateDelta(bool up, UIElement* pItem, ScrollViewer* pScroller, float startTop, float startBottom)
{
    float num;
    return CalculateDelta(up, pItem, pScroller, startTop, startBottom, &num);
}

float TreeViewItem::CalculateDelta(bool up, UIElement* pItem, ScrollViewer* pScroller, float startTop, float startBottom, float* pfCloseEdge)
{
    float num;
    float num2;

    GetTopAndBottom(pItem, pScroller, &num, &num2);

    if (up)
    {
        *pfCloseEdge = startBottom - num2;
        return (startBottom - num);
    }
    *pfCloseEdge = num - startTop;
    return (num2 - startTop);
}
 
bool TreeViewItem::FocusDown()
{
    ItemsControl* pParentItemsControl = get_ParentItemsControl();
    if (pParentItemsControl != NULL)
    {
        TreeViewItem* pItem;
        int index = pParentItemsControl->get_ItemContainerGenerator()->IndexFromContainer(this);
        int count = pParentItemsControl->get_Items()->get_Count();
        while (index < count)
        {
            index++;
            pItem = object_cast<TreeViewItem>(pParentItemsControl->get_ItemContainerGenerator()->ContainerFromIndex(index));
            if (pItem != NULL && pItem->get_IsEnabled() && pItem->Focus())
            {
                return true;
            }
        }
        pItem = object_cast<TreeViewItem>(pParentItemsControl);
        if (pItem != NULL)
        {
            return pItem->FocusDown();
        }
    }
    return false;
}

bool TreeViewItem::FocusIntoItem(TreeViewItem* pItem)
{
    TreeViewItem* pItem2 = NULL;
    int index = -1;
    TreeViewItem* pItem3 = NULL;
    while (pItem != NULL)
    {
        if (pItem->get_IsEnabled())
        {
            if (!pItem->get_IsExpanded() || !pItem->get_CanExpand())
            {
                return pItem->Focus();
            }
            pItem2 = pItem;
            pItem3 = pItem;
            index = pItem->get_Items()->get_Count() - 1;
        }
        else
        {
            if (index <= 0)
            {
                break;
            }
            index--;
        }
        pItem = object_cast<TreeViewItem>(pItem3->get_ItemContainerGenerator()->ContainerFromIndex(index));
    }
    return pItem2 != NULL && pItem2->Focus();
}
 
void TreeViewItem::GetTopAndBottom(Visual* pParent, float* pfTop, float* pfBottom)
{
    UIElement* pHeaderElement = get_HeaderElement();
    if (pHeaderElement != NULL)
    {
        GetTopAndBottom(pHeaderElement, pParent, pfTop, pfBottom);
    }
    else
    {
        GetTopAndBottom(this, pParent, pfTop, pfBottom);
    }
}
 
void TreeViewItem::GetTopAndBottom(UIElement* pItem, Visual* pParent, float* pfTop, float* pfBottom)
{
    Point point;
    Point point2;
    Matrix transform = pItem->TransformToAncestor(pParent);

    *pfTop = (transform.Transform(Point(0.0f, 0.0f))).get_Y();
    *pfBottom = (transform.Transform(Point(0.0f, pItem->get_RenderSize().get_Height()))).get_Y();
}

void TreeViewItem::HandleBringIntoView(RequestBringIntoViewEventArgs* pArgs)
{
    for (TreeViewItem* pItem = get_ParentTreeViewItem(); pItem != NULL; pItem = pItem->get_ParentTreeViewItem())
    {
        if (!pItem->get_IsExpanded())
        {
            pItem->set_IsExpanded(true);
        }
    }
    if (pArgs->get_TargetRect().get_IsEmpty())
    {
        UIElement* pHeaderElement = get_HeaderElement();
        if (pHeaderElement != NULL)
        {
            pArgs->set_IsHandled(true);
            pHeaderElement->BringIntoView();
        }
    }
}
 
bool TreeViewItem::HandleDownKey()
{
    if (AllowHandleKeyEvent(FocusNavigationDirection::Down))
    {
        if (!get_IsExpanded() || !get_CanExpand())
        {
            return FocusDown();
        }
        TreeViewItem* pItem = object_cast<TreeViewItem>(get_ItemContainerGenerator()->ContainerFromIndex(0));
        if (pItem != NULL)
        {
            if (pItem->get_IsEnabled())
            {
                return pItem->Focus();
            }
            pItem->FocusDown();
        }
    }
    return false;
}

bool TreeViewItem::HandleScrollByPage(bool up, ScrollViewer* pScroller, float viewportHeight, float startTop, float startBottom, float* pfCurrentDelta)
{
    float num;
    *pfCurrentDelta = CalculateDelta(up, this, pScroller, startTop, startBottom, &num);
    if (Float::GreaterThan(num, viewportHeight))
    {
        return false;
    }
    if (Float::LessThanOrClose(*pfCurrentDelta, viewportHeight))
    {
        return false;
    }

    bool flag = false;
    UIElement* pHeaderElement = get_HeaderElement();
    if (pHeaderElement != NULL && Float::LessThanOrClose(CalculateDelta(up, pHeaderElement, pScroller, startTop, startBottom), viewportHeight))
    {
        flag = true;
    }
    TreeViewItem* pItem = NULL;
    int count = get_Items()->get_Count();
    bool flag2 = up && get_ContainsSelection();
    for (int i = up ? (count - 1) : 0; (0 <= i) && (i < count); i += up ? -1 : 1)
    {
        TreeViewItem* pItem2 = object_cast<TreeViewItem>(get_ItemContainerGenerator()->ContainerFromIndex(i));
        if (pItem2 != NULL && pItem2->get_IsEnabled())
        {
            float num5;
            if (flag2)
            {
                if (pItem2->get_IsSelected())
                {
                    flag2 = false;
                    continue;
                }
                if (!pItem2->get_ContainsSelection())
                {
                    continue;
                }
                flag2 = false;
            }
            if (pItem2->HandleScrollByPage(up, pScroller, viewportHeight, startTop, startBottom, &num5))
            {
                return true;
            }
            if (Float::GreaterThan(num5, viewportHeight))
            {
                break;
            }
            pItem = pItem2;
        }
    }
    if (pItem != NULL)
    {
        if (up)
        {
            return pItem->Focus();
        }
        return FocusIntoItem(pItem);
    }
    return (flag && Focus());
}
 
bool TreeViewItem::HandleUpKey()
{
    if (AllowHandleKeyEvent(FocusNavigationDirection::Up))
    {
        ItemsControl* pParentItemsControl = get_ParentItemsControl();
        if (pParentItemsControl != NULL)
        {
            int index = pParentItemsControl->get_ItemContainerGenerator()->IndexFromContainer(this);
            while (index > 0)
            {
                index--;
                TreeViewItem* pItem = object_cast<TreeViewItem>(pParentItemsControl->get_ItemContainerGenerator()->ContainerFromIndex(index));
                if (pItem != NULL && pItem->get_IsEnabled())
                {
                    return FocusIntoItem(pItem);
                }
            }
            if (index == 0)
            {
                if (pParentItemsControl != get_ParentTreeView())
                {
                    return pParentItemsControl->Focus();
                }
                return true;
            }
        }
    }
    return false;
}

bool TreeViewItem::LogicalLeft(Key::Enum key)
{
    bool flag = get_FlowDirection() == FlowDirection::RightToLeft;
    return ((!flag && (key == Key::Left)) || (flag && (key == Key::Right)));
}

void TreeViewItem::OnIsExpandedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& args)
{
    TreeViewItem* pCollapsed = (TreeViewItem*) pDO;
    bool newValue = UnboxValue<bool>(args.pNewValue);
    if (!newValue)
    {
        TreeView* pParentTreeView = pCollapsed->get_ParentTreeView();
        if (pParentTreeView != NULL)
        {
            pParentTreeView->HandleSelectionAndCollapsed(pCollapsed);
        }
    }
    if (newValue)
    {
        RoutedEventArgs expandedArgs(&get_ExpandedEvent(), pCollapsed);
        pCollapsed->OnExpanded(&expandedArgs);
    }
    else
    {
        RoutedEventArgs collapsedArgs(&get_CollapsedEvent(), pCollapsed);
        pCollapsed->OnCollapsed(&collapsedArgs);
    }
}

void TreeViewItem::OnIsSelectedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& args)
{
    TreeViewItem* pElement = (TreeViewItem*) pDO;
    bool newValue = UnboxValue<bool>(args.pNewValue);

    pElement->Select(newValue);

    if (newValue)
    {
        RoutedEventArgs selectedArgs(&get_SelectedEvent(), pElement);
        pElement->OnSelected(&selectedArgs);
    }
    else
    {
        RoutedEventArgs unselectedArgs(&get_UnselectedEvent(), pElement);
        pElement->OnUnselected(&unselectedArgs);
    }
}

void TreeViewItem::OnMouseButtonDown(Object* pSender, MouseButtonEventArgs* pArgs)
{
    TreeViewItem* pItem = (TreeViewItem*) pSender;
    TreeView* pParentTreeView = pItem->get_ParentTreeView();
    if (pParentTreeView != NULL)
    {
        pParentTreeView->HandleMouseButtonDown();
    }
}
 
void TreeViewItem::OnRequestBringIntoView(Object* pSender, RequestBringIntoViewEventArgs* pArgs)
{
    if (pArgs->get_Target() == pSender)
    {
        ((TreeViewItem*) pSender)->HandleBringIntoView(pArgs);
    }
}

void TreeViewItem::Select(bool selected)
{
    TreeView* pParentTreeView = get_ParentTreeView();
    ItemsControl* pParentItemsControl = get_ParentItemsControl();
    if (pParentTreeView != NULL && pParentItemsControl != NULL && !pParentTreeView->get_IsSelectionChangeActive())
    {
        Object* pItemOrContainerFromContainer = pParentItemsControl->GetItemOrContainerFromContainer(this);
        pParentTreeView->ChangeSelection(pItemOrContainerFromContainer, this, selected);
    }
}
 
void TreeViewItem::UpdateContainsSelection(bool selected)
{
    for (TreeViewItem* pItem = get_ParentTreeViewItem(); pItem != NULL; pItem = pItem->get_ParentTreeViewItem())
    {
        pItem->set_ContainsSelection(selected);
    }
}

 

 

 
 
 
 

 
 

 



}; // namespace AVUI


