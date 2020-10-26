#pragma once

#include <AVUIItemsControl.h>
#include <AVUIHeaderedContentControl.h>

namespace AVUI {

class HeaderedItemsControl : public ItemsControl
{
    friend class ItemsControl;
public:
    HeaderedItemsControl() : m_boolFieldStore(0) { };

    DECLARE_ELEMENT(HeaderedItemsControl, ItemsControl);

    static const AVUI::DependencyProperty& get_HeaderProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, OnHeaderChanged));
        const static AVUI::DependencyProperty& property = HeaderedContentControl::get_HeaderProperty().AddOwner(GET_TYPE(_thisType), NULL, &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_HasHeaderProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(PropertyFlag::None);
        const static AVUI::DependencyProperty& property = HeaderedContentControl::get_HasHeaderProperty().AddOwner(GET_TYPE(_thisType), NULL, &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_HeaderTemplateProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, OnHeaderTemplateChanged));
        const static AVUI::DependencyProperty& property = HeaderedContentControl::get_HeaderTemplateProperty().AddOwner(GET_TYPE(_thisType), NULL, &metadata);

        return property;
    }

    static const AVUI::DependencyProperty& get_HeaderTemplateSelectorProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, OnHeaderTemplateSelectorChanged));
        const static AVUI::DependencyProperty& property = HeaderedContentControl::get_HeaderTemplateProperty().AddOwner(GET_TYPE(_thisType), NULL, &metadata);

        return property;
    }


    bool get_HasHeader() { return UnboxValue<bool>(GetValue(get_HasHeaderProperty())); }

    ObjectPtr get_Header() { return GetValue(get_HeaderProperty()); }
    void set_Header(Object* pHeader) { SetValue(get_HeaderProperty(), pHeader); }

    TRefCountedPtr<DataTemplate> get_HeaderTemplate() { return (DataTemplate*) (Object*)GetValue(get_HeaderTemplateProperty()); }
    void set_HeaderTemplate(DataTemplate* pHeaderTemplate) { SetValue(get_HeaderTemplateProperty(), (Object*)pHeaderTemplate); }

    TRefCountedPtr<DataTemplateSelector> get_HeaderTemplateSelector() { return (DataTemplateSelector*) (Object*) GetValue(get_HeaderTemplateSelectorProperty()); }
    void set_HeaderTemplateSelector(DataTemplateSelector* pHeaderTemplateSelector) { SetValue(get_HeaderTemplateSelectorProperty(), (Object*)pHeaderTemplateSelector); }

protected:

    virtual void OnHeaderChanged(Object* pOldHeader, Object* pNewHeader);
    virtual void OnHeaderTemplateChanged(DataTemplate* pOldHeaderTemplate, DataTemplate* pNewHeaderTemplate) { }
    virtual void OnHeaderTemplateSelectorChanged(DataTemplateSelector* pOldHeaderTemplateSelector, DataTemplateSelector* pNewHeaderTemplateSelector) { }

private:
    bool IsHeaderLogical();

    void PrepareHeaderedItemsControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector, Style* pItemContainerStyle, StyleSelector* pItemContainerStyleSelector);
    void PrepareHierarchy(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector);

    static void OnHeaderChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
    {
        HeaderedItemsControl* pControl = (HeaderedItemsControl*) pDO;
        pControl->SetValue(get_HasHeaderProperty(), (e.pNewValue != NULL) ? BoxValue(true) : BoxValue(false));
        pControl->OnHeaderChanged(e.pOldValue, e.pNewValue);
    }

    static void OnHeaderTemplateChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
    {
        ((HeaderedItemsControl*)pDO)->OnHeaderTemplateChanged((DataTemplate*) e.pOldValue, (DataTemplate*) e.pNewValue);
    }

    static void OnHeaderTemplateSelectorChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
    {
        ((HeaderedItemsControl*)pDO)->OnHeaderTemplateSelectorChanged((DataTemplateSelector*) e.pOldValue, (DataTemplateSelector*) e.pNewValue);
    }

    class BoolField
    {
    public:
        enum Enum
        {
            HeaderIsItem = 4,
            HeaderIsNotLogical = 8,
            ItemsSourceSet = 0x10,
            ItemTemplateSelectorSet = 0x40,
            ItemTemplateSet = 0x20
        };
    };

    bool GetBoolField(unsigned int field)
    {
        return (m_boolFieldStore & field) != 0;
    }
    void SetBoolField(unsigned int field, bool value)
    {
        if (value)
        {
            m_boolFieldStore |= field;
        }
        else
        {
            m_boolFieldStore &= ~field;
        }
    }

    bool get_HeaderIsItem()
    {
        return GetBoolField(BoolField::HeaderIsItem);
    }
    void set_HeaderIsItem(bool headerIsItem)
    {
        SetBoolField(BoolField::HeaderIsItem, headerIsItem);
    }

    unsigned int m_boolFieldStore;

};



}; // namespace AVUI

