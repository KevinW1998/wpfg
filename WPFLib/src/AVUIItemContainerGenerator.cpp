#include <AVUICommon.h>
#include <AVUIItemsControl.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIObjectCollection.h>
#include <AVUIItemCollection.h>
#include <AVUIItemsPresenter.h>
#include <AVUIPanel.h>

REGISTER_ELEMENT_BEGIN(AVUI::ItemContainerGenerator)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::ItemContainerGenerator::ItemBlock);
REGISTER_ELEMENT(AVUI::ItemContainerGenerator::RealizedItemBlock);
REGISTER_ELEMENT(AVUI::ItemContainerGenerator::UnrealizedItemBlock);
REGISTER_ELEMENT(AVUI::ItemsChangedEventArgs);
REGISTER_ELEMENT(AVUI::MapChangedEventArgs);

namespace AVUI {

ItemContainerGenerator::ItemContainerGenerator(ItemsControl* pHost)
{
    Initialize(NULL, pHost, pHost, 0);

    pHost->get_View()->add_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &ItemContainerGenerator::OnCollectionChanged));
}

ItemContainerGenerator::ItemContainerGenerator(ItemContainerGenerator* pParent, GroupItem* pGroupItem)
{
    Initialize(pParent, pParent->get_Host(), pGroupItem, pParent->get_Level() + 1);
}

ItemContainerGenerator::ItemContainerGenerator(ItemContainerGenerator* pParent, ItemsControl* pHost, DependencyObject* pPeer, int level)
{
    Initialize(pParent, pHost, pPeer, level);
}

void ItemContainerGenerator::Initialize(ItemContainerGenerator* pParent, ItemsControl* pHost, DependencyObject* pPeer, int level)
{
    m_pGenerator = NULL;
    m_pParent = pParent;
    m_pHost = pHost;
    m_pPeer = pPeer;
    m_level = level;
    m_status = GeneratorStatus::NotStarted;
    m_startIndexForUIFromItem = 0;
    OnRefresh();
}

ItemContainerGenerator::~ItemContainerGenerator()
{
    TRefCountedPtr<ItemBlock> pBlock = m_pItemMap;

    ClearItemMap(pBlock);
}


void ItemContainerGenerator::OnRefresh()
{
    RemoveAll();

    GeneratorPosition position(0, 0);
    ItemsChangedEventArgs eventArgs(NotifyCollectionChangedAction::Reset, position, 0, 0);
    invoke_ItemsChanged(this, &eventArgs);
}

void ItemContainerGenerator::RemoveAll()
{
    TRefCountedPtr<ItemBlock> pBlock = m_pItemMap;
    m_pItemMap = NULL;
    if (pBlock != NULL)
    {
        for (ItemBlock* pNext = pBlock->get_Next(); pNext != pBlock; pNext = pNext->get_Next())
        {
            RealizedItemBlock* pRealizedBlock = object_cast<RealizedItemBlock>(pNext);
            if (pRealizedBlock != NULL)
            {
                for (int i = 0; i < pRealizedBlock->get_ContainerCount(); i++)
                {
                    UnlinkContainerFromItem(pRealizedBlock->ContainerAt(i), pRealizedBlock->ItemAt(i));
                }
            }
        }
    }

    ClearItemMap(pBlock);

    PrepareGrouping();
    m_pItemMap = object_allocate<ItemBlock>();
    m_pItemMap->set_Prev(m_pItemMap);
    m_pItemMap->set_Next(m_pItemMap);

    TRefCountedPtr<UnrealizedItemBlock> pNewBlock = object_allocate<UnrealizedItemBlock>();

    pNewBlock->InsertAfter(m_pItemMap);
    pNewBlock->set_ItemCount(get_Items()->get_Count());



    MapChangedEventArgs eventArgs(NULL, -1, 0, pNewBlock, 0, 0);

    invoke_MapChanged(this, &eventArgs);
}

void ItemContainerGenerator::Remove(GeneratorPosition position, int count)
{
    TRefCountedPtr<UnrealizedItemBlock> pNewBlock;
    int newOffset;
    int deltaCount;
    if (position.Offset != 0)
    {
        LibraryCriticalError();
    }
    if (count <= 0)
    {
        LibraryCriticalError();
    }
    int index = position.Index;
    int offset = index;

    // pNext needs to be rooted, as may be removed from the map.
    TRefCountedPtr<ItemBlock> pNext = m_pItemMap->get_Next();

    while (pNext != m_pItemMap)
    {
        if (offset < pNext->get_ContainerCount())
        {
            break;
        }
        offset -= pNext->get_ContainerCount();
        pNext = pNext->get_Next();
    }
    RealizedItemBlock* pLastRIB = object_cast<RealizedItemBlock>(pNext);
    int num3 = (offset + count) - 1;
    while (pNext != m_pItemMap)
    {
        if (!pNext->IsType(GET_TYPE(RealizedItemBlock)))
        {
            LibraryCriticalError();
        }
        if (num3 < pNext->get_ContainerCount())
        {
            break;
        }
        num3 -= pNext->get_ContainerCount();
        pNext = pNext->get_Next();
    }
    RealizedItemBlock* pBlock3 = object_cast<RealizedItemBlock>(pNext);
    RealizedItemBlock* pBlock4 = pLastRIB;
    int num4 = offset;
    while (pBlock4 != pBlock3 || num4 <= num3)
    {
        UnlinkContainerFromItem(pBlock4->ContainerAt(num4), pBlock4->ItemAt(num4));
        if ((++num4 >= pBlock4->get_ContainerCount()) && (pBlock4 != pBlock3))
        {
            pBlock4 = object_cast<RealizedItemBlock>(pBlock4->get_Next());
            num4 = 0;
        }
    }
    bool flag = offset == 0;
    bool flag2 = num3 == (pBlock3->get_ItemCount() - 1);
    bool flag3 = flag && (pLastRIB->get_Prev()->IsType(GET_TYPE(UnrealizedItemBlock)));
    bool flag4 = flag2 && (pBlock3->get_Next()->IsType(GET_TYPE(UnrealizedItemBlock)));
    ItemBlock* pPrev = NULL;
    if (flag3)
    {
        pNewBlock = (UnrealizedItemBlock*) pLastRIB->get_Prev();
        newOffset = pNewBlock->get_ItemCount();
        deltaCount = -pNewBlock->get_ItemCount();
    }
    else if (flag4)
    {
        pNewBlock = (UnrealizedItemBlock*) pBlock3->get_Next();
        newOffset = 0;
        deltaCount = offset;
    }
    else
    {
        pNewBlock = object_allocate<UnrealizedItemBlock>();
        newOffset = 0;
        deltaCount = offset;
        pPrev = flag ? pLastRIB->get_Prev() : pLastRIB;
    }
    pNext = pLastRIB;
    while (pNext != pBlock3)
    {
        int itemCount = pNext->get_ItemCount();
        MoveItems(pNext, offset, itemCount - offset, pNewBlock, newOffset, deltaCount);
        newOffset += itemCount - offset;
        offset = 0;
        deltaCount -= itemCount;
        if (pNext->get_ItemCount() == 0)
        {
            pNext->Remove();
        }
        pNext = pNext->get_Next();
    }

    int num8 = (pNext->get_ItemCount() - 1) - num3;
    MoveItems(pNext, offset, (num3 - offset) + 1, pNewBlock, newOffset, deltaCount);
    TRefCountedPtr<RealizedItemBlock> pBlock7 = pBlock3;
    if (!flag2)
    {
        if (pLastRIB == pBlock3 && !flag)
        {
            pBlock7 = object_allocate<RealizedItemBlock>();
        }
        MoveItems(pNext, num3 + 1, num8, pBlock7, 0, num3 + 1);
    }
    if (pPrev != NULL)
    {
        pNewBlock->InsertAfter(pPrev);
    }
    if (pBlock7 != pBlock3)
    {
        pBlock7->InsertAfter(pNewBlock);
    }
    RemoveAndCoalesceBlocksIfNeeded(pNext);
}



