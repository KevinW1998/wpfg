#pragma once

#include <AVUIDataTemplate.h>

namespace AVUI {

class BindingBase;
class DataTemplateSelector;

class HierarchicalDataTemplate : public DataTemplate
{
    friend class HeaderedItemsControl;
public:
    HierarchicalDataTemplate() : m_itemTemplateSelectorSet(false), m_itemTemplateSet(false) { };

    DECLARE_ELEMENT(HierarchicalDataTemplate, DataTemplate);

    IMPLEMENT_REFERENCE_PROPERTY(ItemsSource, BindingBase, false);
    IMPLEMENT_REFERENCE_PROPERTY(ItemTemplate, DataTemplate, false);
    IMPLEMENT_REFERENCE_PROPERTY(ItemTemplateSelector, DataTemplateSelector, false);

    BindingBase* get_ItemsSource() { return m_pItemsSourceBinding; }
    void set_ItemsSource(BindingBase* pBinding) { CheckSealed(); m_pItemsSourceBinding = pBinding; }

    DataTemplate* get_ItemTemplate() { return m_pItemTemplate; }
    void set_ItemTemplate(DataTemplate* pTemplate) { CheckSealed(); m_pItemTemplate = pTemplate; m_itemTemplateSet = true; }

    DataTemplateSelector* get_ItemTemplateSelector() { return m_pItemTemplateSelector; }
    void set_ItemTemplateSelector(DataTemplateSelector* pTemplateSelector) { CheckSealed(); m_pItemTemplateSelector = pTemplateSelector; m_itemTemplateSelectorSet = true; }


private:

    bool get_IsItemTemplateSet() { return m_itemTemplateSet; }
    bool get_IsItemTemplateSelectorSet() { return m_itemTemplateSelectorSet; }

    TRefCountedPtr<BindingBase> m_pItemsSourceBinding;
    TRefCountedPtr<DataTemplate> m_pItemTemplate;
    TRefCountedPtr<DataTemplateSelector> m_pItemTemplateSelector;
    bool m_itemTemplateSelectorSet;
    bool m_itemTemplateSet;

};



}; // namespace AVUI

