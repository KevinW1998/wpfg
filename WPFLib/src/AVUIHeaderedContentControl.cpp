#include <AVUICommon.h>
#include <AVUIHeaderedContentControl.h>
#include <AVUIDataTemplate.h>
#include <AVUIDataTemplateSelector.h>

REGISTER_ELEMENT_BEGIN(AVUI::HeaderedContentControl)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Header)
    REGISTER_PROPERTY(HeaderTemplate)
    REGISTER_PROPERTY(HeaderTemplateSelector)
REGISTER_ELEMENT_END()

namespace AVUI {


void HeaderedContentControl::PrepareHeaderedContentControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector)
{
    if (pItem != this)
    {
        bool itemIsVisual = false;
        if(pItem != NULL && pItem->IsType(GET_TYPE(Visual)))
        {
            itemIsVisual = true;
        }

        set_ContentIsNotLogical(true);
        set_HeaderIsNotLogical(true);
        if (get_ContentIsItem() || !HasNonDefaultValue(ContentControl::get_ContentProperty()))
        {
            set_Content(pItem);
            set_ContentIsItem(true);
        }
        if ( (!itemIsVisual && get_HeaderIsItem()) || !HasNonDefaultValue(get_HeaderProperty()))
        {
            set_Header(pItem);
            set_HeaderIsItem(true);
        }
        if (pItemTemplate != NULL)
        {
            SetValue(get_HeaderTemplateProperty(), pItemTemplate);
        }
        if (pItemTemplateSelector != NULL)
        {
            SetValue(get_HeaderTemplateSelectorProperty(), pItemTemplateSelector);
        }
    }
    else
    {
        set_ContentIsNotLogical(false);
    }
}

void HeaderedContentControl::OnHeaderChangedStatic(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    HeaderedContentControl* pControl = (HeaderedContentControl*) pDO;
    pControl->SetValue(get_HasHeaderProperty(), (e.pNewValue != NULL) ? BoxValue(true) : BoxValue(false));
    pControl->OnHeaderChanged(e.pOldValue, e.pNewValue);
}

void HeaderedContentControl::OnHeaderChanged(Object* pOldHeader, Object* pNewHeader)
{
    RemoveLogicalChild(pOldHeader);
    AddLogicalChild(pNewHeader);
}
 

 



}; // namespace AVUI