void ItemContainerGenerator::ClearItemMap(ItemBlock* pBlock)
{
    TRefCountedPtr<ItemBlock> pCurrent = pBlock;
    TRefCountedPtr<ItemBlock> pLast;

    while(pCurrent != NULL)
    {
        pLast = pCurrent;
        pCurrent = pCurrent->get_Next();

        pLast->set_Next(NULL);
        pLast->set_Prev(NULL);
    }
}

void ItemContainerGenerator::PrepareGrouping()
{
    /*
    GroupStyle groupStyle;
    IList view;
    if (this.Level == 0)
    {
        groupStyle = this.Host.GetGroupStyle(null, 0);
        if (groupStyle == null)
        {
            view = this.Host.View;
        }
        else
        {
            CollectionView collectionView = this.Host.View.CollectionView;
            view = (collectionView == null) ? null : ((IList) collectionView.Groups);
            if (view == null)
            {
                view = this.Host.View;
            }
        }
    }
    else
    {
        GroupItem peer = (GroupItem) this.Peer;
        CollectionViewGroup group = peer.ReadLocalValue(ItemForItemContainerProperty) as CollectionViewGroup;
        if (group != null)
        {
            if (group.IsBottomLevel)
            {
                groupStyle = null;
            }
            else
            {
                groupStyle = this.Host.GetGroupStyle(group, this.Level);
            }
            view = group.Items;
        }
        else
        {
            groupStyle = null;
            view = this.Host.View;
        }
    }
    this.GroupStyle = groupStyle;
    */

    set_Items(m_pHost->get_View());

    /*
    if ((this.Level == 0) && (this.Host != null))
    {
        this.Host.SetIsGrouping(this.IsGrouping);
    }
    */

    
}

void ItemContainerGenerator::UnlinkContainerFromItem(DependencyObject* pContainer, Object* pItem)
{
    pContainer->ClearValue(get_ItemForItemContainerProperty());
    m_pHost->ClearContainerForItem(pContainer, pItem);
}



void ItemContainerGenerator::set_Items(Object* pItems)
{
    if (m_pItemsObject != pItems)
    {
        /*
        INotifyCollectionChanged source = this._items as INotifyCollectionChanged;
        if ((this._items != this.Host.View) && (source != null))
        {
            CollectionChangedEventManager.RemoveListener(source, this);
        }
        */

        m_pItemsObject = pItems;
        m_pItems = interface_cast<ICollection>(pItems);

        /*
        source = this._items as INotifyCollectionChanged;
        if ((this._items != this.Host.View) && (source != null))
        {
            CollectionChangedEventManager.AddListener(source, this);
        }
        */
    }
}



ItemContainerGenerator* ItemContainerGenerator::GetItemContainerGeneratorForPanel(Panel* pPanel)
{
    if (!pPanel->get_IsItemsHost())
    {
        LibraryCriticalError();
    }
    ItemsPresenter* pPresenter = ItemsPresenter::FromPanel(pPanel);
    if (pPresenter != NULL)
    {
        return pPresenter->get_Generator();
    }
    if (pPanel->get_TemplatedParent() != NULL)
    {
        return this;
    }
    return NULL;
}

 
void ItemContainerGenerator::MoveToPosition(GeneratorPosition position, GeneratorDirection::Enum direction, bool allowStartAtRealizedItem, GeneratorState* pState)
{
    ItemBlock* pNext = m_pItemMap;
    int num = 0;
    if (position.Index != -1)
    {
        int num2 = 0;
        int index = position.Index;
        pNext = pNext->get_Next();
        while (index >= pNext->get_ContainerCount())
        {
            num2 += pNext->get_ItemCount();
            index -= pNext->get_ContainerCount();
            num += pNext->get_ItemCount();
            pNext = pNext->get_Next();
        }
        pState->Block = pNext;
        pState->Offset = index;
        pState->Count = num2;
        pState->ItemIndex = num + index;
    }
    else
    {
        pState->Block = pNext;
        pState->Offset = 0;
        pState->Count = 0;
        pState->ItemIndex = num - 1;
    }
    int offset = position.Offset;
    if ((offset == 0) && (!allowStartAtRealizedItem || (pState->Block == m_pItemMap)))
    {
        offset = (direction == GeneratorDirection::Forward) ? 1 : -1;
    }
    if (offset > 0)
    {
        pState->Block->MoveForward(pState, true);
        while (--offset > 0)
        {
            pState->Block->MoveForward(pState, allowStartAtRealizedItem);
        }
    }
    else if (offset < 0)
    {
        if (pState->Block == m_pItemMap)
        {
            pState->ItemIndex = pState->Count = get_Items()->get_Count();
        }
        pState->Block->MoveBackward(pState, true);
        while (++offset < 0)
        {
            pState->Block->MoveBackward(pState, allowStartAtRealizedItem);
        }
    }
}



