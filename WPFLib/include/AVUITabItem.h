#pragma once

#include <AVUISelector.h>
#include <AVUIHeaderedContentControl.h>

namespace AVUI {

class TabItem : public HeaderedContentControl
{
public:
    TabItem() { };
    ~TabItem() { };

    DECLARE_ELEMENT(TabItem, HeaderedContentControl);

    static const AVUI::DependencyProperty& get_IsSelectedProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, OnIsSelectedChanged));
        const static AVUI::DependencyProperty& property = Selector::get_IsSelectedProperty().AddOwner(GET_TYPE(_thisType), BoxValue(false), &metadata);

        return property;
    }
    bool get_IsSelected() { return UnboxValue< bool >(GetValue(get_IsSelectedProperty())); }
    void set_IsSelected(const bool& value) { SetValue(get_IsSelectedProperty(), AVUI::BoxValue(value)); }

protected:

    virtual void OnSelected(RoutedEventArgs* pArgs);
    virtual void OnUnselected(RoutedEventArgs* pArgs);

    virtual void OnMouseDown(MouseButtonEventArgs* pArgs);

private:
    void HandleIsSelectedChanged(bool newValue, RoutedEventArgs* pArgs);
    static void OnIsSelectedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    bool SetFocus();



};

}; // namespace AVUI
