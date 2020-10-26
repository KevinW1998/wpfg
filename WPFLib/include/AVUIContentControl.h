#pragma once

#include <AVUIControl.h>

namespace AVUI {

class DataTemplate;
class DataTemplateSelector;

class ContentControl : public Control
{
    friend class ItemsControl;

public:
    ContentControl() : m_contentIsNotLogical(false), m_contentIsItem(false) { };
    ~ContentControl() { };

    DECLARE_ELEMENT(ContentControl, Control);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Content, Object, NULL, DependencyPropertyMetadata(PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure, ContentControl::OnContentChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(HasContent, bool, true, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ContentTemplate, DataTemplate, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ContentTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::AffectsMeasure);

protected:

    bool get_ContentIsItem() { return m_contentIsItem; }
    void set_ContentIsItem(bool contentIsItem) { m_contentIsItem = contentIsItem; }

    bool get_ContentIsNotLogical() { return m_contentIsNotLogical; }
    void set_ContentIsNotLogical(bool contentIsNotLogical) { m_contentIsNotLogical = contentIsNotLogical; }

    virtual void OnContentChanged(Object* pOldContent, Object* pNewContent);

private:
    void PrepareContentControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector);
    static void OnContentChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);


    bool m_contentIsNotLogical;
    bool m_contentIsItem;
};

 


}; // namespace AVUI

