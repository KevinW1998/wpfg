#pragma once

#include <AVUIContentControl.h>
#include <AVUISelector.h>

namespace AVUI {

class ListBox;

class ListBoxItem : public ContentControl
{
    friend class ListBox;
public:
    ListBoxItem()  { };
    ~ListBoxItem() { };

    DECLARE_ELEMENT(ListBoxItem, ContentControl);

    static const AVUI::DependencyProperty& get_IsSelectedProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, OnIsSelectedChanged));
        const static AVUI::DependencyProperty& property = Selector::get_IsSelectedProperty().AddOwner(GET_TYPE(_thisType), BoxValue(false), &metadata);

        return property;
    }
    bool get_IsSelected() { return UnboxValue< bool >(GetValue(get_IsSelectedProperty())); }
    void set_IsSelected(const bool& value) { SetValue(get_IsSelectedProperty(), AVUI::BoxValue(value)); }


    ALIAS_ROUTED_EVENT(Selector, Selected, RoutedEventArgs);
    ALIAS_ROUTED_EVENT(Selector, Unselected, RoutedEventArgs);

protected:

    virtual void OnSelected(RoutedEventArgs* pArgs);
    virtual void OnUnselected(RoutedEventArgs* pArgs);
    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseRightButtonDown(MouseButtonEventArgs* pArgs);

    Selector* get_ParentSelector();
private:
    ListBox* get_ParentListBox();
    void HandleMouseButtonDown(MouseButton::Enum mouseButton);

    void HandleIsSelectedChanged(bool newValue, RoutedEventArgs* pArgs);
    static void OnIsSelectedChanged(Object* pSender, const DependencyPropertyChangedEventArgs&);
};


}; // namespace AVUI
