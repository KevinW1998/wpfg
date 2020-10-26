#include <AVUICommon.h>
#include <AVUIUIElement.h>
#include <AVUIVisualTreeHelper.h>
#include <AVUIUIElementHelper.h>

namespace AVUI {

UIElement* UIElementHelper::GetContainingUIElement2D(DependencyObject* pReference)
{
    UIElement* pElement = NULL;
    while (pReference != NULL)
    {
        pElement = object_cast<UIElement>(pReference);
        if (pElement != NULL)
        {
            return pElement;
        }
        pReference = VisualTreeHelper::GetParent(pReference);
    }
    return pElement;
}

DependencyObject* UIElementHelper::GetUIParent(DependencyObject* pChild, bool continuePastVisualTree)
{
    DependencyObject* pContainingUIElement = NULL;
    DependencyObject* pDO = NULL;
    if (is_type<Visual>(pChild))
    {
        pDO = ((Visual*)pChild)->get_VisualParent();
    }
    /*
    else
    {
        o = ((Visual3D) child).InternalVisualParent;
    }
    containingUIElement = InputElement.GetContainingUIElement(o);
    */
    pContainingUIElement = object_cast<UIElement>(pDO);
    
    if (pContainingUIElement == NULL && continuePastVisualTree)
    {
        UIElement* pElement = object_cast<UIElement>(pChild);
        if (pElement != NULL)
        {
            //return (InputElement.GetContainingInputElement(element.GetUIParentCore()) as DependencyObject);
            return pElement->get_Parent();
        }
        /*
        UIElement3D elementd = child as UIElement3D;
        if (elementd != null)
        {
            containingUIElement = InputElement.GetContainingInputElement(elementd.GetUIParentCore()) as DependencyObject;
        }
        */
    }
    return pContainingUIElement;
}

bool UIElementHelper::IsHitTestVisible(DependencyObject* pDO)
{
    UIElement* pElement = object_cast<UIElement>(pDO);
    if (pElement != NULL)
    {
        return pElement->get_IsHitTestVisible();
    }

    return false;

    //return ((UIElement3D) o).IsHitTestVisible;
}
bool UIElementHelper::IsUIElementOrUIElement3D(DependencyObject* pDO)
{
    return is_type<UIElement>(pDO); // || is_type<UIElement3D>(pDO);
}

bool UIElementHelper::IsVisible(DependencyObject* pDO)
{
    UIElement* pElement = object_cast<UIElement>(pDO);
    if (pElement != NULL)
    {
        return pElement->get_IsVisible();
    }

    return false;
    //return ((UIElement3D) o).IsVisible;
}
DependencyObject* UIElementHelper::PredictFocus(DependencyObject* pDO, FocusNavigationDirection::Enum direction)
{
    UIElement* pElement = object_cast<UIElement>(pDO);
    if (pElement != NULL)
    {
        //return pElement->PredictFocus(direction);
    }
    return NULL;
    //return ((UIElement3D) o).PredictFocus(direction);
}


}; // namespace AVUI
 
