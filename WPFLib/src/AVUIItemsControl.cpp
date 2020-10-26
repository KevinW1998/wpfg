#include <AVUICommon.h>
#include <AVUIItemsControl.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemCollection.h>
#include <AVUIGroupItem.h>
#include <AVUIPanel.h>
#include <AVUIItemsPresenter.h>
#include <AVUIStyleSelector.h>
#include <AVUIStyle.h>
#include <AVUIContentControl.h>
#include <AVUIContentPresenter.h>
#include <AVUIDataTemplateSelector.h>
#include <AVUIDataTemplate.h>
#include <AVUIHelper.h>
#include <AVUIBindingOperations.h>
#include <AVUIScrollViewer.h>
#include <AVUIInputManager.h>
#include <AVUIHeaderedContentControl.h>
#include <AVUIHeaderedItemsControl.h>

REGISTER_ELEMENT_BEGIN(AVUI::ItemsControl)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(ItemsSource)
    REGISTER_PROPERTY(ItemTemplate)
    REGISTER_PROPERTY(ItemTemplateSelector)
    REGISTER_PROPERTY(ItemContainerStyle)
    REGISTER_PROPERTY(ItemContainerStyleSelector)
    REGISTER_PROPERTY(ItemsPanel)
    REGISTER_PROPERTY(HasItems)
    REGISTER_PROPERTY(IsGrouping)
    REGISTER_PROPERTY(Items)
REGISTER_ELEMENT_END()


namespace AVUI {


ItemCollection* ItemsControl::get_Items()
{
    if (m_pItems == NULL)
    {
        CreateItemCollectionAndGenerator();
    }
    return m_pItems;
}

ItemsControl* ItemsControl::ItemsControlFromItemContainer(DependencyObject* pContainer)
{
    UIElement* pCurrent = object_cast<UIElement>(pContainer);
    if (pCurrent == NULL)
    {
        return NULL;
    }
    ItemsControl* pControl = object_cast<ItemsControl>(pCurrent->get_Parent());
    if (pControl != NULL)
    {
        if (pControl->IsItemItsOwnContainerOverride(pCurrent))
        {
            return pControl;
        }
        return NULL;
    }
    pCurrent = object_cast<UIElement>(pCurrent->get_VisualParent());
    return GetItemsOwner(pCurrent);
}

void ItemsControl::CreateItemCollectionAndGenerator()
{
    m_pItems = object_allocate<ItemCollection>(this);
    m_pItemContainerGenerator = object_allocate<ItemContainerGenerator>(this);

    m_pItems->add_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &ItemsControl::OnItemCollectionChanged));

    if (get_IsInitPending())
    {
        m_pItems->BeginInit();
    }
    else if (get_IsInitialized())
    {
        m_pItems->BeginInit();
        m_pItems->EndInit();
    }

//    this._groupStyle.CollectionChanged += new NotifyCollectionChangedEventHandler(this.OnGroupStyleChanged);
}

void ItemsControl::ClearContainerForItem(DependencyObject* pContainer, Object* pItem)
{
    if (!(pContainer->IsType(GET_TYPE(GroupItem))))
    {
        ClearContainerForItemOverride(pContainer, pItem);
    }
}

ItemsControl* ItemsControl::GetItemsOwner(DependencyObject* pElement)
{
    Panel* pPanel = object_cast<Panel>(pElement);
    if(pPanel == NULL || !pPanel->get_IsItemsHost())
    {
        return NULL;
    }

    ItemsPresenter* pPresenter = ItemsPresenter::FromPanel(pPanel);
    if(pPresenter != NULL)
    {
        return pPresenter->get_Owner();
    }

    return object_cast<ItemsControl>(pPanel->get_TemplatedParent());
}

void ItemsControl::PrepareItemContainer(DependencyObject* pContainer, Object* pItem)
{
    GroupItem* pGroupItem = object_cast<GroupItem>(pContainer);
    if (pGroupItem != NULL)
    {
        pGroupItem->PrepareItemContainer(pItem);
    }
    else
    {
        if (ShouldApplyItemContainerStyle(pContainer, pItem))
        {
            ApplyItemContainerStyle(pContainer, pItem);
        }
        PrepareContainerForItemOverride(pContainer, pItem);
    }
}

