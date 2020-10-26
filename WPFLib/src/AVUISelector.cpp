#include <AVUICommon.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemCollection.h>
#include <AVUISelector.h>
#include <AVUISeparator.h>
#include <AVUIInputManager.h>

REGISTER_ELEMENT_BEGIN(AVUI::Selector)


    REGISTER_PROPERTY(IsSelected)
    REGISTER_PROPERTY(IsSelectionActive)
    REGISTER_PROPERTY(IsSynchronizedWithCurrentItem)
    REGISTER_PROPERTY(SelectedIndex)
    REGISTER_PROPERTY(SelectedItem)
    REGISTER_PROPERTY(SelectedItemsImpl)

    REGISTER_ROUTED_EVENT(SelectionChanged)
    REGISTER_ROUTED_EVENT(Selected)
    REGISTER_ROUTED_EVENT(Unselected)

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor);

REGISTER_ELEMENT_END()


REGISTER_ELEMENT(AVUI::SelectionChangedEventArgs);

namespace AVUI
{

void Selector::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(Selector), get_SelectedEvent(), RoutedEventHandler::Create(OnSelected), false);
    EventManager::RegisterClassHandler(GET_TYPE(Selector), get_UnselectedEvent(), RoutedEventHandler::Create(OnUnselected), false);
}

Selector::Selector() : m_canSelectMultiple(false), m_isSelectingSingleItem(false), m_skipCoerceSelectedItemCheck(false), m_isSettingCurrentToSelected(false), m_isSynchronizedWithCurrentItemPrivate(false)
{
   get_ItemContainerGenerator()->add_StatusChanged(EventHandler::Create(this, &Selector::OnGeneratorStatusChanged));
}

Selector::~Selector()
{
   get_ItemContainerGenerator()->remove_StatusChanged(EventHandler::Create(this, &Selector::OnGeneratorStatusChanged));
}


bool Selector::UiGetIsSelectable(DependencyObject* pDO)
{
    if (pDO != NULL)
    {
        if (!ItemGetIsSelectable(pDO))
        {
            return false;
        }
        ItemsControl* pControl = ItemsControl::ItemsControlFromItemContainer(pDO);
        if (pControl != NULL)
        {
            Object* pItem = pControl->get_ItemContainerGenerator()->ItemFromContainer(pDO);
            if (pItem != pDO)
            {
                return ItemGetIsSelectable(pItem);
            }
            return true;
        }
    }
    return false;
}

bool Selector::ItemGetIsSelectable(Object* pItem)
{
    if (pItem != NULL)
    {
        return !pItem->IsType(GET_TYPE(Separator));
    }
    return false;
}

void Selector::set_CanSelectMultiple(bool canSelectMultiple)
{
    if(m_canSelectMultiple != canSelectMultiple)
    {
        m_canSelectMultiple = canSelectMultiple;
        if(!canSelectMultiple && m_selectedItems.get_Count() > 1)
        {
            get_SelectionChange()->Validate();
        }
    }
}

Selector::SelectionChanger* Selector::get_SelectionChange()
{
    if(m_pSelectionInstance == NULL)
    {
        m_pSelectionInstance = object_allocate<SelectionChanger>(this);
    }
    return m_pSelectionInstance;

}

bool Selector::GetIsSelected(DependencyObject* pElement)
{
    if (pElement == NULL)
    {
        LibraryCriticalError();
    }
    return UnboxValue<bool>(pElement->GetValue(get_IsSelectedProperty()));
}

 

 



void Selector::ItemSetIsSelected(Object* pItem, bool value)
{
    if (pItem != NULL)
    {
        DependencyObject* pElement = get_ItemContainerGenerator()->ContainerFromItem(pItem);
        if (pElement != NULL)
        {
            if (GetIsSelected(pElement) != value)
            {
                pElement->SetValue(get_IsSelectedProperty(), BoxValue(value));
            }
        }
        else
        {
            DependencyObject* pDO = object_cast<DependencyObject>(pItem);
            if (pDO != NULL && GetIsSelected(pDO) != value)
            {
                pDO->SetValue(get_IsSelectedProperty(), BoxValue(value));
            }
        }
    }
}

 