void ItemContainerGenerator::SetStatus(GeneratorStatus::Enum value)
{
    if (value != m_status)
    {
        m_status = value;

        invoke_StatusChanged(this, NULL);
    }
}

 
void ItemContainerGenerator::Realize(UnrealizedItemBlock* pBlock, int offset, Object* pItem, DependencyObject* pContainer)
{
    TRefCountedPtr<RealizedItemBlock> pBlock2;
    TRefCountedPtr<RealizedItemBlock> pNewBlock;
    int newOffset;
    if (offset == 0 && ((pBlock2 = object_cast<RealizedItemBlock>(pBlock->get_Prev())) != NULL) && pBlock2->get_ItemCount() < 0x10)
    {
        pNewBlock = pBlock2;
        newOffset = pBlock2->get_ItemCount();
        MoveItems(pBlock, offset, 1, pNewBlock, newOffset, -pBlock2->get_ItemCount());
        MoveItems(pBlock, 1, pBlock->get_ItemCount(), pBlock, 0, 1);
    }
    else
    {
        TRefCountedPtr<RealizedItemBlock> pBlock3;
        if (offset == (pBlock->get_ItemCount() - 1) && ((pBlock3 = object_cast<RealizedItemBlock>(pBlock->get_Next())) != NULL) && pBlock3->get_ItemCount() < 0x10)
        {
            pNewBlock = pBlock3;
            newOffset = 0;
            MoveItems(pNewBlock, 0, pNewBlock->get_ItemCount(), pNewBlock, 1, -1);
            MoveItems(pBlock, offset, 1, pNewBlock, newOffset, offset);
        }
        else
        {
            pNewBlock = object_allocate<RealizedItemBlock>();
            newOffset = 0;
            if (offset == 0)
            {
                pNewBlock->InsertBefore(pBlock);
                MoveItems(pBlock, offset, 1, pNewBlock, newOffset, 0);
                MoveItems(pBlock, 1, pBlock->get_ItemCount(), pBlock, 0, 1);
            }
            else if (offset == (pBlock->get_ItemCount() - 1))
            {
                pNewBlock->InsertAfter(pBlock);
                MoveItems(pBlock, offset, 1, pNewBlock, newOffset, offset);
            }
            else
            {
                TRefCountedPtr<UnrealizedItemBlock> pBlock5 = object_allocate<UnrealizedItemBlock>();
                pBlock5->InsertAfter(pBlock);
                pNewBlock->InsertAfter(pBlock);
                MoveItems(pBlock, offset + 1, (pBlock->get_ItemCount() - offset) - 1, pBlock5, 0, offset + 1);
                MoveItems(pBlock, offset, 1, pNewBlock, 0, offset);
            }
        }
    }
    RemoveAndCoalesceBlocksIfNeeded(pBlock);
    pNewBlock->RealizeItem(newOffset, pItem, pContainer);
}

 
void ItemContainerGenerator::MoveItems(ItemBlock* pBlock, int offset, int count, ItemBlock* pNewBlock, int newOffset, int deltaCount)
{
    RealizedItemBlock* pSrc = object_cast<RealizedItemBlock>(pBlock);
    RealizedItemBlock* pBlock3 = object_cast<RealizedItemBlock>(pNewBlock);
    if (pSrc != NULL && pBlock3 != NULL)
    {
        pBlock3->CopyEntries(pSrc, offset, count, newOffset);
    }
    else if (pSrc != NULL && pSrc->get_ItemCount() > count)
    {
        pSrc->ClearEntries(offset, count);
    }
    pBlock->set_ItemCount(pBlock->get_ItemCount() - count);
    pNewBlock->set_ItemCount(pNewBlock->get_ItemCount() + count);


    MapChangedEventArgs eventArgs(pBlock, offset, count, pNewBlock, newOffset, deltaCount);
    invoke_MapChanged(this, &eventArgs);
}

void ItemContainerGenerator::RemoveAndCoalesceBlocksIfNeeded(ItemBlock* pBlock)
{
    if (pBlock != NULL && pBlock != m_pItemMap && pBlock->get_ItemCount() == 0)
    {
        pBlock->Remove();
        if (pBlock->get_Prev()->IsType(GET_TYPE(UnrealizedItemBlock)) && pBlock->get_Next()->IsType(GET_TYPE(UnrealizedItemBlock)))
        {
            MoveItems(pBlock->get_Next(), 0, pBlock->get_Next()->get_ItemCount(), pBlock->get_Prev(), pBlock->get_Prev()->get_ItemCount(), -pBlock->get_Prev()->get_ItemCount() - 1);
            pBlock->get_Next()->Remove();
        }
    }
}

 

void ItemContainerGenerator::LinkContainerToItem(DependencyObject* pContainer, Object* pItem)
{
    pContainer->ClearValue(get_ItemForItemContainerProperty());
    if (pContainer != pItem)
    {
        /////////////DEVIATION ! If pItem = pContainer, then ItemForItemContainer is a circular reference only set if not! ///////////////
        pContainer->SetValue(get_ItemForItemContainerProperty(), pItem);

        pContainer->ClearValue(UIElement::get_DataContextProperty());
        pContainer->SetValue(UIElement::get_DataContextProperty(), pItem);
    }
}

 
Object* ItemContainerGenerator::ItemFromContainer(DependencyObject* pContainer)
{
    if (pContainer == NULL)
    {
        LibraryCriticalError();
    }
    Object* pLocalValue = pContainer->ReadLocalValue(get_ItemForItemContainerProperty());

    if(pLocalValue == DependencyProperty::get_UnsetValue())
    {
        pLocalValue = pContainer;
    }

    if(!get_Host()->IsHostForItemContainer(pContainer))
    {
        pLocalValue = DependencyProperty::get_UnsetValue();
    }
    return pLocalValue;
}

 
DependencyObject* ItemContainerGenerator::ContainerFromItem(Object* pItem)
{
    DependencyObject* pContainer = NULL;
    int itemIndex;

    DoLinearSearch(&pContainer, &pItem, &itemIndex);

    return pContainer;
}

