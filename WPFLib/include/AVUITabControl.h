#pragma once

#include <AVUISelector.h>

namespace AVUI {

class TabItem;
class ContentPresenter;


// TODO -- Determine, how does TabPanel get invalidated to recalculate which item is at what level?
class TabControl : public Selector
{
public:
    TabControl() { };
    ~TabControl() { };

    DECLARE_ELEMENT(TabControl, Selector);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(TabStripPlacement, Dock, Dock::Top, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(SelectedContent, Object, NULL, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(SelectedContentTemplate, DataTemplate, NULL, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(SelectedContentTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ContentTemplate, DataTemplate, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ContentTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::None);

protected:

    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();
    virtual void OnApplyTemplate();
    virtual void OnInitialized(EventArgs* pEventArgs);
    virtual void OnSelectionChanged(SelectionChangedEventArgs* pArgs);

private:
    ContentPresenter* get_SelectedContentPresenter();
    void UpdateSelectedContent();
    TabItem* GetSelectedTabItem();
    void OnGeneratorStatusChanged(Object* pSender, EventArgs* e);

     


};

}; // namespace AVUI