void Selector::UpdatePublicSelectionProperties()
{
    // Bunch of deferred reference gobbledygook deleted here
    // Ughity ugh ugh -- replaced with this, noncompliant
    if(get_SelectedIndex() != get_InternalSelectedIndex())
    {
        SetValue(get_SelectedIndexProperty(), BoxValue(get_InternalSelectedIndex()));
    }

    if (get_SelectedItem() != get_InternalSelectedItem())
    {
        m_skipCoerceSelectedItemCheck = true;
        SetValue(get_SelectedItemProperty(), get_InternalSelectedItem());
        m_skipCoerceSelectedItemCheck = false;
    }

    UpdateSelectedItems();
}


void Selector::UpdateSelectedItems()
{
    Object* pSelectedItemsImpl = get_SelectedItemsImpl();
    ICollection* pSelectedItems = interface_cast<ICollection>(pSelectedItemsImpl);

    if (pSelectedItems != NULL)
    {
        ItemStructList<ObjectPtr> storage(pSelectedItems->get_Count());

        for (int i = 0; i < pSelectedItems->get_Count(); i++)
        {
            Object* pT = pSelectedItems->GetItemAt(i);
            if (m_selectedItems.Contains(pT) && !storage.Contains(pT))
            {
                storage.AddItem(pT);
            }
            else
            {
                pSelectedItems->RemoveAt(i);
                i--;
            }
        }

        for(int i = 0; i < m_selectedItems.get_Count(); i++)
        {
            Object* pT = m_selectedItems[i];

            if (!storage.Contains(pT))
            {
                pSelectedItems->Add(pT);
            }
        }
    }
}


void Selector::SetCurrentToSelected()
{
    if (!m_isSettingCurrentToSelected)
    {
        m_isSettingCurrentToSelected = true;
        if (m_selectedItems.get_Count() == 0)
        {
            get_Items()->MoveCurrentToPosition(-1);
        }
        else
        {
            get_Items()->MoveCurrentTo(get_InternalSelectedItem());
        }
        m_isSettingCurrentToSelected = false;
    }
}

void Selector::InvokeSelectionChanged(IEnumerable* pUnselectedItems, IEnumerable* pSelectedItems)
{
    SelectionChangedEventArgs e(pUnselectedItems, pSelectedItems);
    e.set_Source(this);

    OnSelectionChanged(&e);
}

