
#pragma once

#include <AVUIEventableObject.h>
#include <AVUIGroupItem.h>

namespace AVUI {

class ItemsChangedEventArgs;
class MapChangedEventArgs;
class ItemsControl;
class ICollection;
class Panel;
class CollectionChangedEventArgs;

struct GeneratorPosition
{
    GeneratorPosition() { }
    GeneratorPosition(int index, int offset) : Index(index), Offset(offset) { }
    int Index;
    int Offset;
};

class ItemsChangedEventArgs : public EventArgs
{
public:
    DECLARE_ELEMENT(ItemsChangedEventArgs, EventArgs);

    ItemsChangedEventArgs(NotifyCollectionChangedAction::Enum action, GeneratorPosition position, int itemCount, int itemUICount) :
                          m_action(action), m_position(position), m_itemCount(itemCount), m_itemUICount(itemUICount)
    {

    }

    ItemsChangedEventArgs(NotifyCollectionChangedAction::Enum action, GeneratorPosition position, GeneratorPosition oldPosition, int itemCount, int itemUICount) :
        m_action(action), m_position(position), m_oldPosition(oldPosition), m_itemCount(itemCount), m_itemUICount(itemUICount)
    {

    }

    NotifyCollectionChangedAction::Enum get_Action() const { return m_action; }
    GeneratorPosition get_Position() const { return m_position; }
    GeneratorPosition get_OldPosition() const { return m_oldPosition; }
    int get_ItemCount() const { return m_itemCount; }
    int get_ItemUICount() const { return m_itemUICount; }
private:
    NotifyCollectionChangedAction::Enum m_action;
    GeneratorPosition m_position;
    GeneratorPosition m_oldPosition;
    int m_itemCount;
    int m_itemUICount;
};

class MapChangedEventArgs : public EventArgs
{
public:
    DECLARE_ELEMENT(MapChangedEventArgs, EventArgs);

    MapChangedEventArgs(Object* pBlock, int offset, int count, Object* pNewBlock, int newOffset, int deltaCount)
    {
        m_pBlock = pBlock;
        m_offset = offset;
        m_count = count;
        m_pNewBlock = pNewBlock;
        m_newOffset = newOffset;
        m_deltaCount = deltaCount;
    }

    Object* get_Block() { return m_pBlock; }
    int get_Offset() { return m_offset; }
    int get_Count() { return m_count; }
    Object* get_NewBlock() { return m_pNewBlock; }
    int get_NewOffset() { return m_newOffset; }
    int get_DeltaCount() { return m_deltaCount; }

private:
    Object* m_pBlock;
    int m_offset;
    int m_count;
    Object* m_pNewBlock;
    int m_newOffset;
    int m_deltaCount;
};


DEFINE_EVENTHANDLER(ItemsChanged);
DEFINE_EVENTHANDLER(MapChanged);

class ItemContainerGenerator : public EventableObject
{
public:
    class ItemBlock;
    class UnrealizedItemBlock;

    DECLARE_ELEMENT(ItemContainerGenerator, Object);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(ItemForItemContainer, Object, NULL, PropertyFlag::IsAttachedProperty);

    ItemContainerGenerator(ItemsControl* pHost);
    ItemContainerGenerator(ItemContainerGenerator* pParent, GroupItem* pGroupItem);
    ItemContainerGenerator(ItemContainerGenerator* pParent, ItemsControl* pHost, DependencyObject* pPeer, int level);
    ~ItemContainerGenerator();

    IMPLEMENT_EVENT(ItemsChanged, ItemsChangedEventArgs);
    IMPLEMENT_EVENT(MapChanged, MapChangedEventArgs);
    IMPLEMENT_EVENT(StatusChanged, EventArgs);

    void Refresh() { OnRefresh(); };
    void RemoveAll();
    void Remove(GeneratorPosition position, int count);
    ItemContainerGenerator* GetItemContainerGeneratorForPanel(Panel* pPanel);
    ObjectPtr StartAt(GeneratorPosition position, GeneratorDirection::Enum direction, bool allowStartAtRealizedItem = false);
    TRefCountedPtr<DependencyObject> GenerateNext(bool* pIsNewlyRealized = NULL);
    void PrepareItemContainer(DependencyObject* pContainer);
    void Realize(UnrealizedItemBlock* pBlock, int offset, Object* pItem, DependencyObject* pContainer); 
    Object* ItemFromContainer(DependencyObject* pContainer);
    int IndexFromContainer(DependencyObject* pContainer);
    int IndexFromGeneratorPosition(GeneratorPosition position);
    GeneratorPosition GeneratorPositionFromIndex(int itemIndex);
    DependencyObject* ContainerFromItem(Object* pItem);
    DependencyObject* ContainerFromIndex(int index);

    GeneratorStatus::Enum get_Status() { return m_status; } 


    struct BlockEntry
    {
        ObjectPtr Item;
        TRefCountedPtr<DependencyObject> Container; 
    };

    // TODO - Empty Group Item
    class EmptyGroupItem : public GroupItem
    {
    public:
        EmptyGroupItem() { }
        void SetGenerator(ItemContainerGenerator* pGenerator);
    private:
        void OnItemsChanged(Object* pSender, ItemsChangedEventArgs* pArgs);
    };

    struct GeneratorState
    {
        TRefCountedPtr<ItemBlock> Block;
        int Count;
        int ItemIndex;
        int Offset;
    };

    class Generator : public Object
    {
    public:
        Generator(ItemContainerGenerator* pFactory, GeneratorPosition position, GeneratorDirection::Enum direction, bool allowStartAtRealizedItem);
        ~Generator();

        TRefCountedPtr<DependencyObject> GenerateNext(bool stopAtRealized, bool *pIsNewlyRealized);
    private:

