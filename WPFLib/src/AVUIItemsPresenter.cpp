#include <AVUICommon.h>
#include <AVUIPanel.h>
#include <AVUIItemsControl.h>
#include <AVUIItemsPresenter.h>
#include <AVUIItemsPanelTemplate.h>


REGISTER_ELEMENT_BEGIN(AVUI::ItemsPresenter)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Template)
REGISTER_ELEMENT_END()

namespace AVUI {


void ItemsPresenter::OnPreApplyTemplate()
{
    UIElement::OnPreApplyTemplate();
    AttachToOwner();
}


Size ItemsPresenter::MeasureOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Measure(constraintSize);
            return pVisualChild->get_DesiredSize();
        }
    }
    return Size(0, 0);
}

Size ItemsPresenter::ArrangeOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Arrange(Rect(constraintSize));
        }
    }
    return constraintSize;
}


void ItemsPresenter::OnApplyTemplate()
{

}

void ItemsPresenter::AttachToOwner()
{
    TRefCountedPtr<ItemContainerGenerator> pItemContainerGenerator;

    ItemsControl* pOwner = object_cast<ItemsControl>(get_TemplatedParent());
    if (pOwner != NULL)
    {
        pItemContainerGenerator = pOwner->get_ItemContainerGenerator();
    }
    else
    {
        /*
        // TODO -- group
        GroupItem groupItem = base.TemplatedParent as GroupItem;
        ItemsPresenter presenter = FromGroupItem(groupItem);
        if (presenter != null)
        {
            owner = presenter.Owner;
        }
        itemContainerGenerator = (groupItem != null) ? groupItem.Generator : null;
        */
    }
    m_pOwner = pOwner;
    UseGenerator(pItemContainerGenerator);

    /*
    // TODO -- Group Style
    GroupStyle style = (this._generator != null) ? this._generator.GroupStyle : null;
    this.Template = (style != null) ? style.Panel : ((this._owner != null) ? this._owner.ItemsPanel : null);
    */

    set_Template(m_pOwner->get_ItemsPanel());
}

void ItemsPresenter::UseGenerator(ItemContainerGenerator* pGenerator)
{
    /*
    // todo -- onpanelchanged
    if (pGenerator != m_pGenerator)
    {
        if (m_pGenerator != NULL)
        {
            m_pGenerator->remove_PanelChanged(EventHandler::Create(this, ItemsPresenter::OnPanelChanged));
        }
    */
        m_pGenerator = pGenerator;
    /*
        if (m_pGenerator != NULL)
        {
            m_pGenerator->add_PanelChanged(EventHandler::Create(this, &ItemsPresenter::OnPanelChanged));
        }
    }
    */
}

 
ItemsPresenter* ItemsPresenter::FromPanel(Panel* pPanel)
{
    if (pPanel == NULL)
    {
        return NULL;
    }
    return object_cast<ItemsPresenter>(pPanel->get_TemplatedParent());
}

 
 


}; // namespace AVUI