void Selector::OnSelectionChanged(SelectionChangedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void Selector::OnSelected(Object* pSender, RoutedEventArgs* pArgs)
{
   ((Selector*) pSender)->NotifyIsSelectedChanged(object_cast<UIElement>(pArgs->get_OriginalSource()), true, pArgs);
}

void Selector::OnUnselected(Object* pSender, RoutedEventArgs* pArgs)
{
    ((Selector*) pSender)->NotifyIsSelectedChanged(object_cast<UIElement>(pArgs->get_OriginalSource()), false, pArgs);
}
 

void Selector::NotifyIsSelectedChanged(UIElement* pContainer, bool selected, RoutedEventArgs* pArgs)
{
    if (!get_SelectionChange()->get_IsActive())
    {
        if (pContainer != NULL)
        {
            Object* pItem = GetItemOrContainerFromContainer(pContainer);
            if (pItem != DependencyProperty::get_UnsetValue())
            {
                SetSelectedHelper(pItem, pContainer, selected);
                pArgs->set_IsHandled(true);
            }
        }
    }
    else
    {
        pArgs->set_IsHandled(true);
    }
}

 
void Selector::SetSelectedHelper(Object* pItem, UIElement* pContainer, bool selected)
{
    if (!ItemGetIsSelectable(pItem) && selected)
    {
        LibraryCriticalError();
    }
    get_SelectionChange()->Begin();
    if (selected)
    {
        get_SelectionChange()->Select(pItem, true);
    }
    else
    {
        get_SelectionChange()->Unselect(pItem);
    }
    get_SelectionChange()->End();
}

 

void Selector::SetInitialMousePosition()
{
    m_lastMousePosition = get_InputManager()->get_MousePosition(this);
}
 
void Selector::ResetLastMousePosition()
{
    m_lastMousePosition = Point();
}

void Selector::OnGeneratorStatusChanged(Object* pSender, EventArgs* pArgs)
{
    if(get_ItemContainerGenerator()->get_Status() == GeneratorStatus::ContainersGenerated && get_HasItems())
    {
        get_SelectionChange()->Begin();
        for (int i = 0; i < m_selectedItems.get_Count(); i++)
        {
            ItemSetIsSelected(m_selectedItems[i], true);
        }
        get_SelectionChange()->Cancel();
    }
}

int Selector::get_InternalSelectedIndex()
{
    if(m_selectedItems.get_Count() > 0)
    {
        return get_Items()->IndexOf(m_selectedItems[0]);
    }
    return -1;
}


ObjectPtr Selector::CoerceSelectedIndex(DependencyObject* pDO, Object* pValue)
{
    Selector* pSelector = (Selector*) pDO;

    if(pValue->IsType(GET_TYPE(int)) && UnboxValue<int>(pValue) >= pSelector->get_Items()->get_Count())
    {
        return DependencyProperty::get_UnsetValue();
    }

    return pValue;
}

void Selector::OnSelectedIndexChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    Selector* pSelector = (Selector*) pDO;
    if (!pSelector->get_SelectionChange()->get_IsActive())
    {
        int newValue = UnboxValue<int>(e.pNewValue);

        Object* pItem = (newValue == -1) ? NULL : pSelector->get_Items()->GetItemAt(newValue);

        pSelector->get_SelectionChange()->SelectJustThisItem(pItem, false);
    }
}

void Selector::OnItemsChanged(CollectionChangedEventArgs* pArgs)
{
   if ((pArgs->get_Action() == NotifyCollectionChangedAction::Reset) || ((pArgs->get_Action() == NotifyCollectionChangedAction::Add) && (pArgs->get_NewStartingIndex() == 0)))
    {
        //ResetSelectedItemsAlgorithm();
    }
    ItemsControl::OnItemsChanged(pArgs);

    EffectiveValueEntry entry = GetValueEntry(LookupEntry(get_SelectedIndexProperty().get_PropertyID()), get_SelectedIndexProperty(), NULL, RequestFlags::DeferredReferences);
    /*
    if (!entry.get_IsDeferredReference() || !(entry.get_Value()->IsType(GET_TYPE(DeferredSelectedIndexReference))))
    {
    */
        CoerceValue(get_SelectedIndexProperty());
    /*
    }
    */

    CoerceValue(get_SelectedItemProperty());

    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Add:
    {
        get_SelectionChange()->Begin();

        Object* pItem = pArgs->get_NewItem();
        if (ItemGetIsSelected(pItem))
        {
            get_SelectionChange()->Select(pItem, true);
        }
        get_SelectionChange()->End();

        return;
    }

    case NotifyCollectionChangedAction::Remove:
    case NotifyCollectionChangedAction::Replace:
            break;

    case NotifyCollectionChangedAction::Move:
        get_SelectionChange()->Validate();
        return;

    case NotifyCollectionChangedAction::Reset:
        if (get_Items()->get_IsEmpty())
        {
            get_SelectionChange()->CleanupDeferSelection();
        }
        if (get_Items()->get_CurrentItem() != NULL && m_isSynchronizedWithCurrentItemPrivate)
        {
            SetSelectedToCurrent();
            return;
        }
        get_SelectionChange()->Begin();
        for (int i = 0; i < m_selectedItems.get_Count(); i++)
        {
            Object* pObj4 = m_selectedItems[i];
            if (!get_Items()->Contains(pObj4))
            {
                get_SelectionChange()->Unselect(pObj4);
            }
        }
        if (get_ItemsSource() == NULL)
        {
            for (int j = 0; j < get_Items()->get_Count(); j++)
            {
                Object* pObj5 = get_Items()->GetItemAt(j);
                if (IndexGetIsSelected(j, pObj5) && !m_selectedItems.Contains(pObj5))
                {
                    get_SelectionChange()->Select(pObj5, true);
                }
            }
        }
        get_SelectionChange()->End();

        return;

    default:
        LibraryCriticalError();
    }

    get_SelectionChange()->Begin();
    Object* pT = pArgs->get_OldItem();
    if (m_selectedItems.Contains(pT))
    {
        get_SelectionChange()->Unselect(pT);
    }
    get_SelectionChange()->End();
}

