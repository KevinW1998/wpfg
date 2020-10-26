#include <AVUICommon.h>
#include <AVUIObjectRef.h>
#include <AVUIUIElement.h>
#include <AVUIPanel.h>
#include <AVUIHelper.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIRelativeSource.h>
#include <AVUIContentPresenter.h>
#include <AVUIItemsControl.h>
#include <AVUIBindingExpression.h>

REGISTER_ELEMENT_BEGIN(AVUI::ObjectRef)

REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::ElementObjectRef)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::RelativeObjectRef)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::ExplicitObjectRef)

REGISTER_ELEMENT_END()


namespace AVUI {

class HeaderedItemsControl;

ElementObjectRef::ElementObjectRef(String* pName)
{
    m_pName = pName;
}

TRefCountedPtr<Object> ElementObjectRef::GetDataObjectImpl(DependencyObject* pDO, bool resolveNamesInTemplate)
{
    if (pDO == NULL)
    {
        LibraryCriticalError();
    }

    Object* pObj2 = NULL;
    if (resolveNamesInTemplate)
    {
        UIElement* pElement = object_cast<UIElement>(pDO);
        if (pElement != NULL && (pElement->get_TemplateInternal() != NULL))
        {
            pObj2 = Helper::FindNameInTemplate(m_pName, pDO);
        }
    }
    DependencyObject* pObj3 = pDO;
    while(pObj2 == NULL && pObj3 != NULL)
    {
        UIElement* pUIElement = object_cast<UIElement>(pObj3);
        DependencyObject* pObj4 = NULL;

        if(pUIElement == NULL)
        {
            pObj3 = object_cast<DependencyObject>(pObj3->get_InheritanceContext());
            continue;
        }

        if(pUIElement != NULL)
        {
            pObj2 = pUIElement->FindName(*m_pName, &pObj4);
        }                                   

        /*
        if (((d == obj4) && (d is IComponentConnector)) && (d.ReadLocalValue(NavigationService.NavigationServiceProperty) == DependencyProperty.UnsetValue))
        {
            DependencyObject parent = LogicalTreeHelper.GetParent(d);
            if (parent == null)
            {
                parent = Helper.FindMentor(d.InheritanceContext);
            }
            if (parent != null)
            {
                obj2 = null;
                obj3.Reset(parent);
                continue;
            }
        }
        */

        if (pObj2 == NULL)
        {
            UIElement* pObj6 = object_cast<UIElement>(pObj4);

            if(pObj6 == NULL)
            {
                break;
            }

            DependencyObject* pTemplatedParent = pObj6->get_TemplatedParent();
            if (pTemplatedParent == NULL)
            {
                Panel* pPanel = object_cast<Panel>(pUIElement->get_Parent());
                if (pPanel != NULL && pPanel->get_IsItemsHost())
                {
                    pTemplatedParent = pPanel;
                }
            }
            pObj3 = pTemplatedParent;
        }
    }
    if (pObj2 == NULL)
    {
        return DependencyProperty::get_UnsetValue();
    }
    return pObj2;
}

ExplicitObjectRef::ExplicitObjectRef(Object* pObject)
{
    if (pObject != NULL && pObject->IsType(GET_TYPE(DependencyObject)))
    {
        m_pElement = pObject;
    }
    else
    {
        m_pObject = pObject;
    }
}

TRefCountedPtr<Object> ExplicitObjectRef::GetObject(DependencyObject* pDO)
{
    if (m_pElement == NULL)
    {
        return m_pObject;
    }
    return (Object*) m_pElement;
}

RelativeObjectRef::RelativeObjectRef(RelativeSource* pRelativeSource)
{
    if (pRelativeSource == NULL)
    {
        LibraryCriticalError();
    }
    m_pRelativeSource = pRelativeSource;
}

TRefCountedPtr<Object> RelativeObjectRef::GetDataObject(DependencyObject* pDO, bool resolveNamesInTemplate)
{
    TRefCountedPtr<Object> pDataObjectImpl = GetDataObjectImpl(pDO);
    DependencyObject* pObj3 = object_cast<DependencyObject>(pDataObjectImpl);
    if (pObj3 != NULL && get_ReturnsDataContext())
    {
        pDataObjectImpl = pObj3->GetValue(ItemContainerGenerator::get_ItemForItemContainerProperty());
        if (pDataObjectImpl == NULL)
        {
            pDataObjectImpl = pObj3->GetValue(UIElement::get_DataContextProperty());
        }
    }
    return pDataObjectImpl;
}

TRefCountedPtr<Object> RelativeObjectRef::GetObject(DependencyObject* pDO)
{
    return GetDataObjectImpl(pDO);
}

bool RelativeObjectRef::get_ReturnsDataContext()
{
    return m_pRelativeSource->get_Mode() == RelativeSourceMode::PreviousData;
}

bool RelativeObjectRef::ProtectedTreeContextIsRequired(DependencyObject* pTarget)
{
    if (m_pRelativeSource->get_Mode() != RelativeSourceMode::FindAncestor)
    {
        return (m_pRelativeSource->get_Mode() == RelativeSourceMode::PreviousData);
    }
    return true;
}

bool RelativeObjectRef::get_ProtectedUsesMentor()
{
    switch (m_pRelativeSource->get_Mode())
    {
    case RelativeSourceMode::PreviousData:
    case RelativeSourceMode::TemplatedParent:
        return true;
    }
    return false;
}

TRefCountedPtr<DependencyObject> RelativeObjectRef::FindAncestorOfType(const Type* pType, int level, DependencyObject* pDO)
{
    if (pType == NULL)
    {
        LibraryCriticalError();
        return NULL;
    }
    if (level < 1)
    {
        LibraryCriticalError();
        return NULL;
    }

    while (pDO != NULL)
    {
        if (pDO->IsType(*pType) && (--level <= 0))
        {
            break;
        }

        if(pDO->IsType(GET_TYPE(Visual)))
        {
            pDO = ((Visual*)pDO)->get_VisualParent();
        }
        else
        {
            pDO = object_cast<DependencyObject>(pDO->get_InheritanceContext());
        }
    }
    return pDO;
}

TRefCountedPtr<Object> RelativeObjectRef::GetDataObjectImpl(DependencyObject* pDO)
{
    if (pDO != NULL)
    {
        switch (m_pRelativeSource->get_Mode())
        {
        case RelativeSourceMode::PreviousData:
            return GetPreviousData(pDO);

        case RelativeSourceMode::TemplatedParent:
            pDO = Helper::GetTemplatedParent(pDO);
            return pDO;

        case RelativeSourceMode::Self:
            return pDO;

        case RelativeSourceMode::FindAncestor:
            pDO = FindAncestorOfType(m_pRelativeSource->get_AncestorType(), m_pRelativeSource->get_AncestorLevel(), pDO);
            if (pDO != NULL)
            {
                return pDO;
            }
            return DependencyProperty::get_UnsetValue();
        }
    }
    return NULL;
}

TRefCountedPtr<Object> RelativeObjectRef::GetPreviousData(DependencyObject* pDO)
{
   while (pDO != NULL)
    {
        if (BindingExpression::HasLocalDataContext(pDO))
        {
            UIElement* pElement;
            ContentPresenter* pPresenter = object_cast<ContentPresenter>(pDO);

            // TODO HeaderedItemsControl

            if (pPresenter == NULL || ((pElement = object_cast<UIElement>(pPresenter->get_TemplatedParent())) == NULL) || 
                (!(pElement->IsType(GET_TYPE(ContentControl)))/* && !(pElement->IsType(GET_TYPE(HeaderedItemsControl))) */) || 
                !Object::Equals(pPresenter->get_DataContext(), pElement->get_DataContext()))
            {
                break;
            }
            pDO = pElement;
            if (BindingExpression::HasLocalDataContext(pElement))
            {
                break;
            }
        }

        pDO = UIElement::GetFrameworkParent(pDO);
    }
    if (pDO == NULL)
    {
        return DependencyProperty::get_UnsetValue();
    }
    Visual* pReference = object_cast<Visual>(pDO);
    DependencyObject* pObj2 = (pReference != NULL) ? pReference->get_VisualParent() : NULL;
    if (ItemsControl::GetItemsOwner(pObj2) == NULL)
    {
        return NULL;
    }
    Visual* pParent = object_cast<Visual>(pObj2);
    int num = (pParent != NULL) ? pParent->get_VisualChildCount() : 0;
    int num2 = -1;
    Visual* pPrevChild = NULL;
    if (num != 0)
    {
        num2 = IndexOf(pParent, pReference, &pPrevChild);
    }
    if (num2 > 0)
    {
        pDO = pPrevChild;

        return pDO;
    }
    pDO = NULL;

    return pDO;
}


int RelativeObjectRef::IndexOf(Visual* pParent, Visual* pChild, Visual** ppPrevChild)
{
    bool flag = false;
    *ppPrevChild = NULL;
    int internalVisualChildrenCount = pParent->get_VisualChildCount();
    int index = 0;
    while (index < internalVisualChildrenCount)
    {
        Visual* pVisualChild = pParent->get_VisualChild(index);
        if (pChild == pVisualChild)
        {
            flag = true;
            break;
        }
        *ppPrevChild = pVisualChild;
        index++;
    }
    if (flag)
    {
        return index;
    }
    return -1;
}

};
