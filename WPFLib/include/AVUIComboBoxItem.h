#pragma once

#include <AVUIListBoxItem.h>

namespace AVUI {

class ComboBox;

class ComboBoxItem : public ListBoxItem
{
    friend class ComboBox;
public:
    ComboBoxItem() { };

    DECLARE_ELEMENT(ComboBoxItem, ListBoxItem);

    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsHighlighted, bool, false, PropertyFlag::None);

protected:
    virtual void OnGotFocus(RoutedEventArgs* pArgs);
    virtual void OnMouseEnter(MouseEventArgs* pArgs);
    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs);


private:
    void SetIsHighlighted(bool isHighlighted);
    ComboBox* get_ParentComboBox();

};

 


}; // namespace AVUI