bool Selector::ItemGetIsSelected(Object* pItem)
{
    if (pItem == NULL)
    {
        return false;
    }
    return ContainerGetIsSelected(get_ItemContainerGenerator()->ContainerFromItem(pItem), pItem);
}

bool Selector::IndexGetIsSelected(int index, Object* pItem)
{
    return ContainerGetIsSelected(get_ItemContainerGenerator()->ContainerFromIndex(index), pItem);
}
 
bool Selector::ContainerGetIsSelected(DependencyObject* pContainer, Object* pItem)
{
    if (pContainer != NULL)
    {
        return UnboxValue<bool>(pContainer->GetValue(get_IsSelectedProperty()));
    }

    DependencyObject* pObj2 = object_cast<DependencyObject>(pItem);
    return pObj2 != NULL && UnboxValue<bool>(pObj2->GetValue(get_IsSelectedProperty()));
}

 
void Selector::SetSelectedToCurrent()
{
    if (!m_isSettingCurrentToSelected)
    {
        m_isSettingCurrentToSelected = true;
        Object* pCurrentItem = get_Items()->get_CurrentItem();
        if (pCurrentItem != NULL && ItemGetIsSelectable(pCurrentItem))
        {
            get_SelectionChange()->SelectJustThisItem(pCurrentItem, true);
        }
        else
        {
            get_SelectionChange()->SelectJustThisItem(NULL, false);
        }
        m_isSettingCurrentToSelected = false;
    }
}

bool Selector::DidMouseMove()
{
    Point position = get_InputManager()->get_MousePosition(this);
    if (position != m_lastMousePosition)
    {
        m_lastMousePosition = position;
        return true;
    }
    return false;
}


void Selector::SelectionChanger::CreateDeltaSelectionChange(ItemStructList<ObjectPtr> *pUnselectedItems, ItemStructList<ObjectPtr> *pSelectedItems)
{
    for (int i = 0; i < m_toDeferSelect.get_Count(); i++)
    {
        Object* pItem = m_toDeferSelect[i];
        if (m_pOwner->get_Items()->Contains(pItem))
        {
            m_toSelect.AddItem(pItem);
            m_toDeferSelect.Remove(pItem);
            i--;
        }
    }

    if (m_toUnselect.get_Count() > 0 || m_toSelect.get_Count() > 0)
    {
        for(int i = 0; i < m_toUnselect.get_Count(); i++)
        {
            Object* pCurrent = m_toUnselect[i];

            m_pOwner->ItemSetIsSelected(pCurrent, false);
            if (m_pOwner->m_selectedItems.Contains(pCurrent))
            {
                m_pOwner->m_selectedItems.Remove(pCurrent);
                pUnselectedItems->AddItem(pCurrent);
            }
        }
        for(int i = 0; i < m_toSelect.get_Count(); i++)
        {
            Object* pCurrent = m_toSelect[i];

            m_pOwner->ItemSetIsSelected(pCurrent, true);
            if (!m_pOwner->m_selectedItems.Contains(pCurrent))
            {
                m_pOwner->m_selectedItems.AddItem(pCurrent);
                pSelectedItems->AddItem(pCurrent);
            }
        }
    }
}