        void OnMapChanged(Object* pSender, MapChangedEventArgs* pArgs);

        GeneratorState m_cachedState;
        GeneratorDirection::Enum m_direction;
        bool m_done;
        ItemContainerGenerator* m_pFactory;
    };

    class ItemBlock : public Object
    {
    public:
        DECLARE_ELEMENT(ItemBlock, Object);

        ItemBlock();
        ~ItemBlock()
        {
            m_pNext = NULL;
            m_pPrev = NULL;
        }   

        virtual TRefCountedPtr<DependencyObject> ContainerAt(int index) { return NULL; }
        void InsertAfter(ItemBlock* pPrev);
        void InsertBefore(ItemBlock* pPrev);
        void Remove();
        virtual bool IsMoveAllowed(bool allowMovePastRealizedItem) { return allowMovePastRealizedItem; }
        virtual ObjectPtr ItemAt(int index) { return NULL; }
        void MoveBackward(GeneratorState* pState, bool allowMovePastRealizedItem);
        void MoveForward(GeneratorState* pState, bool allowMovePastRealizedItem);

        virtual int get_ContainerCount() { return 0x7fffffff; }
        int get_ItemCount() { return m_count; }
        void set_ItemCount(int newCount) { m_count = newCount; }
        ItemBlock* get_Next() { return m_pNext; }
        void set_Next(ItemBlock* pNext) { m_pNext = pNext; }
        ItemBlock* get_Prev() { return m_pPrev; }
        void set_Prev(ItemBlock* pPrev) { m_pPrev = pPrev; }
    private:
        int m_count;
        TRefCountedPtr<ItemBlock> m_pNext;
        TRefCountedPtr<ItemBlock> m_pPrev;
        static const int BlockSize = 0x10;
    };

    class RealizedItemBlock : public ItemBlock
    {
    public:
        DECLARE_ELEMENT(RealizedItemBlock, ItemBlock);

        RealizedItemBlock();
        void ClearEntries(int offset, int count);
        virtual TRefCountedPtr<DependencyObject> ContainerAt(int index);
        void CopyEntries(RealizedItemBlock* pSrc, int offset, int count, int newOffset);
        virtual ObjectPtr ItemAt(int index);
        int OffsetOfItem(Object* pItem);
        void RealizeItem(int index, Object* pItem, DependencyObject* pContainer);        
        virtual int get_ContainerCount() { return ItemBlock::get_ItemCount(); }

    private:
        ItemStructList<BlockEntry> m_entry;
    };

    class UnrealizedItemBlock : public ItemBlock
    {
    public:
        DECLARE_ELEMENT(UnrealizedItemBlock, ItemBlock);

        UnrealizedItemBlock() { };
        virtual bool IsMoveAllowed(bool allowMovePastRealizedItem) { return true; }
        virtual int get_ContainerCount() { return 0; }
    };

private:

    void OnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs);
    void ValidateAndCorrectIndex(Object* pItem, int* pIndex);
    void OnItemAdded(Object* pItem, int index);
    void OnItemRemoved(Object* pItem, int itemIndex);
    void OnItemReplaced(Object* pOldItem, Object* pNewItem, int index);
    void OnItemMoved(Object* pItem, int oldIndex, int newIndex);

    DependencyObject* GetParentInternal(DependencyObject* pElement);

    bool DoLinearSearch(DependencyObject** ppContainer, Object** ppItem, int* pItemIndex);

    void Initialize(ItemContainerGenerator* pParent, ItemsControl* pHost, DependencyObject* pPeer, int level);
    void OnRefresh();
    void ClearItemMap(ItemBlock* pBlock);
    void PrepareGrouping();
    void UnlinkContainerFromItem(DependencyObject* pContainer, Object* pItem);
    void MoveToPosition(GeneratorPosition position, GeneratorDirection::Enum direction, bool allowStartAtRealizedItem, GeneratorState* pState);
    void MoveItems(ItemBlock* pBlock, int offset, int count, ItemBlock* pNewBlock, int newOffset, int deltaCount);
    void SetStatus(GeneratorStatus::Enum value);
    void RemoveAndCoalesceBlocksIfNeeded(ItemBlock* pBlock);
    int GetRealizedItemBlockCount(RealizedItemBlock* pRIB, int end);
    int GetCount();
    int GetCount(ItemBlock* pStop);
    void GetBlockAndPosition(Object* pItem, int itemIndex, bool deletedFromItems, GeneratorPosition* pPosition, ItemBlock** ppBlock, int * pOffsetFromBlockStart, int *pCorrectIndex);
    void GetBlockAndPosition(int itemIndex, GeneratorPosition* pPosition, ItemBlock** ppBlock, int* pOffsetFromBlockStart);
    void GetBlockAndPosition(Object* pItem, bool deletedFromItems, GeneratorPosition *pPosition, ItemBlock** ppBlock, int *pOffsetFromBlockStart, int* pCorrectIndex);

 
    static void LinkContainerToItem(DependencyObject* pContainer, Object* pItem);

 

    int get_Level() { return m_level; }
    ItemsControl* get_Host() { return m_pHost; }
    ICollection* get_Items() { return m_pItems; }
    void set_Items(Object* pItems);

    ItemStructList<ObjectPtr> m_emptyGroupItems;
    Generator* m_pGenerator;
    // GroupStyle
    ItemsControl* m_pHost;
    TRefCountedPtr<ItemBlock> m_pItemMap;
    ObjectPtr m_pItemsObject;
    ICollection* m_pItems;
    int m_itemsGenerated;
    int m_level;
    ItemContainerGenerator* m_pParent;
    DependencyObject* m_pPeer;
    int m_startIndexForUIFromItem;
    GeneratorStatus::Enum m_status;

};


}; // namespace AVUI
