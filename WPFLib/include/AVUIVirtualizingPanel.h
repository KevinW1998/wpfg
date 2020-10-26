#pragma once

#include <AVUIPanel.h>

namespace AVUI {

class ItemContainerGenerator;

class VirtualizingPanel : public Panel
{
public:
    VirtualizingPanel() : Panel(true) { };
    ~VirtualizingPanel() { };

    DECLARE_ELEMENT(VirtualizingPanel, Panel);

    ItemContainerGenerator* get_ItemContainerGenerator() { return m_pItemContainerGenerator; }

    static void RemoveInternalChildRange(UIElementCollection* pChildren, int index, int range);

protected:

    void AddChild(UIElement* pChild);
    void InsertChild(int index, UIElement* pChild);
    void RemoveChildRange(int index, int range);

    virtual void OnClearChildren() { }
    virtual void BringIndexIntoView(int index) { };

    virtual void OnClearChildrenInternal() { OnClearChildren(); }
    virtual void OnItemsChanged(Object* pSender, ItemsChangedEventArgs* pArgs) { }
    virtual void OnItemsChangedInternal(Object* pSender, ItemsChangedEventArgs* pArgs);

};

}; // namespace AVUI