void ItemsControl::ApplyItemContainerStyle(DependencyObject* pContainer, Object* pItem)
{
    UIElement* pUIElement = object_cast<UIElement>(pContainer);

    if(pUIElement == NULL)
    {
        LibraryCriticalError();
    }

    if (pUIElement->get_IsStyleSetFromGenerator() || (pContainer->ReadLocalValue(UIElement::get_StyleProperty()) == DependencyProperty::get_UnsetValue()))
    {
        TRefCountedPtr<Style> pItemContainerStyle = get_ItemContainerStyle();
        if (pItemContainerStyle == NULL && get_ItemContainerStyleSelector() != NULL)
        {
            pItemContainerStyle = get_ItemContainerStyleSelector()->SelectStyle(pItem, pContainer);
        }
        if (pItemContainerStyle != NULL)
        {
            if (!pContainer->IsType(*(pItemContainerStyle->get_TargetType())))
            {
                LibraryCriticalError();
            }
            pUIElement->set_Style(pItemContainerStyle);
            pUIElement->set_IsStyleSetFromGenerator(true);
        }
        else if (pUIElement->get_IsStyleSetFromGenerator())
        {
            pUIElement->set_IsStyleSetFromGenerator(false);
            pContainer->ClearValue(UIElement::get_StyleProperty());
        }
    }
}

void ItemsControl::PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem)
{
    HeaderedContentControl* pControl = object_cast<HeaderedContentControl>(pElement);
    if (pControl != NULL)
    {
        pControl->PrepareHeaderedContentControl(pItem, get_ItemTemplate(), get_ItemTemplateSelector());
        return;
    }
      
    ContentControl* pControl2 = object_cast<ContentControl>(pElement);
    if (pControl2 != NULL)
    {
        pControl2->PrepareContentControl(pItem, get_ItemTemplate(), get_ItemTemplateSelector());
        return;
    }

    ContentPresenter* pPresenter = object_cast<ContentPresenter>(pElement);
    if (pPresenter != NULL)
    {
        pPresenter->PrepareContentPresenter(pItem, get_ItemTemplate(), get_ItemTemplateSelector());
        return;
    }

    HeaderedItemsControl* pControl4 = object_cast<HeaderedItemsControl>(pElement);
    if (pControl4 != NULL)
    {
        pControl4->PrepareHeaderedItemsControl(pItem, get_ItemTemplate(), get_ItemTemplateSelector(), get_ItemContainerStyle(), get_ItemContainerStyleSelector());
        return;
    }

    ItemsControl* pControl3 = object_cast<ItemsControl>(pElement);;

    if (pControl3 != this)
    {
        pControl3->PrepareItemsControl(pItem, get_ItemTemplate(), get_ItemTemplateSelector(), get_ItemContainerStyle(), get_ItemContainerStyleSelector());
    }
}

void ItemsControl::PrepareItemsControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector, Style* pItemContainerStyle, StyleSelector* pItemContainerStyleSelector)
{
    if (pItem == this)
    {
        return;
    }

    if (pItemTemplate != NULL)
    {
        SetValue(get_ItemTemplateProperty(), pItemTemplate);
    }
    if (pItemTemplateSelector != NULL)
    {
        SetValue(get_ItemTemplateSelectorProperty(), pItemTemplateSelector);
    }
    if (pItemContainerStyle != NULL && Helper::HasDefaultValue(get_ItemContainerStyleProperty(), this, this))
    {
        SetValue(get_ItemContainerStyleProperty(), pItemContainerStyle);
    }
    if (pItemContainerStyleSelector != NULL && Helper::HasDefaultValue(get_ItemContainerStyleSelectorProperty(), this, this))
    {
        SetValue(get_ItemContainerStyleSelectorProperty(), pItemContainerStyleSelector);
    }
}



