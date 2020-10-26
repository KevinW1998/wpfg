#include <AVUICommon.h>
#include <AVUITabControl.h>
#include <AVUITabItem.h>
#include <AVUIItemCollection.h>
#include <AVUIContentPresenter.h>
#include <AVUIDataTemplate.h>
#include <AVUIDataTemplateSelector.h>
#include <AVUIItemContainerGenerator.h>

REGISTER_ELEMENT_BEGIN(AVUI::TabControl)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(TabStripPlacement)
    REGISTER_PROPERTY(SelectedContent)
    REGISTER_PROPERTY(SelectedContentTemplate)
    REGISTER_PROPERTY(SelectedContentTemplateSelector)
    REGISTER_PROPERTY(ContentTemplate)
    REGISTER_PROPERTY(ContentTemplateSelector)
REGISTER_ELEMENT_END()


namespace AVUI
{

bool TabControl::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem->IsType(GET_TYPE(TabItem));
}

TRefCountedPtr<DependencyObject> TabControl::GetContainerForItemOverride()
{
    return (DependencyObject*) object_allocate<TabItem>();
}

void TabControl::OnApplyTemplate()
{
    Selector::OnApplyTemplate();
    UpdateSelectedContent();
}


void TabControl::UpdateSelectedContent()
{
    if (get_SelectedIndex() < 0)
    {
        SetValue(get_SelectedContentProperty(), NULL);
        SetValue(get_SelectedContentTemplateProperty(), NULL);
        SetValue(get_SelectedContentTemplateSelectorProperty(), NULL);
    }
    else
    {
        TabItem* pReference = GetSelectedTabItem();
        if (pReference != NULL)
        {
            // TODO -- Focus updates
            /*
            FrameworkElement d = VisualTreeHelper.GetParent(reference) as FrameworkElement;
            if (d != null)
            {
                KeyboardNavigation.SetTabOnceActiveElement(d, reference);
                KeyboardNavigation.SetTabOnceActiveElement(this, d);
            }
            */

            SetValue(get_SelectedContentProperty(), pReference->get_Content());
            ContentPresenter* pSelectedContentPresenter = get_SelectedContentPresenter();
            if (pSelectedContentPresenter != NULL)
            {
                pSelectedContentPresenter->set_HorizontalAlignment(pReference->get_HorizontalContentAlignment());
                pSelectedContentPresenter->set_VerticalAlignment(pReference->get_VerticalContentAlignment());
            }
            if (pReference->get_ContentTemplate() != NULL || pReference->get_ContentTemplateSelector() != NULL)
            {
                SetValue(get_SelectedContentTemplateProperty(), pReference->get_ContentTemplate());
                SetValue(get_SelectedContentTemplateSelectorProperty(), pReference->get_ContentTemplateSelector());
            }
            else
            {
                SetValue(get_SelectedContentTemplateProperty(), get_ContentTemplate());
                SetValue(get_SelectedContentTemplateSelectorProperty(), get_ContentTemplateSelector());
            }
        }
    }
}


TabItem* TabControl::GetSelectedTabItem()
{
    Object* pSelectedItem = get_SelectedItem();
    if (pSelectedItem == NULL)
    {
        return NULL;
    }
    TabItem* pItem = object_cast<TabItem>(pSelectedItem);
    if (pItem == NULL)
    {
        pItem = object_cast<TabItem>(get_ItemContainerGenerator()->ContainerFromIndex(get_SelectedIndex()));
    }

    return pItem;
}

ContentPresenter* TabControl::get_SelectedContentPresenter()
{
    return object_cast<ContentPresenter>(FindTemplateChild(L"PART_SelectedContentHost"));
}
 
void TabControl::OnInitialized(EventArgs* pEventArgs)
{
    Selector::OnInitialized(pEventArgs);

    set_CanSelectMultiple(false);
    get_ItemContainerGenerator()->add_StatusChanged(EventHandler::Create(this, &TabControl::OnGeneratorStatusChanged));
}

void TabControl::OnGeneratorStatusChanged(Object* pSender, EventArgs* e)
{
    if (get_ItemContainerGenerator()->get_Status() == GeneratorStatus::ContainersGenerated)
    {
        if (get_HasItems() && m_selectedItems.get_Count() == 0)
        {
            set_SelectedIndex(0);
        }
        UpdateSelectedContent();
    }
}
 
void TabControl::OnSelectionChanged(SelectionChangedEventArgs* pArgs)
{
    Selector::OnSelectionChanged(pArgs);

    // TODO -- Focus
    /*
    if (base.IsKeyboardFocusWithin)
    {
        TabItem selectedTabItem = this.GetSelectedTabItem();
        if (selectedTabItem != null)
        {
            selectedTabItem.SetFocus();
        }
    }
    */
    UpdateSelectedContent();
}

 

 



};