DependencyObject* ItemContainerGenerator::ContainerFromIndex(int index)
{
    int num = 0;

    // TODO -- Grouping
    /*
    if (this.IsGrouping)
    {
        num = index;
        index = 0;
        int count = this.Items.Count;
        while (index < count)
        {
            CollectionViewGroup group = this.Items[index] as CollectionViewGroup;
            int num3 = (group == null) ? 1 : group.ItemCount;
            if (num < num3)
            {
                break;
            }
            num -= num3;
            index++;
        }
    }
    */
    for (ItemBlock* pNext = m_pItemMap->get_Next(); pNext != m_pItemMap; pNext = pNext->get_Next())
    {
        if (index < pNext->get_ItemCount())
        {
            DependencyObject* pObj2 = pNext->ContainerAt(index);
            // TODO -- Grouping
            /*
            GroupItem item = obj2 as GroupItem;
            if (item != null)
            {
                obj2 = item.Generator.ContainerFromIndex(num);
            }
            */
            return pObj2;
        }
        index -= pNext->get_ItemCount();
    }
    return NULL;

}


int ItemContainerGenerator::IndexFromContainer(DependencyObject* pContainer)
{
    Object* pItem = NULL;
    int itemIndex;

    DoLinearSearch(&pContainer, &pItem, &itemIndex);

    return itemIndex;
}

int ItemContainerGenerator::IndexFromGeneratorPosition(GeneratorPosition position)
{
    int index = position.Index;
    if (index == -1)
    {
        if (position.Offset >= 0)
        {
            return (position.Offset - 1);
        }
        return (get_Items()->get_Count() + position.Offset);
    }
    if (m_pItemMap != NULL)
    {
        int num2 = 0;
        for (ItemBlock* pNext = m_pItemMap->get_Next(); pNext != m_pItemMap; pNext = pNext->get_Next())
        {
            if (index < pNext->get_ContainerCount())
            {
                return ((num2 + index) + position.Offset);
            }
            num2 += pNext->get_ItemCount();
            index -= pNext->get_ContainerCount();
        }
    }
    return -1;
}

 
GeneratorPosition ItemContainerGenerator::GeneratorPositionFromIndex(int itemIndex)
{
    GeneratorPosition position;
    ItemBlock* pBlock;
    int offsetFromBlockStart;
    GetBlockAndPosition(itemIndex, &position, &pBlock, &offsetFromBlockStart);
    if (pBlock == m_pItemMap && position.Index == -1)
    {
        position.Offset++;
    }
    return position;
}

 

 



bool ItemContainerGenerator::DoLinearSearch(DependencyObject** ppContainer, Object** ppItem, int* pItemIndex)
{
    int num2;
    *pItemIndex = 0;

    ItemBlock* pNext = m_pItemMap->get_Next();
    int num = 0;

    while (num <= m_startIndexForUIFromItem && pNext != m_pItemMap)
    {
        num += pNext->get_ItemCount();
        pNext = pNext->get_Next();
    }
    pNext = pNext->get_Prev();
    num -= pNext->get_ItemCount();

    RealizedItemBlock* pRIB = object_cast<RealizedItemBlock>(pNext);
    if (pRIB != NULL)
    {
        num2 = m_startIndexForUIFromItem - num;
        if (num2 >= pRIB->get_ItemCount())
        {
            num2 = 0;
        }
    }
    else
    {
        num2 = 0;
    }

    ItemBlock* pStop = pNext;
    int index = num2;
    int itemCount = pNext->get_ItemCount();

    do
    {
        if (pRIB != NULL)
        {
            while (index < itemCount)
            {
                bool flag = false;
                if (*ppContainer != NULL)
                {
                    if (pRIB->ContainerAt(index) == *ppContainer)
                    {
                        flag = true;
                        *ppItem = pRIB->ItemAt(index);
                    }
                }
                else if (Object::Equals(*ppItem, pRIB->ItemAt(index)))
                {
                    flag = true;
                    *ppContainer = pRIB->ContainerAt(index);
                }
                // TODO -- Grouping
                /*
                if ((this.IsGrouping && !flag) && (rib.ItemAt(index) is CollectionViewGroup))
                {
                    GroupItem item2 = (GroupItem) rib.ContainerAt(index);
                    flag = item2.Generator.DoLinearSearch(ref container, ref item, out itemIndex);
                }
                */
                if (flag)
                {
                    m_startIndexForUIFromItem = num + index;
                    *pItemIndex += GetRealizedItemBlockCount(pRIB, index) + GetCount(pStop);
                    return true;
                }
                index++;
            }
            if (pStop == pNext && index == num2)
            {
                *pItemIndex = -1;
                return false;
            }
        }
        num += pStop->get_ItemCount();
        index = 0;
        pStop = pStop->get_Next();
        if (pStop == m_pItemMap)
        {
            pStop = pStop->get_Next();
            num = 0;
        }
        itemCount = pStop->get_ItemCount();
        pRIB = object_cast<RealizedItemBlock>(pStop);

    } while(pStop != pNext || (pRIB != NULL && (itemCount = num2)));

    *pItemIndex = -1;
    return false;
}

int ItemContainerGenerator::GetRealizedItemBlockCount(RealizedItemBlock* pRIB, int end)
{
    return end;
    // TODO Group
    /*
    if (!get_IsGrouping())
    {
        return end;
    }

    int num = 0;
    for (int i = 0; i < end; i++)
    {
        if (rib.ItemAt(i) is CollectionViewGroup)
        {
            GroupItem item = (GroupItem) rib.ContainerAt(i);
            num += item.Generator.GetCount();
        }
        else
        {
            num++;
        }
    }
    return num;
    */
}
 
int ItemContainerGenerator::GetCount()
{
    return GetCount(m_pItemMap);
}

