#pragma once

#include <AVUIObject.h>

namespace AVUI {

class DependencyObject;

class DeferredElementTreeState : public Object
{
public:
    DeferredElementTreeState() { };
    ~DeferredElementTreeState() { };

    DECLARE_ELEMENT(DeferredElementTreeState, Object);

    void Clear() { m_coreParents.Clear(); m_logicalParents.Clear(); }

    static DependencyObject* GetCoreParent(DependencyObject* pObject, DeferredElementTreeState* pTreeState);
    static DependencyObject* GetLogicalParent(DependencyObject* pObject, DeferredElementTreeState* pTreeState);
    void SetCoreParent(DependencyObject* pElement, DependencyObject* pParent);
    void SetLogicalParent(DependencyObject* pElement, DependencyObject* pParent);

    bool get_IsEmpty() { return m_coreParents.get_Count() == 0 && m_logicalParents.get_Count() == 0; }

private:

    struct DeferredElementEntry
    {
        DeferredElementEntry() { }
        DeferredElementEntry(DependencyObject* pElementIn, DependencyObject* pParentIn) : pElement(pElementIn), pParent(pParentIn) { }
        TRefCountedPtr<DependencyObject> pElement;
        TRefCountedPtr<DependencyObject> pParent;
    };

    ItemStructList<DeferredElementEntry> m_coreParents;
    ItemStructList<DeferredElementEntry> m_logicalParents;
};

}; // namespace AVUI