TRefCountedPtr<DependencyObject> ItemsControl::GetContainerForItem(Object* pItem)
{
    TRefCountedPtr<DependencyObject> pContainerForItemOverride;
    if (IsItemItsOwnContainerOverride(pItem))
    {
        pContainerForItemOverride = object_cast<DependencyObject>(pItem);
    }
    else
    {
        pContainerForItemOverride = GetContainerForItemOverride();
        if(pContainerForItemOverride != NULL)
        {
            ContentPresenter::BindToBasicFontProperties(this, pContainerForItemOverride);
        }
    }

    Visual* pReference = object_cast<Visual>(pContainerForItemOverride);
    if (pReference != NULL)
    {
        Visual* pParent = pReference->get_VisualParent();
        if (pParent == NULL)
        {
            return pContainerForItemOverride;
        }

        if(!pParent->IsType(GET_TYPE(UIElement)))
        {
            LibraryCriticalError();
        }

        Panel* pPanel = object_cast<Panel>(pParent);
        if (pPanel != NULL && pReference->IsType(GET_TYPE(UIElement)))
        {
            // TODO -- ToolBarPanel
            /*
            ToolBarPanel panel2 = panel as ToolBarPanel;
            if (panel2 != null)
            {
                panel2.UIElementCollection.RemoveNoVerify((UIElement) reference);
                return containerForItemOverride;
            }
            */

            pPanel->get_Children()->Remove(object_cast<UIElement>(pReference));
            return pContainerForItemOverride;
        }
        // TODO -- How does this work?
        //((FrameworkElement) pReference).TemplateChild = null;
    }
    return pContainerForItemOverride;
}


bool ItemsControl::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem->IsType(GET_TYPE(UIElement));
}

TRefCountedPtr<DependencyObject> ItemsControl::GetContainerForItemOverride()
{
    return (DependencyObject*) object_allocate<ContentPresenter>();
} 

 
bool ItemsControl::IsHostForItemContainer(DependencyObject* pContainer)
{
    ItemsControl* pControl = ItemsControlFromItemContainer(pContainer);
    if (pControl != NULL)
    {
        return (pControl == this);
    }

    return get_Items()->Contains(pContainer);
}


Object* ItemsControl::GetItemOrContainerFromContainer(DependencyObject* pContainer)
{
    Object* pItem = get_ItemContainerGenerator()->ItemFromContainer(pContainer);
    if (pItem == DependencyProperty::get_UnsetValue() && ItemsControlFromItemContainer(pContainer) == this && IsItemItsOwnContainerOverride(pContainer))
    {
        pItem = pContainer;
    }
    return pItem;
}


void ItemsControl::OnItemsSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ItemsControl* pControl = (ItemsControl*) pDO;
    if (e.pNewValue == NULL && !BindingOperations::IsDataBound(pControl, &get_ItemsSourceProperty()))
    {
        pControl->get_Items()->ClearItemsSource();
    }
    else
    {
        pControl->get_Items()->SetItemsSource(e.pNewValue);
    }

    pControl->OnItemsSourceChanged(e.pOldValue, e.pNewValue);
}

 
void ItemsControl::OnItemCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{
    set_HasItems(m_pItems != NULL && !m_pItems->get_IsEmpty());

    if ((pArgs->get_Action() == NotifyCollectionChangedAction::Remove && 
        m_pFocusedItem != NULL && m_pFocusedItem->Equals(pArgs->get_OldItem())) || 
        (pArgs->get_Action() == NotifyCollectionChangedAction::Reset))
    {
        m_pFocusedItem = NULL;
    }
    OnItemsChanged(pArgs);
}

 
void ItemsControl::NavigateByPage(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs)
{
    if (get_ItemsHost() != NULL)
    {
        if (pStartingItem != NULL && !IsOnCurrentPage(pStartingItem, direction))
        {
            while (MakeVisible(get_Items()->IndexOf(pStartingItem)))
            {
                float horizontalOffset = get_ScrollHost()->get_HorizontalOffset();
                float verticalOffset = get_ScrollHost()->get_VerticalOffset();
                get_ItemsHost()->UpdateLayout();
                if (Float::AreClose(horizontalOffset, get_ScrollHost()->get_HorizontalOffset()) && Float::AreClose(verticalOffset, get_ScrollHost()->get_VerticalOffset()))
                {
                    break;
                }
            }
        }
        NavigateByPageInternal(pStartingItem, direction, pItemNavigateArgs);
    }
}