int ItemContainerGenerator::GetCount(ItemBlock* pStop)
{
    int num = 0;
    for (ItemBlock* pNext = m_pItemMap->get_Next(); pNext != pStop; pNext = pNext->get_Next())
    {
        RealizedItemBlock* pRIB = object_cast<RealizedItemBlock>(pNext);
        if (pRIB != NULL)
        {
            num += GetRealizedItemBlockCount(pRIB, pRIB->get_ItemCount());
        }
        else
        {
            num += pNext->get_ItemCount();
        }
    }
    return num;
}



void ItemContainerGenerator::GetBlockAndPosition(Object* pItem, int itemIndex, bool deletedFromItems, GeneratorPosition* pPosition, ItemBlock** ppBlock, int * pOffsetFromBlockStart, int *pCorrectIndex)
{
    if (itemIndex >= 0)
    {
        GetBlockAndPosition(itemIndex, pPosition, ppBlock, pOffsetFromBlockStart);
        *pCorrectIndex = itemIndex;
    }
    else
    {
        GetBlockAndPosition(pItem, deletedFromItems, pPosition, ppBlock, pOffsetFromBlockStart, pCorrectIndex);
    }
}

 
void ItemContainerGenerator::GetBlockAndPosition(int itemIndex, GeneratorPosition* pPosition, ItemBlock** ppBlock, int* pOffsetFromBlockStart)
{
    *pPosition = GeneratorPosition(-1, 0);
    *ppBlock = NULL;
    *pOffsetFromBlockStart = itemIndex;
    if (m_pItemMap != NULL && itemIndex >= 0)
    {
        int num = 0;
        *ppBlock = m_pItemMap->get_Next();
        while (*ppBlock != m_pItemMap)
        {
            if (*pOffsetFromBlockStart >= (*ppBlock)->get_ItemCount())
            {
                num += (*ppBlock)->get_ContainerCount();
                *pOffsetFromBlockStart -= (*ppBlock)->get_ItemCount();
            }
            else
            {
                if ((*ppBlock)->get_ContainerCount() > 0)
                {
                    *pPosition = GeneratorPosition(num + *pOffsetFromBlockStart, 0);
                    return;
                }
                *pPosition = GeneratorPosition(num - 1, *pOffsetFromBlockStart + 1);
                return;
            }
            *ppBlock = (*ppBlock)->get_Next();
        }
    }
}

 

void ItemContainerGenerator::GetBlockAndPosition(Object* pItem, bool deletedFromItems, GeneratorPosition *pPosition, ItemBlock** ppBlock, int *pOffsetFromBlockStart, int* pCorrectIndex)
{
    *pCorrectIndex = 0;
    int num = 0;
    *pOffsetFromBlockStart = 0;
    int num2 = deletedFromItems ? 1 : 0;
    *pPosition = GeneratorPosition(-1, 0);
    *ppBlock = m_pItemMap->get_Next();
    while (*ppBlock != m_pItemMap)
    {
        RealizedItemBlock* pRIB = object_cast<RealizedItemBlock>(*ppBlock);
        if (pRIB != NULL)
        {
            *pOffsetFromBlockStart = pRIB->OffsetOfItem(pItem);
            if ((*pOffsetFromBlockStart) >= 0)
            {
                *pPosition = GeneratorPosition(num + *pOffsetFromBlockStart, 0);
                *pCorrectIndex += *pOffsetFromBlockStart;
                break;
            }
        }
        else if((*ppBlock)->IsType(GET_TYPE(UnrealizedItemBlock)))
        {
            bool flag = false;
            pRIB = object_cast<RealizedItemBlock>((*ppBlock)->get_Next());
            if (pRIB != NULL && pRIB->get_ContainerCount() > 0)
            {
                flag = Object::Equals(pRIB->ItemAt(0), get_Items()->GetItemAt(*pCorrectIndex + (*ppBlock)->get_ItemCount() - num2));
            }
            else if ((*ppBlock)->get_Next() == m_pItemMap)
            {
                flag = ((*ppBlock)->get_Prev() == m_pItemMap) || (get_Items()->get_Count() == ((*pCorrectIndex + (*ppBlock)->get_ItemCount()) - num2));
            }
            if (flag)
            {
                *pOffsetFromBlockStart = 0;
                *pPosition = GeneratorPosition(num - 1, 1);
                break;
            }
        }

        *pCorrectIndex += (*ppBlock)->get_ItemCount();
        num += (*ppBlock)->get_ContainerCount();
        *ppBlock = (*ppBlock)->get_Next();
    }
    if (*ppBlock == m_pItemMap)
    {
        LibraryCriticalError();
    }
}

 



















ItemContainerGenerator::ItemBlock::ItemBlock() : m_count(0)
{

}

void ItemContainerGenerator::ItemBlock::InsertAfter(ItemBlock* pPrev)
{
    set_Next(pPrev->get_Next());
    set_Prev(pPrev);
    get_Prev()->set_Next(this);
    get_Next()->set_Prev(this);
}

void ItemContainerGenerator::ItemBlock::InsertBefore(ItemBlock* pNext)
{
    InsertAfter(pNext->get_Prev());
}

void ItemContainerGenerator::ItemBlock::Remove()
{
   get_Prev()->set_Next(get_Next());
   get_Next()->set_Prev(get_Prev());
}

void ItemContainerGenerator::ItemBlock::MoveBackward(GeneratorState* pState, bool allowMovePastRealizedItem)
{
    if (IsMoveAllowed(allowMovePastRealizedItem))
    {
        if (--(pState->Offset) < 0)
        {
            pState->Block = get_Prev();
            pState->Offset = pState->Block->get_ItemCount() - 1;
            pState->Count -= pState->Block->get_ItemCount();
        }
        pState->ItemIndex--;
    }
}

void ItemContainerGenerator::ItemBlock::MoveForward(GeneratorState* pState, bool allowMovePastRealizedItem)
{
    if (IsMoveAllowed(allowMovePastRealizedItem))
    {
        (pState->ItemIndex)++;
        if (++(pState->Offset) >= get_ItemCount())
        {
            pState->Block = get_Next();
            pState->Offset = 0;
            pState->Count += get_ItemCount();
        }
    }
}













