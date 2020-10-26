#pragma once

#include <AVUIGridViewColumn.h>
#include <AVUIObjectCollection.h>
#include <AVUIINotifyCollectionChanged.h>

namespace AVUI {

class GridViewColumn;
class Property;

class GridViewColumnCollectionChangedEventArgs : public CollectionChangedEventArgs
{
public:
    DECLARE_ELEMENT(GridViewColumnCollectionChangedEventArgs, CollectionChangedEventArgs);
    // Methods
    GridViewColumnCollectionChangedEventArgs(GridViewColumn* pColumnIn, const Property* pPropertyIn)
    {
        ActualIndex = -1;
        pColumn = pColumnIn;
        pProperty = pPropertyIn;
    }

    int ActualIndex;
    GridViewColumn* pColumn;
    const Property* pProperty;
};

DEFINE_EVENTHANDLER(GridViewColumnCollectionChanged);



class GridViewColumnCollection : public TObjectCollection<GridViewColumn>
{
    friend class GridViewRowPresenterBase;
public:
    DECLARE_ELEMENT(GridViewColumnCollection, ObjectCollectionBase);

    IMPLEMENT_EVENT(InternalCollectionChanged, CollectionChangedEventArgs);

    GridViewColumnCollection(DependencyObject* pOwner) : m_pOwner(pOwner), m_isImmutable(false), m_inViewMode(false) { }

    bool get_IsImmutable() { return m_isImmutable; }
    void set_IsImmutable(bool isImmutable) { m_isImmutable = isImmutable; }

    bool get_InViewMode() { return m_inViewMode; }
    void set_InViewMode(bool inViewMode) { m_inViewMode = inViewMode; }

protected:
    void OnAdd(GridViewColumn* pAdd);
    void OnRemove(GridViewColumn* pRemove);

private:

    DependencyObject* get_Owner() { return m_pOwner; }
    void set_Owner(DependencyObject* pOwner) { m_pOwner = pOwner; }

    void ColumnPropertyChanged(Object* pSender, PropertyChangedEventArgs* pArgs);

    bool m_isImmutable : 1;
    bool m_inViewMode : 1;
    DependencyObject* m_pOwner;
};


}; // namespace AVUI
