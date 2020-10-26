#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class ListViewItem;

class ViewBase : public DependencyObject
{
    friend class ListView;
public:
    ViewBase() : m_isUsed(false), m_pInheritanceContext(NULL) { };

    DECLARE_ELEMENT(ViewBase, DependencyObject);

protected:

    virtual DependencyObject* get_InheritanceContext() { return m_pInheritanceContext; }
    virtual void AddInheritanceContext(DependencyObject* pContext, const DependencyProperty* pDP)
    {
        if(m_pInheritanceContext != pContext)
        {
            m_pInheritanceContext = pContext;
            // OnInheritanceContextChanged();
        }
    }
    virtual void RemoveInheritanceContext(DependencyObject* pContext, const DependencyProperty* pDP)
    {
        if (m_pInheritanceContext == pContext)
        {
            m_pInheritanceContext = NULL;
            // OnInheritanceContextChanged(EventArgs.Empty);
        }
    }

    virtual void ClearItem(ListViewItem* pItem) { }
    virtual void PrepareItem(ListViewItem* pItem) { }

    bool get_IsUsed() { return m_isUsed; }
    void set_IsUsed(bool isUsed) { m_isUsed = isUsed; }

private:

    bool m_isUsed;
    DependencyObject* m_pInheritanceContext;
};

 


}; // namespace AVUI