ItemContainerGenerator::RealizedItemBlock::RealizedItemBlock() : m_entry(0x10)
{
    m_entry.AddEmptyItems(0x10);
}

void ItemContainerGenerator::RealizedItemBlock::ClearEntries(int offset, int count)
{
    for (int i = 0; i < count; i++)
    {
        m_entry[offset + i].Item = NULL;
        m_entry[offset + i].Container = NULL;
    }
}

TRefCountedPtr<DependencyObject> ItemContainerGenerator::RealizedItemBlock::ContainerAt(int index)
{
    return m_entry[index].Container;
}

void ItemContainerGenerator::RealizedItemBlock::CopyEntries(RealizedItemBlock* pSrc, int offset, int count, int newOffset)
{
    int num;
    if (offset < newOffset)
    {
        for (num = count - 1; num >= 0; num--)
        {
            m_entry[newOffset + num] = pSrc->m_entry[offset + num];
        }
        if (pSrc != this)
        {
            pSrc->ClearEntries(offset, count);
        }
        else
        {
            pSrc->ClearEntries(offset, newOffset - offset);
        }
    }
    else
    {
        for (num = 0; num < count; num++)
        {
            m_entry[newOffset + num] = pSrc->m_entry[offset + num];
        }
        if (pSrc != this)
        {
            pSrc->ClearEntries(offset, count);
        }
        else
        {
            pSrc->ClearEntries(newOffset + count, offset - newOffset);
        }
    }
}

ObjectPtr ItemContainerGenerator::RealizedItemBlock::ItemAt(int index)
{
    return m_entry[index].Item;
}

int ItemContainerGenerator::RealizedItemBlock::OffsetOfItem(Object* pItem)
{
    for (int i = 0; i < get_ItemCount(); i++)
    {
        if (Object::Equals(m_entry[i].Item, pItem))
        {
            return i;
        }
    }
    return -1;
}

void ItemContainerGenerator::RealizedItemBlock::RealizeItem(int index, Object* pItem, DependencyObject* pContainer)
{
   m_entry[index].Item = pItem;
   m_entry[index].Container = pContainer;
}

ObjectPtr ItemContainerGenerator::StartAt(GeneratorPosition position, GeneratorDirection::Enum direction, bool allowStartAtRealizedItem)
{
    if (m_pGenerator != NULL)
    {
        LibraryCriticalError();
    }
    TRefCountedPtr<Generator> pGenerator = object_allocate<Generator>(this, position, direction, allowStartAtRealizedItem);
    m_pGenerator = pGenerator;
    return m_pGenerator;
}

TRefCountedPtr<DependencyObject> ItemContainerGenerator::GenerateNext(bool* pIsNewlyRealized)
{
    if(m_pGenerator == NULL)
    {
        LibraryCriticalError();
    }

    if(pIsNewlyRealized)
    {
        return m_pGenerator->GenerateNext(false, pIsNewlyRealized);
    }
    else
    {
        bool isNewlyRealized;
        return m_pGenerator->GenerateNext(false, &isNewlyRealized);
    }
}

void ItemContainerGenerator::PrepareItemContainer(DependencyObject* pContainer)
{
    Object* pItem = pContainer->ReadLocalValue(get_ItemForItemContainerProperty());

    /////////////DEVIATION ! If pItem = pContainer, then ItemForItemContainer is a circular reference! ///////////////
    if(pItem == NULL)
    {
        pItem = pContainer;
    }
    get_Host()->PrepareItemContainer(pContainer, pItem);
}

void ItemContainerGenerator::OnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{
    Object* pObj2;
    int index = -1;
    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Add:
    case NotifyCollectionChangedAction::Remove:
        if (pSender == m_pItemsObject)
        {
            if (pArgs->get_Action() == NotifyCollectionChangedAction::Add)
            {
                pObj2 = pArgs->get_NewItem();
                index = pArgs->get_NewStartingIndex();
            }
            else
            {
                pObj2 = pArgs->get_OldItem();
                index = pArgs->get_OldStartingIndex();
            }
            break;
        }
        return;
    
    case NotifyCollectionChangedAction::Replace:
    case NotifyCollectionChangedAction::Move:
    case NotifyCollectionChangedAction::Reset:
        pObj2 = NULL;
        break;
    
    default:
        LibraryCriticalError();
    }

    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Add:
        OnItemAdded(pObj2, index);
        return;
    
    case NotifyCollectionChangedAction::Remove:
        OnItemRemoved(pObj2, index);
        return;
    
    case NotifyCollectionChangedAction::Replace:
        OnItemReplaced(pArgs->get_OldItem(), pArgs->get_NewItem(), pArgs->get_NewStartingIndex());
        return;
    
    case NotifyCollectionChangedAction::Move:
        OnItemMoved(pArgs->get_OldItem(), pArgs->get_OldStartingIndex(), pArgs->get_NewStartingIndex());
        return;
    
    case NotifyCollectionChangedAction::Reset:
        OnRefresh();
        return;
    }
    
    
    
    

}

void ItemContainerGenerator::ValidateAndCorrectIndex(Object* pItem, int* pIndex)
{
    if (*pIndex < 0)
    {
        *pIndex = get_Items()->IndexOf(pItem);
        if (*pIndex < 0)
        {
            LibraryCriticalError();
        }
    }
}

