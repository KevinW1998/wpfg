#pragma once

#include <AVUIObjectCollection.h>
#include <AVUIVisual3D.h>

namespace AVUI {

class Visual3D;

class Visual3DCollection : public TObjectCollection<Visual3D>
{
public:
    DECLARE_ELEMENT(Visual3DCollection, ObjectCollectionBase);

    Visual3DCollection(DependencyObject* pOwner = NULL) : m_pOwner(pOwner)
    {
        
    }

    virtual void OnAdd(Visual3D* pAdd)
    {
        pAdd->AddInheritanceContext(m_pOwner, NULL);
    }

    virtual void OnRemove(Visual3D* pRemove)
    {
        pRemove->RemoveInheritanceContext(m_pOwner, NULL);
    }


private:

    DependencyObject* m_pOwner;

};

}; // namespace AVUI