void ItemsControl::NavigateByPageInternal(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs)
{
    if (pStartingItem == NULL)
    {
        NavigateToFirstItemOnCurrentPage(pStartingItem, direction, pItemNavigateArgs);
    }
    else
    {
        int num;
        Object* pObj2 = GetFirstItemOnCurrentPage(pStartingItem, direction, &num);
        if (!pStartingItem->Equals(pObj2))
        {
            if (pObj2 != DependencyProperty::get_UnsetValue())
            {
                NavigateToItem(pObj2, num, pItemNavigateArgs);
            }
        }
        else
        {
            bool flag = false;
            if (get_ScrollHost() != NULL)
            {
                switch (direction)
                {
                case FocusNavigationDirection::Up:
                    if (!get_IsLogicalHorizontal())
                    {
                        get_ScrollHost()->PageUp();
                    }
                    else
                    {
                        get_ScrollHost()->PageLeft();
                    }
                    flag = true;
                    break;

                case FocusNavigationDirection::Down:
                    if (!get_IsLogicalHorizontal())
                    {
                        get_ScrollHost()->PageDown();
                    }
                    else
                    {
                        get_ScrollHost()->PageRight();
                    }
                    flag = true;
                    break;
                }
            }
            if (flag && get_ItemsHost() != NULL)
            {
                get_ItemsHost()->UpdateLayout();
                NavigateToFirstItemOnCurrentPage(pStartingItem, direction, pItemNavigateArgs);
            }
        }
    }
}

void ItemsControl::NavigateToFirstItemOnCurrentPage(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs)
{
    int num;
    Object* pItem = GetFirstItemOnCurrentPage(pStartingItem, direction, &num);
    if (pItem != DependencyProperty::get_UnsetValue())
    {
        FocusItem(pItem, pItemNavigateArgs);
    }
}
 
Object* ItemsControl::GetFirstItemOnCurrentPage(Object* pStartingItem, FocusNavigationDirection::Enum direction, int* pFoundIndex)
{
    *pFoundIndex = -1;
    if (get_IsLogicalVertical())
    {
        if (direction == FocusNavigationDirection::Up)
        {
            return FindFocusable((int) get_ScrollHost()->get_VerticalOffset(), 1, pFoundIndex);
        }
        return FindFocusable((int) ((get_ScrollHost()->get_VerticalOffset() + get_ScrollHost()->get_ViewportHeight()) - 1.0f), -1, pFoundIndex);
    }
    if (get_IsLogicalHorizontal())
    {
        if (direction == FocusNavigationDirection::Up)
        {
            return FindFocusable((int) get_ScrollHost()->get_HorizontalOffset(), 1, pFoundIndex);
        }
        return FindFocusable((int) ((get_ScrollHost()->get_HorizontalOffset() + get_ScrollHost()->get_ViewportWidth()) - 1.0f), -1, pFoundIndex);
    }

    UIElement* pElement = object_cast<UIElement>(get_ItemContainerGenerator()->ContainerFromItem(pStartingItem));
    UIElement* pElement2 = pElement;
    UIElement* pElement3 = NULL;
    if (pElement == NULL)
    {
        return NULL;
    }
    while (pElement2 != NULL && !IsOnCurrentPage(pElement2, direction))
    {
        pElement3 = pElement2;
        pElement2 = KeyboardNavigation::get_Current()->PredictFocusedElement(pElement2, direction);
    }
    while (pElement2 != NULL && IsOnCurrentPage(pElement2, direction))
    {
        pElement3 = pElement2;
        pElement2 = KeyboardNavigation::get_Current()->PredictFocusedElement(pElement2, direction);
    }
    return GetEncapsulatingItem(pElement3);
}


