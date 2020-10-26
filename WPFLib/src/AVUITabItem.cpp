#include <AVUICommon.h>
#include <AVUITabItem.h>

REGISTER_ELEMENT_BEGIN(AVUI::TabItem)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsSelected)
REGISTER_ELEMENT_END()

namespace AVUI
{


void TabItem::OnIsSelectedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    TabItem* pContainer = (TabItem*) pDO;
    bool isSelected = UnboxValue<bool>(e.pNewValue);

    if (isSelected)
    {
        RoutedEventArgs args(&Selector::get_SelectedEvent(), pContainer);
        pContainer->OnSelected(&args);
    }
    else
    {
        RoutedEventArgs args(&Selector::get_UnselectedEvent(), pContainer);

        pContainer->OnUnselected(&args);
    }
    // TODO Margin navigation issue ?
    /*
    if (isSelected)
    {
        Binding binding = new Binding("Margin");
        binding.Source = container;
        BindingOperations.SetBinding(container, KeyboardNavigation.DirectionalNavigationMarginProperty, binding);
    }
    else
    {
        BindingOperations.ClearBinding(container, KeyboardNavigation.DirectionalNavigationMarginProperty);
    }
    */

    // Originally affectsparentmeasure
    UIElement* pUIElementParent = object_cast<UIElement>(pContainer->get_VisualParent());
    if(pUIElementParent != NULL)
    {
        pUIElementParent->InvalidateMeasure();
    }
}

void TabItem::OnUnselected(RoutedEventArgs* pArgs)
{
    HandleIsSelectedChanged(false, pArgs);
}

void TabItem::OnSelected(RoutedEventArgs* pArgs)
{
    HandleIsSelectedChanged(true, pArgs);
}

 
void TabItem::HandleIsSelectedChanged(bool newValue, RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void TabItem::OnMouseDown(MouseButtonEventArgs* pArgs)
{
    if ((pArgs->get_Source() == this || !get_IsSelected()) && SetFocus())
    {
        set_IsSelected(true);
        pArgs->set_IsHandled(true);
    }
}

bool TabItem::SetFocus()
{
    // TODO -- Use this implementation?
    /*
    bool flag = false;
    if (!this.GetBoolField(BoolField.SettingFocus))
    {
        TabItem item = Keyboard.FocusedElement as TabItem;
        bool flag2 = ((item == this) || (item == null)) || (item.TabControlParent != this.TabControlParent);
        this.SetBoolField(BoolField.SettingFocus, true);
        this.SetBoolField(BoolField.SetFocusOnContent, flag2);
        try
        {
            flag = base.Focus() || flag2;
        }
        finally
        {
            this.SetBoolField(BoolField.SettingFocus, false);
            this.SetBoolField(BoolField.SetFocusOnContent, false);
        }
    }
    return flag;
    */

    Focus();
    return true;
}




};

