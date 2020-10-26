#include <AVUICommon.h>
#include <AVUIHeaderedItemsControl.h>
#include <AVUIDataTemplateSelector.h>
#include <AVUIHierarchicalDataTemplate.h>
#include <AVUIBindingOperations.h>


REGISTER_ELEMENT_BEGIN(AVUI::HeaderedItemsControl)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Header)
    REGISTER_PROPERTY(HeaderTemplate)
    REGISTER_PROPERTY(HeaderTemplateSelector)
REGISTER_ELEMENT_END()


namespace AVUI {

void HeaderedItemsControl::PrepareHeaderedItemsControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector, Style* pItemContainerStyle, StyleSelector* pItemContainerStyleSelector)
{
    bool flag = pItem != this;
    SetBoolField(BoolField::HeaderIsNotLogical, flag);
    PrepareItemsControl(pItem, pItemTemplate, pItemTemplateSelector, pItemContainerStyle, pItemContainerStyleSelector);
    if (flag)
    {
        if (get_HeaderIsItem() || !HasNonDefaultValue(get_HeaderProperty()))
        {
            set_Header(pItem);
            set_HeaderIsItem(true);
        }
        if (pItemTemplate != NULL)
        {
            SetValue(get_HeaderTemplateProperty(), (Object*)pItemTemplate);
        }
        if (pItemTemplateSelector != NULL)
        {
            SetValue(get_HeaderTemplateSelectorProperty(), (Object*)pItemTemplateSelector);
        }
        PrepareHierarchy(pItem, pItemTemplate, pItemTemplateSelector);
    }
}

void HeaderedItemsControl::PrepareHierarchy(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector)
{
    TRefCountedPtr<DataTemplate> pHeaderTemplate = get_HeaderTemplate();
    if (pHeaderTemplate == NULL)
    {
        TRefCountedPtr<DataTemplateSelector> pHeaderTemplateSelector = get_HeaderTemplateSelector();
        if (pHeaderTemplateSelector != NULL)
        {
            pHeaderTemplate = pHeaderTemplateSelector->SelectTemplate(pItem, this);
        }
    }
    /*
    if (pHeaderTemplate == null)
    {
        headerTemplate = (DataTemplate) FrameworkElement.FindTemplateResourceInternal(this, item, typeof(DataTemplate));
        if (headerTemplate != null)
        {
            base.ClearValue(HeaderTemplateProperty);
            this.HeaderTemplate = headerTemplate;
        }
    }
    */

    HierarchicalDataTemplate* pTemplate2 = object_cast<HierarchicalDataTemplate>(pHeaderTemplate);
    if (pTemplate2 != NULL)
    {
        bool flag = get_ItemTemplate() == pItemTemplate;
        if (pTemplate2->get_ItemsSource() != NULL && !HasNonDefaultValue(ItemsControl::get_ItemsSourceProperty()))
        {
            SetBinding(ItemsControl::get_ItemsSourceProperty(), pTemplate2->get_ItemsSource());
            SetBoolField(BoolField::ItemsSourceSet, true);
        }
        if (pTemplate2->get_IsItemTemplateSelectorSet() && get_ItemTemplateSelector() == pItemTemplateSelector)
        {
            ClearValue(ItemsControl::get_ItemTemplateProperty());
            SetBoolField(BoolField::ItemTemplateSet, false);
            ClearValue(ItemsControl::get_ItemTemplateSelectorProperty());
            bool flag2 = pTemplate2->get_ItemTemplateSelector() != NULL;
            if (flag2)
            {
                set_ItemTemplateSelector(pTemplate2->get_ItemTemplateSelector());
            }
            SetBoolField(BoolField::ItemTemplateSelectorSet, flag2);
        }
        if (pTemplate2->get_IsItemTemplateSet() && flag)
        {
            ClearValue(ItemsControl::get_ItemTemplateProperty());
            bool flag3 = pTemplate2->get_ItemTemplate() != NULL;
            if (flag3)
            {
                set_ItemTemplate(pTemplate2->get_ItemTemplate());
            }
            SetBoolField(BoolField::ItemTemplateSet, flag3);
        }
    }
}

void HeaderedItemsControl::OnHeaderChanged(Object* pOldHeader, Object* pNewHeader)
{
    if (IsHeaderLogical())
    {
        RemoveLogicalChild(pOldHeader);
        AddLogicalChild(pNewHeader);
    }
}

bool HeaderedItemsControl::IsHeaderLogical()
{
    if (GetBoolField(BoolField::HeaderIsNotLogical))
    {
        return false;
    }
    if (BindingOperations::IsDataBound(this, &get_HeaderProperty()))
    {
        SetBoolField(BoolField::HeaderIsNotLogical, true);
        return false;
    }
    return true;
}

 

 


}; // namespace AVUI
