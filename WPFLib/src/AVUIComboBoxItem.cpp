#include <AVUICommon.h>
#include <AVUIComboBoxItem.h>
#include <AVUIComboBox.h>

REGISTER_ELEMENT_BEGIN(AVUI::ComboBoxItem)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {


void ComboBoxItem::SetIsHighlighted(bool isHighlighted)
{
    SetValue(get_IsHighlightedProperty(), BoxValue(isHighlighted));
}

void ComboBoxItem::OnGotFocus(RoutedEventArgs* pArgs)
{
    pArgs->set_IsHandled(true);

    ComboBox* pParentComboBox = get_ParentComboBox();
    if (pParentComboBox != NULL)
    {
        pParentComboBox->NotifyComboBoxItemEnter(this);
    }
    ContentControl::OnGotFocus(pArgs);
}

void ComboBoxItem::OnMouseEnter(MouseEventArgs* pArgs)
{
    pArgs->set_IsHandled(true);

    ComboBox* pParentComboBox = get_ParentComboBox();
    if (pParentComboBox != NULL)
    {
        pParentComboBox->NotifyComboBoxItemEnter(this);
    }
    ContentControl::OnMouseEnter(pArgs);
}

void ComboBoxItem::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    pArgs->set_IsHandled(true);

    ComboBox* pParentComboBox = get_ParentComboBox();
    if (pParentComboBox != NULL)
    {
        pParentComboBox->NotifyComboBoxItemMouseDown(this);
    }
    ContentControl::OnMouseLeftButtonDown(pArgs);
}

void ComboBoxItem::OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs)
{
    pArgs->set_IsHandled(true);

    ComboBox* pParentComboBox = get_ParentComboBox();
    if (pParentComboBox != NULL)
    {
        pParentComboBox->NotifyComboBoxItemMouseUp(this);
    }
    ContentControl::OnMouseLeftButtonDown(pArgs);
}


ComboBox* ComboBoxItem::get_ParentComboBox()
{
    return object_cast<ComboBox>(get_ParentSelector());
}


}; // namespace AVUI

