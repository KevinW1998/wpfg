#include <AVUICommon.h>
#include <AVUIContentControl.h>
#include <AVUIDataTemplate.h>
#include <AVUIDataTemplateSelector.h>

REGISTER_ELEMENT_BEGIN(AVUI::ContentControl)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Content)
    REGISTER_PROPERTY(ContentTemplate)
    REGISTER_PROPERTY(ContentTemplateSelector)
REGISTER_ELEMENT_END()

namespace AVUI {


void ContentControl::PrepareContentControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector)
{
    if (pItem != this)
    {
        set_ContentIsNotLogical(true);
        if (get_ContentIsItem() || !HasNonDefaultValue(get_ContentProperty()))
        {
            set_Content(pItem);
            set_ContentIsItem(true);
        }
        if (pItemTemplate != NULL)
        {
            SetValue(get_ContentTemplateProperty(), pItemTemplate);
        }
        if (pItemTemplateSelector != NULL)
        {
            SetValue(get_ContentTemplateSelectorProperty(), pItemTemplateSelector);
        }
    }
    else
    {
        set_ContentIsNotLogical(false);
    }
}

void ContentControl::OnContentChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ContentControl* pControl = (ContentControl*) pDO;
    pControl->SetValue(get_HasContentProperty(), (e.pNewValue != NULL) ? BoxValue(true) : BoxValue(false));
    pControl->OnContentChanged(e.pOldValue, e.pNewValue);
}
 

void ContentControl::OnContentChanged(Object* pOldContent, Object* pNewContent)
{
    RemoveLogicalChild(pOldContent);
    if (!get_ContentIsNotLogical())
    {
        if (get_TemplatedParent() != NULL)
        {
            UIElement* pCurrent = object_cast<UIElement>(pNewContent);
            if (pCurrent != NULL && pCurrent->get_Parent() != NULL)
            {
                return;
            }
        }
        AddLogicalChild(pNewContent);
    }
}

 


 


}; // namespace AVUI
