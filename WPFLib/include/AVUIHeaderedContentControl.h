#pragma once

#include <AVUIContentControl.h>

namespace AVUI {

class StyleSelector;

class HeaderedContentControl : public ContentControl
{
    friend class ItemsControl;
public:
    HeaderedContentControl() : m_headerIsNotLogical(false), m_headerIsItem(false) { };
    ~HeaderedContentControl() { };

    DECLARE_ELEMENT(HeaderedContentControl, ContentControl);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Header, Object, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, &HeaderedContentControl::OnHeaderChangedStatic));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(HasHeader, bool, true, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(HeaderTemplate, DataTemplate, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(HeaderTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::AffectsMeasure);

protected:

    virtual void OnHeaderChanged(Object* pOldHeader, Object* pNewHeader);


private:

    bool get_HeaderIsItem() const { return m_headerIsItem; }
    void set_HeaderIsItem(bool headerIsItem) { m_headerIsItem = headerIsItem; }

    bool get_HeaderIsNotLogical() const { return m_headerIsNotLogical; }
    void set_HeaderIsNotLogical(bool headerIsNotLogical) { m_headerIsNotLogical = headerIsNotLogical; }

    void PrepareHeaderedContentControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector);

    static void OnHeaderChangedStatic(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    bool m_headerIsNotLogical;
    bool m_headerIsItem;

};

}; // namespace AVUI