void ItemContainerGenerator::OnItemAdded(Object* pItem, int index)
{
    ValidateAndCorrectIndex(pItem, &index);

    GeneratorPosition position(-1, 0);
    TRefCountedPtr<ItemBlock> pNext = m_pItemMap->get_Next();
    int offset = index;

    while ((pNext != m_pItemMap) && (offset >= pNext->get_ItemCount()))
    {
        offset -= pNext->get_ItemCount();
        position.Index += pNext->get_ContainerCount();
        pNext = pNext->get_Next();
    }

    position.Offset = offset + 1;
    TRefCountedPtr<UnrealizedItemBlock> pBlock = object_cast<UnrealizedItemBlock>(pNext);
    if (pBlock != NULL)
    {
        MoveItems(pBlock, offset, 1, pBlock, offset + 1, 0);
        pBlock->set_ItemCount(pBlock->get_ItemCount() + 1);
    }
    else if (((offset == 0) || (pNext == m_pItemMap)) && ((pBlock = object_cast<UnrealizedItemBlock>(pNext->get_Prev())) != NULL))
    {
        pBlock->set_ItemCount(pBlock->get_ItemCount() + 1);
    }
    else
    {
        TRefCountedPtr<RealizedItemBlock> pBlock3;
        pBlock = object_allocate<UnrealizedItemBlock>();
        pBlock->set_ItemCount(1);
        if ((offset > 0) && ((pBlock3 = object_cast<RealizedItemBlock>(pNext)) != NULL))
        {
            TRefCountedPtr<RealizedItemBlock> pNewBlock = object_allocate<RealizedItemBlock>();
            MoveItems(pBlock3, offset, pBlock3->get_ItemCount() - offset, pNewBlock, 0, offset);
            pNewBlock->InsertAfter(pBlock3);
            position.Index += pNext->get_ContainerCount();
            position.Offset = 1;
            pNext = pNewBlock;
        }
        pBlock->InsertBefore(pNext);
    }

    MapChangedEventArgs eventArgs(NULL, index, 1, NULL, 0, 0);
    invoke_MapChanged(this, &eventArgs);

    ItemsChangedEventArgs itemEventArgs(NotifyCollectionChangedAction::Add, position, 1, 0);
    invoke_ItemsChanged(this, &itemEventArgs);
}

void ItemContainerGenerator::OnItemRemoved(Object* pItem, int itemIndex)
{
    TRefCountedPtr<DependencyObject> pContainer = NULL;
    GeneratorPosition position;
    ItemBlock* pBlock;
    int num2;
    int num3;
    int itemUICount = 0;
    GetBlockAndPosition(pItem, itemIndex, true, &position, &pBlock, &num2, &num3);
    TRefCountedPtr<RealizedItemBlock> pBlock2 = object_cast<RealizedItemBlock>(pBlock);
    if (pBlock2 != NULL)
    {
        itemUICount = 1;
        pContainer = pBlock2->ContainerAt(num2);
    }
    MoveItems(pBlock, num2 + 1, (pBlock->get_ItemCount() - num2) - 1, pBlock, num2, 0);
    pBlock->set_ItemCount(pBlock->get_ItemCount() - 1);
    RemoveAndCoalesceBlocksIfNeeded(pBlock);


    MapChangedEventArgs eventArgs(NULL, itemIndex, -1, NULL, 0, 0);
    invoke_MapChanged(this, &eventArgs);

    ItemsChangedEventArgs itemEventArgs(NotifyCollectionChangedAction::Remove, position, 1, itemUICount);
    invoke_ItemsChanged(this, &itemEventArgs);

    if (pContainer != NULL)
    {
        UnlinkContainerFromItem(pContainer, pItem);
    }

    /* TODO Grouping
    if ((this.Level > 0) && (this.Items.Count == 0))
    {
        GroupItem peer = (GroupItem) this.Peer;
        CollectionViewGroup group = peer.ReadLocalValue(ItemForItemContainerProperty) as CollectionViewGroup;
        if (group != null)
        {
            this.Parent.OnSubgroupBecameEmpty(group);
        }
    }
    */
}
 
void ItemContainerGenerator::OnItemReplaced(Object* pOldItem, Object* pNewItem, int index)
{
    GeneratorPosition position;
    ItemBlock* pBlock;
    int num;
    int num2;
    GetBlockAndPosition(pOldItem, index, false, &position, &pBlock, &num, &num2);
    TRefCountedPtr<RealizedItemBlock> pBlock2 = object_cast<RealizedItemBlock>(pBlock);
    if (pBlock2 != NULL)
    {
        TRefCountedPtr<DependencyObject> pContainer = pBlock2->ContainerAt(num);
        pBlock2->RealizeItem(num, pNewItem, pContainer);
        LinkContainerToItem(pContainer, pNewItem);
        m_pHost->PrepareItemContainer(pContainer, pNewItem);
    }
}

DependencyObject* ItemContainerGenerator::GetParentInternal(DependencyObject* pElement)
{
    if(pElement == NULL)
    {
        return NULL;
    }

    if(!pElement->IsType(GET_TYPE(Visual)))
    {
        return NULL;
    }

    return ((Visual*) pElement)->get_VisualParent();
}


void ItemContainerGenerator::OnItemMoved(Object* pItem, int oldIndex, int newIndex)
{
    TRefCountedPtr<DependencyObject> pReference = NULL;
    GeneratorPosition position;
    ItemBlock* pNext;
    int num2;
    int num3;
    int itemUICount = 0;
    GetBlockAndPosition(pItem, oldIndex, true, &position, &pNext, &num2, &num3);
    GeneratorPosition oldPosition = position;
    RealizedItemBlock* pBlock3 = object_cast<RealizedItemBlock>(pNext);
    if (pBlock3 != NULL)
    {
        itemUICount = 1;
        pReference = pBlock3->ContainerAt(num2);
    }
    MoveItems(pNext, num2 + 1, (pNext->get_ItemCount() - num2) - 1, pNext, num2, 0);
    pNext->set_ItemCount(pNext->get_ItemCount() - 1);
    RemoveAndCoalesceBlocksIfNeeded(pNext);
    position = GeneratorPosition(-1, 0);
    pNext = m_pItemMap->get_Next();
    num2 = newIndex;
    while ((pNext != m_pItemMap) && (num2 >= pNext->get_ItemCount()))
    {
        num2 -= pNext->get_ItemCount();
        position.Index += pNext->get_ContainerCount();
        pNext = pNext->get_Next();
    }
    position.Offset = num2 + 1;
    TRefCountedPtr<UnrealizedItemBlock> pBlock = object_cast<UnrealizedItemBlock>(pNext);
    if (pBlock != NULL)
    {
        MoveItems(pBlock, num2, 1, pBlock, num2 + 1, 0);
        pBlock->set_ItemCount(pBlock->get_ItemCount() + 1);
    }
    else if (((num2 == 0) || (pNext == m_pItemMap)) && ((pBlock = object_cast<UnrealizedItemBlock>(pNext->get_Prev())) != NULL))
    {
        pBlock->set_ItemCount(pBlock->get_ItemCount() + 1);
    }
    else
    {
        pBlock = object_allocate<UnrealizedItemBlock>();
        pBlock->set_ItemCount(1);
        if ((num2 > 0) && ((pBlock3 = object_cast<RealizedItemBlock>(pNext)) != NULL))
        {
            TRefCountedPtr<RealizedItemBlock> pNewBlock = object_allocate<RealizedItemBlock>();
            MoveItems(pBlock3, num2, pBlock3->get_ItemCount() - num2, pNewBlock, 0, num2);
            pNewBlock->InsertAfter(pBlock3);
            position.Index += pNext->get_ContainerCount();
            position.Offset = 1;
            num2 = 0;
            pNext = pNewBlock;
        }
        pBlock->InsertBefore(pNext);
    }


    DependencyObject* pParentInternal = GetParentInternal(pReference);

    ItemsChangedEventArgs itemEventArgs(NotifyCollectionChangedAction::Move, position, oldPosition, 1, itemUICount);
    invoke_ItemsChanged(this, &itemEventArgs);

    if (pReference != NULL)
    {
        if (pParentInternal == NULL || (GetParentInternal(pReference) != pParentInternal))
        {
            UnlinkContainerFromItem(pReference, pItem);
        }
        else
        {
            Realize(pBlock, num2, pItem, pReference);
        }
    }
}

