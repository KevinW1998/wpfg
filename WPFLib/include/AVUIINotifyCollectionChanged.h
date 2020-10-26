#pragma once

namespace AVUI
{

class CollectionChangedEventArgs : public EventArgs
{
public:
    DECLARE_ELEMENT(CollectionChangedEventArgs, EventArgs);

    CollectionChangedEventArgs() 
    {

    }

    CollectionChangedEventArgs(NotifyCollectionChangedAction::Enum action) : m_action(action), m_oldStartingIndex(-1), m_newStartingIndex(-1)
    {

    }

    CollectionChangedEventArgs(NotifyCollectionChangedAction::Enum action, Object* pOldValue, Object* pNewValue, int index) : m_action(action), m_oldStartingIndex(index), m_newStartingIndex(index), m_pOldItem(pOldValue), m_pNewItem(pNewValue)
    {

    }

    CollectionChangedEventArgs(NotifyCollectionChangedAction::Enum action, Object* pChangedItem, int oldStartingIndex, int newStartingIndex) : m_action(action), m_oldStartingIndex(oldStartingIndex), m_newStartingIndex(newStartingIndex), m_pOldItem(pChangedItem), m_pNewItem(pChangedItem)
    {

    }

    CollectionChangedEventArgs(NotifyCollectionChangedAction::Enum action, Object* pChangedItem, int index) : m_action(action), m_oldStartingIndex(-1), m_newStartingIndex(-1)
    {
        if(action == NotifyCollectionChangedAction::Add)
        {
            m_pNewItem = pChangedItem;
            m_newStartingIndex = index;
        }
        else if(action == NotifyCollectionChangedAction::Remove)
        {
            m_pOldItem = pChangedItem;
            m_oldStartingIndex = index;
        }
    }


    NotifyCollectionChangedAction::Enum get_Action() const { return m_action; }
    ObjectPtr get_OldItem() { return m_pOldItem; }
    int get_OldStartingIndex() { return m_oldStartingIndex; }
    ObjectPtr get_NewItem() { return m_pNewItem; }
    int get_NewStartingIndex() { return m_newStartingIndex; }

private:
    NotifyCollectionChangedAction::Enum m_action;

    ObjectPtr m_pOldItem;
    ObjectPtr m_pNewItem;
    int m_oldStartingIndex;
    int m_newStartingIndex;
};

class INotifyCollectionChanged
{
public:
    virtual void add_CollectionChanged(TDelegate<CollectionChangedEventArgs>* pEventHandler) = 0;
    virtual void remove_CollectionChanged(TDelegate<CollectionChangedEventArgs>* pEventHandler) = 0;
};

typedef AVUI::TDelegate<CollectionChangedEventArgs> NotifyCollectionChangedEventHandler;

};