void ItemsControl::NavigateToEnd(ItemNavigateArgs* pItemNavigateArgs)
{
    if (get_HasItems())
    {
        int num;
        Object* pItem = FindFocusable(get_Items()->get_Count() - 1, -1, &num);
        NavigateToItem(pItem, num, pItemNavigateArgs);
    }
}

void ItemsControl::NavigateToStart(ItemNavigateArgs* pItemNavigateArgs)
{
    if (get_HasItems())
    {
        int num;
        Object* pItem = FindFocusable(0, 1, &num);
        NavigateToItem(pItem, num, pItemNavigateArgs);
    }
}

void ItemsControl::NavigateByLine(FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs)
{
    NavigateByLine(get_FocusedItem(), direction, pItemNavigateArgs);
}


void ItemsControl::NavigateByLine(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs)
{
    if (get_ItemsHost() != NULL)
    {
        if (pStartingItem != NULL && !IsOnCurrentPage(pStartingItem, direction))
        {
            MakeVisible(get_Items()->IndexOf(pStartingItem));
            get_ItemsHost()->UpdateLayout();
        }
        NavigateByLineInternal(pStartingItem, direction, pItemNavigateArgs);
    }
}

 

ScrollViewer* ItemsControl::get_ScrollHost()
{
    if (!m_isScrollHostValid)
    {
        if (m_pItemsHost == NULL)
        {
            return NULL;
        }
        for (Visual* pObj2 = m_pItemsHost; pObj2 != this && pObj2 != NULL; pObj2 = pObj2->get_VisualParent())
        {
            ScrollViewer* pViewer = object_cast<ScrollViewer>(pObj2);
            if (pViewer != NULL)
            {
                m_pScrollHost = pViewer;
                break;
            }
        }
        m_isScrollHostValid = true;
    }
    return m_pScrollHost;
}

bool ItemsControl::IsOnCurrentPage(Object* pItem, FocusNavigationDirection::Enum axis)
{
    UIElement* pElement = object_cast<UIElement>(get_ItemContainerGenerator()->ContainerFromItem(pItem));
    if (pElement == NULL)
    {
        return false;
    }
    return IsOnCurrentPage(pElement, axis, false);
}

bool ItemsControl::IsOnCurrentPage(UIElement* pElement, FocusNavigationDirection::Enum axis)
{
    return IsOnCurrentPage(pElement, axis, false);
}

bool ItemsControl::IsOnCurrentPage(UIElement* pElement, FocusNavigationDirection::Enum axis, bool fullyVisible)
{
    UIElement* pScrollHost = get_ScrollHost();
    if (pScrollHost == NULL)
    {
        pScrollHost = get_ItemsHost();
    }
    if (pScrollHost != NULL)
    {
        if (pElement == NULL || !pScrollHost->IsAncestorOf(pElement))
        {
            return false;
        }
        Rect rect(Point(), pScrollHost->get_RenderSize());
        Rect rect2(Point(), pElement->get_RenderSize());
        rect2 = pElement->TransformToAncestor(pScrollHost).TransformBounds(rect2);
        if (fullyVisible)
        {
            return rect.Contains(rect2);
        }
        if (axis == FocusNavigationDirection::Up || axis == FocusNavigationDirection::Down)
        {
            if (Float::LessThanOrClose(rect.get_Top(), rect2.get_Top()) && Float::LessThanOrClose(rect2.get_Bottom(), rect.get_Bottom()))
            {
                return true;
            }
        }
        else if (((axis == FocusNavigationDirection::Right) || (axis == FocusNavigationDirection::Left)) && (Float::LessThanOrClose(rect.get_Left(), rect2.get_Left()) && Float::LessThanOrClose(rect2.get_Right(), rect.get_Right())))
        {
            return true;
        }
    }
    return false;
}

