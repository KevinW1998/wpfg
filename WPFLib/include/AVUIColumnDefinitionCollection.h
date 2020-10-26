#pragma once

#include <AVUIColumnDefinition.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class Grid;

class ColumnDefinitionCollection : public TObjectCollection<ColumnDefinition>
{
public:
    DECLARE_ELEMENT(ColumnDefinitionCollection, ObjectCollectionBase);
    ColumnDefinitionCollection(Grid* pOwner) : m_pOwner(pOwner) { }

private:
    virtual void OnCollectionChanged();

    virtual void OnAdd(ColumnDefinition* pAdd)
    {
        pAdd->set_Owner(m_pOwner);
        ((UIElement*)m_pOwner)->AddLogicalChild(pAdd);
    }

    virtual void OnRemove(ColumnDefinition* pRemove)
    {
        ((UIElement*)m_pOwner)->RemoveLogicalChild(pRemove);
        pRemove->set_Owner(NULL);
    }


    Grid* m_pOwner;
};


}; // namespace AVUI