ItemContainerGenerator::Generator::Generator(ItemContainerGenerator* pFactory, GeneratorPosition position, GeneratorDirection::Enum direction, bool allowStartAtRealizedItem)
{
    m_pFactory = pFactory;
    m_direction = direction;

    m_pFactory->add_MapChanged(MapChangedEventHandler::Create(this, &ItemContainerGenerator::Generator::OnMapChanged));

    m_pFactory->MoveToPosition(position, direction, allowStartAtRealizedItem, &m_cachedState);
    m_done = m_pFactory->get_Items()->get_Count() == 0;
    m_pFactory->SetStatus(GeneratorStatus::GeneratingContainers);
}

ItemContainerGenerator::Generator::~Generator()
{
    if (m_pFactory != NULL)
    {
        m_pFactory->remove_MapChanged(MapChangedEventHandler::Create(this, &ItemContainerGenerator::Generator::OnMapChanged));
        m_pFactory->SetStatus(GeneratorStatus::ContainersGenerated);
        m_pFactory->m_pGenerator = NULL;
        m_pFactory = NULL;
    }
}


TRefCountedPtr<DependencyObject> ItemContainerGenerator::Generator::GenerateNext(bool stopAtRealized, bool* pIsNewlyRealized)
{
    TRefCountedPtr<DependencyObject> pContainer = NULL;

    *pIsNewlyRealized = false;

    while(pContainer == NULL)
    {
        TRefCountedPtr<UnrealizedItemBlock> pBlock = object_cast<UnrealizedItemBlock>(m_cachedState.Block);
        ICollection* pItems = m_pFactory->get_Items();

        int itemIndex = m_cachedState.ItemIndex;

        if (m_cachedState.Block == m_pFactory->m_pItemMap)
        {
            m_done = true;
        }
        if (pBlock == NULL && stopAtRealized)
        {
            m_done = true;
        }
        if(0 > itemIndex || itemIndex >= pItems->get_Count())
        {
            m_done = true;
        }
        if (m_done)
        {
            *pIsNewlyRealized = false;
            return NULL;
        }

        ObjectPtr pItem = pItems->GetItemAt(itemIndex);
        if (pBlock != NULL)
        {
            *pIsNewlyRealized = true;

            // TODO -- Group
            /*
            CollectionViewGroup group = item as CollectionViewGroup;
            if ((group == null) || !this._factory.IsGrouping)
            {
            */
                pContainer = m_pFactory->get_Host()->GetContainerForItem(pItem);
                ItemContainerGenerator::LinkContainerToItem(pContainer, pItem);
            /*
            }
            else
            {
                container = this._factory.ContainerForGroup(group);
            }
            */

            if (pContainer != NULL)
            {
                m_pFactory->Realize(pBlock, m_cachedState.Offset, pItem, pContainer);
            }
        }
        else
        {
            pIsNewlyRealized = false;
            pContainer = ((RealizedItemBlock*) (ItemBlock*)m_cachedState.Block)->ContainerAt(m_cachedState.Offset);
        }

        m_cachedState.ItemIndex = itemIndex;
        if (m_direction == GeneratorDirection::Forward)
        {
            m_cachedState.Block->MoveForward(&m_cachedState, true);
        }
        else
        {
            m_cachedState.Block->MoveBackward(&m_cachedState, true);
        }
    }
    return pContainer;
}

 


void ItemContainerGenerator::Generator::OnMapChanged(Object* pSender, MapChangedEventArgs* pArgs)
{
    ItemBlock* pBlock = (ItemBlock*) pArgs->get_Block();
    int offset = pArgs->get_Offset();
    int count = pArgs->get_Count();
    ItemBlock* pNewBlock = (ItemBlock*) pArgs->get_NewBlock();
    int newOffset = pArgs->get_NewOffset();
    int deltaCount = pArgs->get_DeltaCount();

    if (pBlock != NULL)
    {
        if (((pBlock == m_cachedState.Block) && (offset <= m_cachedState.Offset)) && (m_cachedState.Offset < (offset + count)))
        {
            m_cachedState.Block = pNewBlock;
            m_cachedState.Offset += newOffset - offset;
            m_cachedState.Count += deltaCount;
        }
    }
    else if (offset >= 0)
    {
        if (offset < m_cachedState.Count)
        {
            m_cachedState.Count += count;
            m_cachedState.ItemIndex += count;
        }
        else if (offset < (m_cachedState.Count + m_cachedState.Offset))
        {
            m_cachedState.Offset += count;
            m_cachedState.ItemIndex += count;
        }
        else if ((offset == (m_cachedState.Count + m_cachedState.Offset)) && (count > 0))
        {
            m_cachedState.Offset += count;
            m_cachedState.ItemIndex += count;
        }
    }
    else
    {
        m_cachedState.Block = pNewBlock;
        m_cachedState.Offset += m_cachedState.Count;
        m_cachedState.Count = 0;
    }
}


 
 




}; // namespace AVUI