bool ItemsControl::MakeVisible(int index, bool alwaysAtTopOfViewport, bool alignMinorAxisToo)
{
    if (index == -1)
    {
        return false;
    }
    if (get_ScrollHost() == NULL)
    {
        return false;
    }
    bool flag = false;
    float horizontalOffset = get_ScrollHost()->get_HorizontalOffset();
    float verticalOffset = get_ScrollHost()->get_VerticalOffset();
    float num3 = horizontalOffset;
    float num4 = verticalOffset;
    if (get_IsLogicalVertical())
    {
        if (alwaysAtTopOfViewport)
        {
            num4 = (float)index;
        }
        else
        {
            if (Float::GreaterThan((float) (index + 1), verticalOffset + get_ScrollHost()->get_ViewportHeight()))
            {
                num4 = Float::Max(0.0f, (float) ((index + 1) - get_ScrollHost()->get_ViewportHeight()));
            }
            if (Float::LessThan((float)index, verticalOffset))
            {
                num4 = (float)index;
            }
        }
        if (alignMinorAxisToo)
        {
            num3 = 0.0f;
        }
        if (!Float::AreClose(horizontalOffset, num3))
        {
            get_ScrollHost()->ScrollToHorizontalOffset(num3);
            flag = true;
        }
        if (!Float::AreClose(verticalOffset, num4))
        {
            get_ScrollHost()->ScrollToVerticalOffset(num4);
            flag = true;
        }
        return flag;
    }
    if (get_IsLogicalHorizontal())
    {
        if (alwaysAtTopOfViewport)
        {
            num3 = (float)index;
        }
        else
        {
            if (Float::GreaterThan((float) (index + 1), horizontalOffset + get_ScrollHost()->get_ViewportWidth()))
            {
                num3 = Float::Max(0.0f, (float) ((index + 1) - get_ScrollHost()->get_ViewportWidth()));
            }
            if (Float::LessThan((float) index, horizontalOffset))
            {
                num3 = (float)index;
            }
        }
        if (alignMinorAxisToo)
        {
            num4 = 0.0f;
        }
        if (!Float::AreClose(horizontalOffset, num3))
        {
            get_ScrollHost()->ScrollToHorizontalOffset(num3);
            flag = true;
        }
        if (!Float::AreClose(verticalOffset, num4))
        {
            get_ScrollHost()->ScrollToVerticalOffset(num4);
            flag = true;
        }
        return flag;
    }
    UIElement* pElement = object_cast<UIElement>(get_ItemContainerGenerator()->ContainerFromIndex(index));
    if (pElement != NULL)
    {
        pElement->BringIntoView();
        flag = !Float::AreClose(horizontalOffset, get_ScrollHost()->get_HorizontalOffset()) || !Float::AreClose(verticalOffset, get_ScrollHost()->get_VerticalOffset());
    }
    return flag;
}


bool ItemsControl::get_IsItemsHostLogicalVertical()
{
    if(get_ItemsHost() == NULL)
    {
        return false;
    }

    if(!get_ItemsHost()->get_HasLogicalOrientation())
    {
        return false;
    }

    if(get_ItemsHost()->get_LogicalOrientation() != Orientation::Vertical)
    {
        return false;
    }

    return true;
}
 
bool ItemsControl::get_IsItemsHostLogicalHorizontal()
{
    if(get_ItemsHost() == NULL)
    {
        return false;
    }

    if(!get_ItemsHost()->get_HasLogicalOrientation())
    {
        return false;
    }

    if(get_ItemsHost()->get_LogicalOrientation() != Orientation::Horizontal)
    {
        return false;
    }

    return true;
}


bool ItemsControl::get_IsLogicalVertical()
{
    if(!get_IsItemsHostLogicalVertical())
    {
        return false;
    }

    if(get_ScrollHost() == NULL)
    {
        return false;
    }

    return get_ScrollHost()->get_CanContentScroll();
}
 
