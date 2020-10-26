#pragma once

#include <AVUIRowDefinition.h>
#include <AVUIObjectCollection.h>
#include <AVUIGrid.h>

namespace AVUI {

class RowDefinitionCollection : public TObjectCollection<RowDefinition>
{
public:
    DECLARE_ELEMENT(RowDefinitionCollection, ObjectCollectionBase);
    RowDefinitionCollection(Grid* pOwner) : m_pOwner(pOwner) { }

private:

    virtual void OnCollectionChanged();

    virtual void OnAdd(RowDefinition* pAdd)
    {
        pAdd->set_Owner(m_pOwner);
        ((UIElement*)m_pOwner)->AddLogicalChild(pAdd);
    }

    virtual void OnRemove(RowDefinition* pRemove)
    {
        ((UIElement*)m_pOwner)->RemoveLogicalChild(pRemove);
        pRemove->set_Owner(NULL);
    }

private:
    Grid* m_pOwner;
};


}; // namespace AVUI
