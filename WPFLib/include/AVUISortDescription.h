#pragma once

#include <AVUIValueTypeCollection.h>
#include <AVUIIComparer.h>

namespace AVUI {

class ListSortDirection
{
public:
    enum Enum
    {
        Ascending,
        Descending
    };
};
 
struct SortDescription
{
public:
    DECLARE_VALUE_ELEMENT(SortDescription);

    SortDescription() { }
    SortDescription(String* pPropertyName, ListSortDirection::Enum sortDirection) : m_pPropertyName(pPropertyName), m_sortDirection(sortDirection), m_isSealed(false)
    {

    }

    ListSortDirection::Enum get_Direction() { return m_sortDirection; }
    void set_Direction(ListSortDirection::Enum sortDirection) { if(m_isSealed) LibraryCriticalError(); m_sortDirection = sortDirection; }

    bool get_IsSealed() { return m_isSealed; }

    String* get_PropertyName() { return m_pPropertyName; }
    void set_PropertyName(String* pPropertyName) { if(m_isSealed) LibraryCriticalError(); m_pPropertyName = pPropertyName; }

    bool operator==(const SortDescription& rhs) const { return m_pPropertyName == rhs.m_pPropertyName && m_sortDirection == rhs.m_sortDirection; }

private:
    void Seal() { m_isSealed = true; }

    ListSortDirection::Enum m_sortDirection;
    TRefCountedPtr<String> m_pPropertyName;
    bool m_isSealed;
};

class SortDescriptionCollection : public ValueTypeCollection<SortDescription>, public INotifyCollectionChanged
{
public:
    SortDescriptionCollection() { set_NotifyOnChange(true); }

    IMPLEMENT_EVENT(CollectionChanged, CollectionChangedEventArgs);

    static SortDescriptionCollection* get_Empty();

protected:

    virtual void OnItemRemoved(int index);
    virtual void OnItemAdded(int insertPosition, const SortDescription& item);
    virtual void OnItemMoved(int oldIndex, int newIndex);
    virtual void OnCleared();

private:

    void AddEventHandler(const void* pvKey, Delegate* pEventHandler)
    {
        EventHelper::AddEventHandler(&m_eventHandlerList, pEventHandler);
    }
    void RemoveEventHandler(const void* pvKey, Delegate* pEventHandler)
    {
        EventHelper::RemoveEventHandler(&m_eventHandlerList, pEventHandler);
    }
    void InvokeEventHandler(const void* pvKey, Object* pSender, Object* pArgs)
    {
        EventHelper::InvokeEventHandler(&m_eventHandlerList, pSender, pArgs);
    }

    EventHandlerList m_eventHandlerList;
};

class EmptySortDescriptionCollection : public SortDescriptionCollection
{

};

class PropertyPath;

class SortFieldComparer : public Object, public IComparer
{
private:
    struct SortPropertyInfo
    {
    public:
        SortPropertyInfo() : m_descending(false) { }
    
        TRefCountedPtr<Object> GetValue(Object* pObject);
    
        bool m_descending;
        TRefCountedPtr<PropertyPath> m_pInfo;
    };

public:
    SortFieldComparer(SortDescriptionCollection* pSortFields)
    {
        m_pSortFields = pSortFields;
        CreatePropertyInfo(m_pSortFields);
    }

    int Compare(Object* pO1, Object* pO2);
    int ComparerCompare(Object* pO1, Object* pO2);

private:

    void CreatePropertyInfo(SortDescriptionCollection* pSortFields);

    TRefCountedPtr<SortDescriptionCollection> m_pSortFields;
    ItemStructList<SortPropertyInfo> m_fields;
};


}; // namespace AVUI