bool ItemsControl::get_IsLogicalHorizontal()
{
    if(!get_IsItemsHostLogicalHorizontal())
    {
        return false;
    }

    if(get_ScrollHost() == NULL)
    {
        return false;
    }

    return get_ScrollHost()->get_CanContentScroll();
}

void ItemsControl::NavigateToItem(Object* pItem, int elementIndex, ItemNavigateArgs* pItemNavigateArgs, bool alwaysAtTopOfViewport)
{
    if(pItem == DependencyProperty::get_UnsetValue())
    {
        return;
    }

    if (elementIndex == -1)
    {
        elementIndex = get_Items()->IndexOf(pItem);
    }

    if (elementIndex == -1)
    {
        return;
    }

    while (MakeVisible(elementIndex, alwaysAtTopOfViewport, false))
    {
        float horizontalOffset = get_ScrollHost()->get_HorizontalOffset();
        float verticalOffset = get_ScrollHost()->get_VerticalOffset();
        get_ItemsHost()->UpdateLayout();
        if (Float::AreClose(horizontalOffset, get_ScrollHost()->get_HorizontalOffset()) && Float::AreClose(verticalOffset, get_ScrollHost()->get_VerticalOffset()))
        {
            break;
        }
    }
    FocusItem(pItem, pItemNavigateArgs);
}
 
void ItemsControl::FocusItem(Object* pItem, ItemNavigateArgs* pItemNavigateArgs)
{
    if (pItem != NULL)
    {
        UIElement* pElement = object_cast<UIElement>(get_ItemContainerGenerator()->ContainerFromItem(pItem));
        if (pElement != NULL)
        {
            get_InputManager()->set_KeyboardFocus(object_cast<Control>(pElement));
        }
    }
    /*
    if (itemNavigateArgs.DeviceUsed is KeyboardDevice)
    {
        KeyboardNavigation.ShowFocusVisual();
    }
    */
}

Object* ItemsControl::FindFocusable(int startIndex, int direction, int* pFoundIndex)
{
    if (get_HasItems())
    {
        int count = get_Items()->get_Count();
        while ((startIndex >= 0) && (startIndex < count))
        {
            Control* pControl = object_cast<Control>(get_ItemContainerGenerator()->ContainerFromIndex(startIndex));
            if (pControl == NULL || Keyboard::IsFocusable(pControl))
            {
                *pFoundIndex = startIndex;
                return get_Items()->GetItemAt(startIndex);
            }
            startIndex += direction;
        }
    }
    *pFoundIndex = -1;
    return NULL;
}

Object* ItemsControl::GetEncapsulatingItem(UIElement* pElement)
{
    Object* pUnsetValue = DependencyProperty::get_UnsetValue();
    while (pUnsetValue == DependencyProperty::get_UnsetValue() && pElement != NULL)
    {
        pUnsetValue = get_ItemContainerGenerator()->ItemFromContainer(pElement);
        pElement = object_cast<UIElement>(pElement->get_VisualParent());
    }
    return pUnsetValue;
}
 
void ItemsControl::NavigateByLineInternal(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs)
{
    if (pStartingItem == NULL)
    {
        NavigateToStart(pItemNavigateArgs);
    }
    else
    {
        UIElement* pDescendant = NULL;
        UIElement* pElement2 = NULL;
        pDescendant = object_cast<UIElement>(get_ItemContainerGenerator()->ContainerFromItem(pStartingItem));
        if (pDescendant == NULL || !get_ItemsHost()->IsAncestorOf(pDescendant))
        {
            pDescendant = get_ScrollHost();
        }
        pElement2 = object_cast<UIElement>(KeyboardNavigation::get_Current()->PredictFocusedElement(pDescendant, direction));
        if (pElement2 != NULL && get_ItemsHost()->IsAncestorOf(pElement2))
        {
            Object* pEncapsulatingItem = GetEncapsulatingItem(pElement2);
            if (pEncapsulatingItem != DependencyProperty::get_UnsetValue())
            {
                NavigateToItem(pEncapsulatingItem, pItemNavigateArgs);
            }
        }
    }
}



}; // namespace AVUI