void Selector::SelectionChanger::ApplyCanSelectMultiple()
{
    if (!m_pOwner->get_CanSelectMultiple())
    {
        if (m_toSelect.get_Count() == 1)
        {
            m_toUnselect.Clear();

            for(int i = 0; i < m_pOwner->m_selectedItems.get_Count(); i++)
            {
                m_toUnselect.AddItem(m_pOwner->m_selectedItems[i]);
            }
        }
        else if (m_pOwner->m_selectedItems.get_Count() > 1 && m_pOwner->m_selectedItems.get_Count() != (m_toUnselect.get_Count() + 1))
        {
            Object* pItemToKeep = m_pOwner->m_selectedItems[0];
            m_toUnselect.Clear();

            for(int i = 0; i < m_pOwner->m_selectedItems.get_Count(); i++)
            {               
                if(m_pOwner->m_selectedItems[i] != pItemToKeep)
                {
                    m_toUnselect.AddItem(m_pOwner->m_selectedItems[i]);
                }
            }
        }
    }
}

void Selector::SelectionChanger::End()
{
    ItemStructList<ObjectPtr> unselectedItems;
    ItemStructList<ObjectPtr> selectedItems;
    ApplyCanSelectMultiple();
    CreateDeltaSelectionChange(&unselectedItems, &selectedItems);

    m_pOwner->UpdatePublicSelectionProperties();

    Cleanup();

    if (unselectedItems.get_Count() > 0 || selectedItems.get_Count() > 0)
    {
        if (m_pOwner->m_isSynchronizedWithCurrentItemPrivate)
        {
            m_pOwner->SetCurrentToSelected();
        }
        m_pOwner->InvokeSelectionChanged(&unselectedItems, &selectedItems);
    }
}

bool Selector::SelectionChanger::Select(Object* pItem, bool assumeInItemsCollection)
{
    if (!Selector::ItemGetIsSelectable(pItem))
    {
        return false;
    }
    if (!assumeInItemsCollection && !m_pOwner->get_Items()->Contains(pItem))
    {
        if (!m_toDeferSelect.Contains(pItem))
        {
            m_toDeferSelect.AddItem(pItem);
        }
        return false;
    }
    if (!m_toUnselect.Remove(pItem))
    {
        if (m_pOwner->m_selectedItems.Contains(pItem))
        {
            return false;
        }
        if (m_toSelect.Contains(pItem))
        {
            return false;
        }
        if (!m_pOwner->get_CanSelectMultiple() && m_toSelect.get_Count() > 0)
        {
            for(int i = 0; i < m_toSelect.get_Count(); i++)
            {
                m_pOwner->ItemSetIsSelected(m_toSelect[i], false);
            }
            m_toSelect.Clear();
        }
        m_toSelect.AddItem(pItem);
    }
    return true;
}

void Selector::SelectionChanger::SelectJustThisItem(Object* pItem, bool assumeInItemsCollection)
{
    Begin();
    CleanupDeferSelection();

    bool flag = false;
    for (int i = m_pOwner->m_selectedItems.get_Count() - 1; i >= 0; i--)
    {
        if (pItem != m_pOwner->m_selectedItems[i])
        {
            Unselect(m_pOwner->m_selectedItems[i]);
        }
        else
        {
            flag = true;
        }
    }
    if ((!flag && (pItem != NULL)) && (pItem != DependencyProperty::get_UnsetValue()))
    {
        Select(pItem, assumeInItemsCollection);
    }
    End();
}

bool Selector::SelectionChanger::Unselect(Object* pItem)
{
    m_toDeferSelect.Remove(pItem);
    if (!m_toSelect.Remove(pItem))
    {
        if (!m_pOwner->m_selectedItems.Contains(pItem))
        {
            return false;
        }
        if (m_toUnselect.Contains(pItem))
        {
            return false;
        }
        m_toUnselect.AddItem(pItem);
    }
    return true;
}


SelectionChangedEventArgs::SelectionChangedEventArgs(IEnumerable* pRemovedItems, IEnumerable* pAddedItems) : RoutedEventArgs(&Selector::get_SelectionChangedEvent())
{

}







}






