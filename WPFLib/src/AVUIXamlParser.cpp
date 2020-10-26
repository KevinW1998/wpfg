#include <AVUICommon.h>                                                                   
#include <AVUIXamlParser.h>
#include <AVUITypeManager.h>
#include <AVUIDependencyObject.h>
#include <AVUIObject.h>
#include <AVUIPropertyName.h>
#include <AVUIObjectCollection.h>
#include <AVUIResourceDictionary.h>
#include <memory.h>
#include <AVUIStyle.h>
#include <AVUITemplate.h>
#include <AVUITrigger.h>
#include <string.h>
#include <AVUIUIElement.h>
#include <AVUIControl.h>
#include <AVUITemplateBinding.h>
#include <AVUINameScope.h>
#include <AVUIRelativeSource.h>
#include <AVUIIAddChild.h>
#include <AVUIBinding.h>
#include <AVUIBindingOperations.h>

namespace AVUI {


XamlParser::XamlParser() : m_pLastUIElement(NULL), m_currentLine(-1)
{
    m_parserContextStack.AddEmptyItems(c_parserInitialSize);
}

bool XamlParser::StartDocument(void)
{
    m_cCurrentStackDepth = 0;
    m_pTemplateCurrent = NULL;
    m_pTemplatedParent = NULL;
    m_pStyleCurrent = NULL;
    m_pTemplateInstantiating = NULL;
    m_isTemplateStarted = false;
    return true;
}

bool XamlParser::IsQualifiedProperty(const WCHAR* pwchData, int cchData)
{
    for(int i = 0; i < cchData; i++)
    {
        if(pwchData[i] == L'.')
        {
            return true;
        }
    }
    return false;
}

void XamlParser::AddChild(Object* pObjectParent, Object* pObjectChild)
{
    if(pObjectParent->IsType(GET_TYPE(ResourceDictionary)))
    {
        SetPropertyValue(pObjectParent, NULL, pObjectChild);
    }
    else
    {
        const Property* pProperty = pObjectParent->get_Type().get_ContentProperty();

        if(pProperty == NULL)
        {
            XamlParseError("Could not find content property for %s.", pObjectParent->get_Type().ToString());
        }

        SetPropertyValue(pObjectParent, pProperty, pObjectChild);
    }
}

void XamlParser::SetPropertyValue(Object* pObjectParent, 
                                  const Property* pProperty,
                                  Object* pObjectValue)
{
    if(pObjectParent->IsType(GET_TYPE(ResourceDictionary)) && pProperty == NULL)
    {
        ResourceDictionary* pResourceDictionary = (ResourceDictionary*) pObjectParent;

        pResourceDictionary->Add(m_keyStack[m_keyStack.get_Count() - 1], pObjectValue);
        m_keyStack.RemoveAt(m_keyStack.get_Count() - 1);
    }
    else if(pObjectValue != NULL && 
            pObjectValue->IsType(GET_TYPE(BindingBase)) && 
            pProperty->IsType(GET_TYPE(DependencyProperty)) && 
            pObjectValue->get_Type() != pProperty->get_PropertyType())
    {
        BindingOperations::SetBinding((DependencyObject*)pObjectParent, (const DependencyProperty*)pProperty, (BindingBase*)pObjectValue);
    }
    else if(pObjectValue == NULL || pObjectValue->IsType(pProperty->get_PropertyType()) || pObjectValue->IsType(GET_TYPE(TemplateBinding)) || pObjectValue->IsType(GET_TYPE(Binding)))
    {
        // Direct add, don't try to add as collection child
        if(pProperty->IsType(GET_TYPE(DependencyProperty)))
        {
            DependencyProperty* pDP = (DependencyProperty*) pProperty;
            DependencyObject* pDO = object_cast<DependencyObject>(pObjectParent);

            pDO->SetValue(*pDP, pObjectValue);
        }
        else
        {
            SimpleProperty* pSP = (SimpleProperty*) pProperty;
            pSP->SetValue(pObjectParent, pObjectValue);
        }
    }
    else if(pProperty->get_PropertyType().HasInterface(GET_TYPE(ICollection)))
    {
        // See if current value is a collection. If so, just add.
        if(pProperty->IsType(GET_TYPE(DependencyProperty)))
        {
            DependencyProperty* pDP = (DependencyProperty*) pProperty;
            DependencyObject* pDO = object_cast<DependencyObject>(pObjectParent);

            ICollection* pCollection = interface_cast<ICollection>(pDO->GetValue(*pDP));
            if(pCollection == NULL)
            {
                if(pDP->get_Flags() & PropertyFlag::AutoCreate)
                {
                    pDO->SetValue(*pDP, pDP->get_PropertyType().CreateInstance());
                    pCollection = interface_cast<ICollection>(pDO->GetValue(*pDP));
                }
            }
            pCollection->Add(pObjectValue);
        }
        else
        {
            SimpleProperty* pSP = (SimpleProperty*) pProperty;

            ICollection* pCollection = interface_cast<ICollection>(pSP->GetValue(pObjectParent));
            if(pCollection == NULL)
            {
                XamlParseError("Could not add collection element, collection itself was NULL.");
            }
            pCollection->Add(pObjectValue);
        }
    }
    else if(pProperty->get_PropertyType().IsTypeOrBase(GET_TYPE(ResourceDictionary)))
    {
        ResourceDictionary* pResourceDictionary = (ResourceDictionary*) (Object*) pProperty->GetValue(pObjectParent);

        if(pResourceDictionary == NULL && pProperty->IsType(GET_TYPE(DependencyProperty)))
        {
            DependencyProperty* pDP = (DependencyProperty*) pProperty;
            DependencyObject* pDO = object_cast<DependencyObject>(pObjectParent);

            pResourceDictionary = object_cast<ResourceDictionary>(pDO->GetValue(*pDP));

            if(pResourceDictionary == NULL)
            {
                if(pDP->get_Flags() & PropertyFlag::AutoCreate)
                {
                    pDO->SetValue(*pDP, pDP->get_PropertyType().CreateInstance());
                    pResourceDictionary = object_cast<ResourceDictionary>(pDO->GetValue(*pDP));
                }
            }
        }

        pResourceDictionary->Add(m_keyStack[m_keyStack.get_Count() - 1], pObjectValue);
        m_keyStack.RemoveAt(m_keyStack.get_Count() - 1);
    }
    else
    {
        IAddChild* pAddChild = interface_cast<IAddChild>(pObjectParent);

        if(pAddChild != NULL && (pObjectValue == NULL || !pObjectValue->IsType(GET_TYPE(int))))
        {
            pAddChild->AddChild(pObjectValue);
        }
        else
        {
            // This is case of enum / int values. TODO - Validate this!
            if(pProperty->IsType(GET_TYPE(DependencyProperty)))
            {
                DependencyProperty* pDP = (DependencyProperty*) pProperty;
                DependencyObject* pDO = object_cast<DependencyObject>(pObjectParent);

                pDO->SetValue(*pDP, pObjectValue);
            }
            else
            {
                SimpleProperty* pSP = (SimpleProperty*) pProperty;
                pSP->SetValue(pObjectParent, pObjectValue);
            }
        }
    }
}



Object* XamlParser::get_PreviousObject()
{
    if(m_cCurrentStackDepth == 0)
    {
        return NULL;
    }

    if(m_parserContextStack[m_cCurrentStackDepth - 1].parserContext != ParserContext::Object)
    {
        return NULL;
    }

    return m_parserContextStack[m_cCurrentStackDepth - 1].pObject;
}

const Property* XamlParser::GetPropertyForName(const WCHAR* pwchData, int cchData, const Type* pType)
{
    String propertyQualifier;
    String propertyName(pwchData, cchData);

    if(IsQualifiedProperty(pwchData, cchData))
    {
        int index = propertyName.IndexOf(L'.');

        propertyQualifier = String(propertyName.get_Ptr(), index);
        propertyName = String(propertyName.get_Ptr() + index + 1, (propertyName.get_Cch() - index - 1));
    }

    if(propertyQualifier.get_Cch() > 0)
    {
        const Type* pAttachedType = TypeManager::FindType(propertyQualifier);
        if(pAttachedType == NULL)
        {
            XamlParseError("Unable to find qualified type %ws for property %ws.", propertyQualifier.get_Ptr(), propertyName.get_Ptr());
        }

        return pAttachedType->get_Property(propertyName);
    }
    else
    {
        return pType->get_Property(propertyName);
    }
}

const RoutedEvent* XamlParser::GetRoutedEventForName(const WCHAR* pwchData, int cchData, const Type* pType)
{
    int index = IndexOf(pwchData, cchData, L':');

    if(index != -1)
    {
        pwchData += index + 1;
        cchData -= index + 1;
    }


    String eventQualifier;
    String eventName(pwchData, cchData);

    if(IsQualifiedProperty(pwchData, cchData))
    {
        int index = eventName.IndexOf(L'.');

        eventQualifier = String(eventName.get_Ptr(), index);
        eventName = String(eventName.get_Ptr() + index + 1, (eventName.get_Cch() - index - 1));
    }

    if(eventQualifier.get_Cch() > 0)
    {
        const Type* pAttachedType = TypeManager::FindType(eventQualifier);
        if(pAttachedType == NULL)
        {
            XamlParseError("Unable to find qualified type %ws for event %ws.", eventQualifier.get_Ptr(), eventName.get_Ptr());
        }

        return pAttachedType->get_RoutedEvent(eventName);
    }
    else
    {
        return pType->get_RoutedEvent(eventName);
    }

}

const RoutedCommand* XamlParser::GetRoutedCommandForName(const WCHAR* pwchData, int cchData, const Type* pType)
{
    String commandQualifier;
    String commandName(pwchData, cchData);

    if(IsQualifiedProperty(pwchData, cchData))
    {
        int index = commandName.IndexOf(L'.');

        commandQualifier = String(commandName.get_Ptr(), index);
        commandName = String(commandName.get_Ptr() + index + 1, (commandName.get_Cch() - index - 1));
    }

    if(commandQualifier.get_Cch() > 0)
    {
        const Type* pAttachedType = TypeManager::FindType(commandQualifier);
        if(pAttachedType == NULL)
        {
            XamlParseError("Unable to find qualified type %ws for command %ws.", commandQualifier.get_Ptr(), commandName.get_Ptr());
        }

        return pAttachedType->get_Command(commandName);
    }
    else
    {
        return pType->get_Command(commandName);
    }

}


bool XamlParser::IsMarkupExtension(const String& str)
{
    if(str.BeginsWith(L"{", 1) && str.EndsWith(L"}", 1))
    {
        return true;
    }

    return false;
}

bool XamlParser::BeginAttributes() 
{
    if(m_pTemplateCurrent != NULL && m_isTemplateStarted)
    {
        m_pTemplateCurrent->RecordBeginAttributes();
    }
    return true;
}

bool XamlParser::EndAttributes() 
{ 
    if(m_pTemplateCurrent != NULL && m_isTemplateStarted)
    {
        m_pTemplateCurrent->RecordEndAttributes();
    }
    else
    {
        HandleDeferredAttributes();
    }
    return true;
};

bool XamlParser::AddAttribute(const WCHAR* pwchName, int cchName, const WCHAR* pwchValue, int cchValue)
{
    String localName(pwchName, cchName);
    String localValue(pwchValue, cchValue);

    localName.Trim();
    localValue.Trim();

    if(m_pTemplateCurrent != NULL && m_isTemplateStarted)
    {
        m_pTemplateCurrent->RecordAttribute(pwchName, cchName, pwchValue, cchValue);

        if(localName.Equals(L"x:Name"))
        {
            m_pTemplateCurrent->RecordTemplateChild(localValue, m_pTemplateCurType); 
        }

        return true;
    }

    if(localName.BeginsWith(L"xmlns:", 6) || localName.Equals(L"xmlns"))
    {
        return true;
    }

    bool isStyleHandled = false;


    if(localName.Equals(L"x:Key"))
    {
        if(IsMarkupExtension(localValue))
        {
            m_keyStack.AddItem(ResolvePropertyAttribute(NULL, localValue));
        }
        else
        {
            m_keyStack.AddItem(ObjectPtr(new String(localValue), false));
        }
    }
    else
    {
        if(m_parserContextStack[m_cCurrentStackDepth - 1].parserContext != ParserContext::Object)
        {
            XamlParseError("Parse Error -- Property set on non-element.");
        }

        Object* pObject = m_parserContextStack[m_cCurrentStackDepth - 1].pObject;

        if(localName.Equals("x:Name"))
        {
            if(m_pTemplateInstantiating != NULL)
            {
                m_pTemplateInstantiating->RecordTemplateChildInstance(m_pTemplatedParent, (DependencyObject*) pObject);
            }
            else
            {
                DependencyObject* pDORoot = object_cast<DependencyObject>(m_pRootElement);

                if(pDORoot != NULL)
                {
                    NameScope* pNameScope = NameScope::GetNameScope(pDORoot);
                    if(pNameScope == NULL)
                    {
                        NameScopePtr pNewNameScope(new NameScope(), false);
                        NameScope::SetNameScope(pDORoot, pNewNameScope);
                        pNameScope = pNewNameScope;
                    }
                    pNameScope->RegisterName(localValue, pObject);
                }
            }
        }
        else if(!localName.Contains(L':')) // Ignore any additional namespaced tags
        {
            const Property* pProperty = GetPropertyForName(pwchName, cchName, &(pObject->get_Type()));

            if(pProperty == NULL)
            {
                XamlParseError("Unable to find property %ws for type %s", String(pwchName, cchName).get_Ptr(), pObject->get_Type().ToString());
            }

            if(IsDeferredAttribute(pProperty, localValue))
            {
                DeferAttributeHandling(pProperty, localValue);
            }
            else
            {
                SetPropertyValue(pObject, pProperty, ResolvePropertyAttribute(pProperty, localValue));
            }
        }

    }



    return true;
}

bool XamlParser::IsDeferredAttribute(const Property* pProperty, const String& value)
{
    if(pProperty->get_PropertyType().IsTypeOrBase(GET_TYPE(Property)) ||
       pProperty->get_PropertyType().IsTypeOrBase(GET_TYPE(RoutedEvent)) ||
       pProperty->get_PropertyType().IsTypeOrBase(GET_TYPE(RoutedCommand)))
    {
        return true;
    }

    return pProperty->get_PropertyType() == GET_TYPE(Object);
    
}

void XamlParser::DeferAttributeHandling(const Property* pProperty, const String& localValue)
{
    DeferredAttribute attr;

    attr.pProperty = pProperty;
    attr.Value = localValue;

    m_deferredAttributeStack.AddItem(attr);
}


void XamlParser::HandleDeferredAttributes()
{
    if(m_deferredAttributeStack.get_Count() > 0)
    {
        for(int idx = 0; idx < m_deferredAttributeStack.get_Count(); idx++)
        {
            HandleDeferredAttribute(m_deferredAttributeStack[idx].pProperty, m_deferredAttributeStack[idx].Value);
        }
        m_deferredAttributeStack.Clear();
    }
}

void XamlParser::HandleDeferredAttribute(const Property* pProperty, const String& value)
{
    Object* pObject = m_parserContextStack[m_cCurrentStackDepth - 1].pObject;
    const Type* pTypeResolutionContext = NULL;

    if(pProperty->get_PropertyType().IsTypeOrBase(GET_TYPE(Property)))
    {

        if(!IsQualifiedProperty(value.get_Ptr(), value.get_Cch()))
        {
            IXamlResolutionContext* pResolutionContext = get_ResolutionContext();
            if(pResolutionContext == NULL)
            {
                XamlParseError("Xaml resolution context required for property resolution.");
            }

            pTypeResolutionContext = pResolutionContext->GetPropertyResolutionContext(pObject);

            if(pTypeResolutionContext == NULL)
            {
                XamlParseError("Type resolution context required to determine property value.");
            }
        }

        const Property* pValueProperty = GetPropertyForName(value.get_Ptr(), value.get_Cch(), pTypeResolutionContext);

        SetPropertyValue(pObject, pProperty, (Object*)pValueProperty);
    }
    else if(pProperty->get_PropertyType().IsTypeOrBase(GET_TYPE(RoutedEvent)))
    {
        if(!IsQualifiedProperty(value.get_Ptr(), value.get_Cch()))
        {
            IXamlResolutionContext* pResolutionContext = get_ResolutionContext();
            if(pResolutionContext == NULL)
            {
                XamlParseError("Xaml resolution context required for event resolution.");
            }

            pTypeResolutionContext = pResolutionContext->GetPropertyResolutionContext(pObject);

            if(pTypeResolutionContext == NULL)
            {
                XamlParseError("Type resolution context required to determine event value.");
            }
        }

        const RoutedEvent* pRoutedEvent = GetRoutedEventForName(value.get_Ptr(), value.get_Cch(), pTypeResolutionContext);

        SetPropertyValue(pObject, pProperty, (Object*)pRoutedEvent);
    }
    else if(pProperty->get_PropertyType().IsTypeOrBase(GET_TYPE(RoutedCommand)))
    {
        if(!IsQualifiedProperty(value.get_Ptr(), value.get_Cch()))
        {
            IXamlResolutionContext* pResolutionContext = get_ResolutionContext();
            if(pResolutionContext == NULL)
            {
                XamlParseError("Xaml resolution context required for command resolution.");
            }

            pTypeResolutionContext = pResolutionContext->GetPropertyResolutionContext(pObject);

            if(pTypeResolutionContext == NULL)
            {
                XamlParseError("Type resolution context required to determine command value.");
            }
        }

        const RoutedCommand* pRoutedCommand = GetRoutedCommandForName(value.get_Ptr(), value.get_Cch(), pTypeResolutionContext);

        if(pRoutedCommand == NULL)
        {
            XamlParseError("Unable to resolve routed command %ws.", value.get_Ptr());
        }

        SetPropertyValue(pObject, pProperty, (Object*)pRoutedCommand);
    }
    else if(pProperty->get_PropertyType() == GET_TYPE(Object))
    {
        const Property* pPropertyContext = NULL;

        IXamlResolutionContext* pResolutionContext = get_ResolutionContext();
        if(pResolutionContext != NULL)
        {
            pPropertyContext = pResolutionContext->GetValueConverterContext(pObject);
        }

        if(pPropertyContext == NULL)
        {
            pPropertyContext = pProperty;
        }

        SetPropertyValue(pObject, pProperty, ResolvePropertyAttribute(pPropertyContext, value));
    }
}

ObjectPtr XamlParser::ParseMarkupExtension(const String& localValue)
{
    int indexSpace;

    for(indexSpace = 1; indexSpace < localValue.get_Cch(); indexSpace++)
    {
        if(String::IsWhiteSpace(localValue[indexSpace]) || localValue[indexSpace] == L'}')
        {
            break;
        }
    }

    String extensionType(localValue.get_Ptr() + 1, indexSpace - 1);

    const WCHAR* pwchArgument = localValue.get_Ptr() + indexSpace + 1;
    int cchArgument = localValue.get_Cch() - indexSpace - 2;

    if(cchArgument > 0 && pwchArgument[0] != L'{')
    {
        int indexNS = IndexOf(pwchArgument, cchArgument, L':');

        if(indexNS != -1)
        {
            pwchArgument += indexNS + 1;
            cchArgument = cchArgument - indexNS - 1;
        }
    }


    String extensionArgument(pwchArgument, cchArgument);

    extensionType.Trim();
    extensionArgument.Trim();

    if(extensionType.Equals(L"x:Type"))
    {
        return TypeManager::FindType(extensionArgument);
    }
    else if(extensionType.Equals(L"x:Null"))
    {
        return NULL;
    }
    else if(extensionType.Equals(L"StaticResource") || extensionType.Equals(L"DynamicResource"))
    {
        ObjectPtr pExtensionValue = &extensionArgument;

        if(IsMarkupExtension(extensionArgument))
        {
            pExtensionValue = ParseMarkupExtension(extensionArgument);
        }

        for(int i = m_cCurrentStackDepth; i >= 0; i--)
        {
            if(m_parserContextStack[i].parserContext == ParserContext::Object)
            {
                ResourceDictionary* pResourceDictionary = object_cast<ResourceDictionary>(m_parserContextStack[i].pObject);

                if(pResourceDictionary == NULL)
                {
                    FrameworkTemplate* pTemplate = object_cast<FrameworkTemplate>(m_parserContextStack[i].pObject);
                    if(pTemplate != NULL)
                    {
                        pResourceDictionary = pTemplate->get_Resources();
                    }
                }

                if(pResourceDictionary != NULL)
                {
                    Object* pValue = pResourceDictionary->GetValue(pExtensionValue);
                    if(pValue != DependencyProperty::get_UnsetValue())
                    {
                        return pValue;
                    }
                }

            }

        }

        if(m_pTemplateCurrent != NULL)
        {
            Object* pValue = m_pTemplateCurrent->get_Resources()->GetValue(pExtensionValue);
            if(pValue != DependencyProperty::get_UnsetValue())
            {
                return pValue;
            }
        }

        for(int i = m_cCurrentStackDepth; i >= 0; i--)
        {
            if(m_parserContextStack[i].parserContext == ParserContext::Object)
            {
                UIElement* pUIElement = object_cast<UIElement>(m_parserContextStack[i].pObject);

                Object* pValue = pUIElement->FindResource(pExtensionValue);
                if(pValue != DependencyProperty::get_UnsetValue())
                {
                    return pValue;
                }
            }

        }

        if(m_pTemplatedParent != NULL && m_pTemplatedParent->IsType(GET_TYPE(UIElement)))
        {
            Object* pValue = ((UIElement*)m_pTemplatedParent)->FindResource(pExtensionValue);

            if(pValue == DependencyProperty::get_UnsetValue())
            {
                XamlParseError("Unable to find resource %ws.", extensionArgument.get_Ptr());
            }

            return pValue;
        }

        XamlParseError("Unable to find resource %ws.", extensionArgument.get_Ptr());
    }
    else if(extensionType.Equals(L"TemplateBinding"))
    {
        if(!m_pTemplatedParent)
        {
            XamlParseError("Template binding is only valid in a template.");
        }

        const Property* pProperty = GetPropertyForName(extensionArgument.get_Ptr(), extensionArgument.get_Cch(), &(m_pTemplatedParent->get_Type()));
        if(!pProperty || !pProperty->IsType(GET_TYPE(DependencyProperty)))
        {
            XamlParseError("Unable to find bindable dependency property %ws on parent type %s.", extensionArgument.get_Ptr(), m_pTemplatedParent->get_Type().ToString());
        }
        const DependencyProperty* pDP = (const DependencyProperty*) pProperty;

        TemplateBindingPtr pExpression = object_allocate<TemplateBinding>();

        pExpression->set_SourceObject(m_pTemplatedParent);
        pExpression->set_SourceProperty(pDP);

        return (Object*) pExpression;
    }
    else if(extensionType.Equals("RelativeSource"))
    {
        // Do we need to support more complicated relative sources?
        return RelativeSource::get_TemplatedParent();
    }
    else
    {
        Type* pType = TypeManager::FindType(extensionType);
        if(pType == NULL)
        {
            XamlParseError("Markup extension type not valid: %ws.", extensionType.get_Ptr());
        }

        ObjectPtr pObject = pType->CreateInstance();
        if(pObject == NULL)
        {
            XamlParseError("Unable to create instance of type: %s", pType->ToString());
        }

        AddToInitializeList(pObject);

        ParseMarkupExtensionArguments(pObject, extensionArgument);

        return pObject;
    }

    return NULL;
}


ObjectPtr XamlParser::ResolvePropertyAttribute(const Property* pProperty, const String& localValue)
{
    if(!IsMarkupExtension(localValue))
    {
        if(pProperty == NULL)
        {
            XamlParseError("Property value is required for resolution.");
        }

        const TypeConverter* pConverter;
        if(pProperty->get_PropertyType() == GET_TYPE(Object))
        {
            pConverter = GET_TYPE(String).get_TypeConverter();
        }
        else
        {
            pConverter = pProperty->get_PropertyConverter();

            if(pConverter == NULL)
            {
                pConverter = pProperty->get_PropertyType().get_TypeConverter();
            }

            if(pConverter == NULL)
            {
                XamlParseError("Unable to locate type converter for %s", pProperty->get_PropertyType().ToString());
            }
        }

        ObjectPtr converterResult = pConverter->ConvertFrom(&localValue);

        AddToInitializeList(converterResult);

        return converterResult;
    }
    else
    {
        return ParseMarkupExtension(localValue);
    }
}

void XamlParser::ParseMarkupExtensionArguments(Object* pObject, const String& extensionArgument)
{
    int curIdx = 0;


    int idxPropertyStart = curIdx;
    int idxPropertyEnd = -1;
    int idxArgumentStart = -1;
    int idxArgumentEnd = -1;
    int braceCount = 0;

    while(curIdx != extensionArgument.get_Cch())
    {

        WCHAR wchCur = extensionArgument[curIdx];

        if(wchCur == L'{')
        {
            braceCount++;
        }
        else if(wchCur == L'}')
        {
            braceCount--;
        }
        else if(wchCur == L',')
        {
            if(braceCount == 0)
            {
                idxArgumentEnd = curIdx - 1;

                AddMarkupExtension(pObject, extensionArgument, idxPropertyStart, idxPropertyEnd, idxArgumentStart, idxArgumentEnd);

                idxPropertyStart = curIdx + 1;
                idxPropertyEnd = idxArgumentStart = idxArgumentEnd = -1;
            }
        }
        else if(wchCur == L'=')
        {
            if(braceCount == 0)
            {
                idxPropertyEnd = curIdx - 1;
                idxArgumentStart = curIdx + 1;
            }
        }

        curIdx += 1;

        if(curIdx == extensionArgument.get_Cch())
        {
            if(braceCount != 0)
            {
                XamlParseError("Unmatched bracket in markup extension.");
            }

            idxArgumentEnd = curIdx - 1;

            AddMarkupExtension(pObject, extensionArgument, idxPropertyStart, idxPropertyEnd, idxArgumentStart, idxArgumentEnd);

            idxPropertyStart = curIdx + 1;
            idxPropertyEnd = idxArgumentStart = idxArgumentEnd = -1;
        }
    }
}

void XamlParser::AddMarkupExtension(Object* pObject, const String& extensionArgument, int idxPropertyStart, int idxPropertyEnd, int idxArgumentStart, int idxArgumentEnd)
{
    if(idxPropertyStart == -1 || idxPropertyEnd == -1 || idxArgumentStart == -1 || idxArgumentEnd == -1 || idxPropertyEnd <= idxPropertyStart || idxArgumentEnd <= idxArgumentStart)
    {
        return;
    }

    String extensionProperty(extensionArgument.get_Ptr() + idxPropertyStart, idxPropertyEnd - idxPropertyStart + 1);
    String extensionValue(extensionArgument.get_Ptr() + idxArgumentStart, idxArgumentEnd - idxArgumentStart + 1);

    extensionProperty.Trim();
    extensionValue.Trim();

    const Property* pProperty = GetPropertyForName(extensionProperty.get_Ptr(), extensionProperty.get_Cch(), &(pObject->get_Type()));
    if(pProperty == NULL)
    {
        XamlParseError("Unable to find property for markup extension %ws on type %s.", extensionProperty.get_Ptr(), pObject->get_Type().ToString());
    }

    SetPropertyValue(pObject, pProperty, ResolvePropertyAttribute(pProperty, extensionValue));    
}

int XamlParser::IndexOf(const WCHAR* pwch, int cch, WCHAR wchFind)
{
    for(int i = 0; i < cch; i++)
    {
        if(pwch[i] == wchFind)
        {
            return i;
        }
    }

    return -1;
}


bool XamlParser::StartElement(const WCHAR *pwchName, int cchName)
{
    int index = IndexOf(pwchName, cchName, L':');

    if(index != -1)
    {
        pwchName += index + 1;
        cchName -= index + 1;
    }

    if(m_pTemplateCurrent != NULL)
    {
        if(!m_isTemplateStarted)
        {
            if(!IsQualifiedProperty(pwchName, cchName) && get_PreviousObject() == m_pTemplateCurrent)
            {
                m_isTemplateStarted = true;
            }
        }

        if(m_isTemplateStarted)
        {
            m_pTemplateCurrent->RecordStartElement(pwchName, cchName);

            if(!IsQualifiedProperty(pwchName, cchName))
            {
                m_pTemplateCurType = TypeManager::FindType(String(pwchName, cchName));
                if(m_pTemplateCurType->IsTypeOrBase(GET_TYPE(FrameworkTemplate)))
                {
                    XamlParseError("Nested template definition not allowed.");
                }
            }
            else
            {
                m_pTemplateCurType = NULL;
            }
            return true;
        }

    }

    if(IsQualifiedProperty(pwchName, cchName))
    {
        const Property* pProperty = GetPropertyForName(pwchName, cchName, &(get_PreviousObject()->get_Type()));

        if(pProperty == NULL)
        {
            XamlParseError("Unable to locate qualified property %ws for type %s.", String(pwchName, cchName).get_Ptr(), get_PreviousObject()->get_Type().ToString());
        }

        m_parserContextStack[m_cCurrentStackDepth].pProperty = pProperty;
        m_parserContextStack[m_cCurrentStackDepth].parserContext = ParserContext::Property;
    }
    else
    {
        const Type* pType = TypeManager::FindType(String(pwchName, cchName));
        if(pType == NULL)
        {
            XamlParseError("Unable to find type %ws.", String(pwchName, cchName).get_Ptr());
        }

        if(pType->get_HasConstructor())
        {
            ObjectPtr pObject = pType->CreateInstance();
            if(pObject == NULL)
            {
                XamlParseError("Unable to create instance of type %s (Missing factory?)", pType->ToString());
            }

            if(m_pRootElement == NULL)
            {
                m_pRootElement = pObject;
            }

            m_parserContextStack[m_cCurrentStackDepth].pObject = pObject;
            m_parserContextStack[m_cCurrentStackDepth].parserContext = ParserContext::Object;

            AddToInitializeList(pObject);

            if(pObject->IsType(GET_TYPE(UIElement)))
            {
                m_pLastUIElement = (UIElement*) (Object*)pObject;
            }
            else if(pObject->IsType(GET_TYPE(Style)))
            {
                m_pStyleCurrent = object_cast<Style>(pObject);
            }

            if(pObject->IsType(GET_TYPE(FrameworkTemplate)))
            {
                m_pTemplateCurrent = object_cast<FrameworkTemplate>(pObject);
                m_isTemplateStarted = false;
            }

            if(m_pTemplatedParent != NULL && pObject->IsType(GET_TYPE(UIElement)))
            {
                ((UIElement*)(Object*)pObject)->set_TemplatedParent(m_pTemplatedParent);
            }
        }
        else
        {
            m_parserContextStack[m_cCurrentStackDepth].pType = pType;
            m_parserContextStack[m_cCurrentStackDepth].parserContext = ParserContext::RequiresTypeConverter;

            if(pType->get_TypeConverter() == NULL)
            {
                XamlParseError("Factory or type converter required for type %s.", pType->ToString());
            }
        }
    }

    m_cCurrentStackDepth++;
    if(m_parserContextStack.get_Count() == m_cCurrentStackDepth)
    {
        m_parserContextStack.AddEmptyItems(c_parserInitialSize);
    }

    return true;
}

bool XamlParser::EndElement()
{
    if(m_pTemplateCurrent != NULL && m_isTemplateStarted)
    {
        m_pTemplateCurType = NULL;

        if(!m_pTemplateCurrent->RecordEndElement())
        {
            m_pTemplateCurrent = NULL;
        }

        return true;
    }

    m_cCurrentStackDepth--;


    if(m_cCurrentStackDepth == 0)
    {        
        Visual* pRootVisual = object_cast<Visual>(m_pRootElement);
        if(m_pTemplatedParent != NULL && pRootVisual != NULL)
        {
            object_cast<Visual>(m_pTemplatedParent)->AddVisualChild(pRootVisual);
        }
        InitializeTree();
    } 
    else if(m_parserContextStack[m_cCurrentStackDepth].parserContext == ParserContext::Property)
    {
        // No work - deferred
    }
    else if(m_parserContextStack[m_cCurrentStackDepth - 1].parserContext == ParserContext::Object)
    {
        AddChild(m_parserContextStack[m_cCurrentStackDepth - 1].pObject, m_parserContextStack[m_cCurrentStackDepth].pObject);
    }
    else
    {
        SetPropertyValue(m_parserContextStack[m_cCurrentStackDepth - 2].pObject, 
                         m_parserContextStack[m_cCurrentStackDepth - 1].pProperty,
                         m_parserContextStack[m_cCurrentStackDepth].pObject
                         );
    }


    if(m_parserContextStack[m_cCurrentStackDepth].parserContext == ParserContext::Object)
    {
        Object* pObject = m_parserContextStack[m_cCurrentStackDepth].pObject;

        if(m_pStyleCurrent != NULL && pObject == m_pStyleCurrent)
        {
            m_pStyleCurrent->Seal();
            m_pStyleCurrent = NULL;
        }

        if(pObject->IsType(GET_TYPE(UIElement)))
        {
            m_pLastUIElement = object_cast<UIElement>(((UIElement*)pObject)->get_Parent());
        }
    }

    m_parserContextStack[m_cCurrentStackDepth].pObject = NULL;

    return true;
}

void XamlParser::InitializeTree()
{
    for(int idx = 0; idx < m_initializeList.get_Count(); idx++)
    {
        if(m_initializeList[idx].pObject != NULL)
        {
            m_initializeList[idx].pInitialize->EndInit();
        }
    }

    m_initializeList.Clear();
}


bool XamlParser::EndDocument(void)
{


    return true;
}

bool XamlParser::Characters(const WCHAR *pwchChars, int cchChars)
{
    bool isWhiteSpace = true;
    for(int i = 0; i < cchChars; i++)
    {
        if(!String::IsWhiteSpace(pwchChars[i]))
        {
            isWhiteSpace = false;
            break;
        }
    }

    if(isWhiteSpace)
    {
        return true;
    }

    if(m_pTemplateCurrent != NULL)
    {
        m_pTemplateCurrent->RecordCharacters(pwchChars, cchChars);
        return true;
    }

    StringPtr pString(new String(pwchChars, cchChars), false);

    pString->Trim();

    if(m_parserContextStack[m_cCurrentStackDepth - 1].parserContext == ParserContext::RequiresTypeConverter)
    {
        const TypeConverter* pTypeConverter = m_parserContextStack[m_cCurrentStackDepth - 1].pType->get_TypeConverter();

        m_parserContextStack[m_cCurrentStackDepth - 1].pObject = pTypeConverter->ConvertFrom(pString);
    }
    else
    {
        if(m_parserContextStack[m_cCurrentStackDepth - 1].parserContext != ParserContext::Object)
        {
            XamlParseError("Unable to add text content to non-object.");
        }

        if(m_parserContextStack[m_cCurrentStackDepth - 1].pObject == NULL)
        {
            XamlParseError("Previous object is null.");
        }

        AddChild(m_parserContextStack[m_cCurrentStackDepth - 1].pObject, pString);
    }



    return true;
}

IXamlResolutionContext* XamlParser::get_ResolutionContext()
{
    for(int stackIndex = m_cCurrentStackDepth; stackIndex >= 0; stackIndex--)
    {
        if(m_parserContextStack[stackIndex].parserContext != ParserContext::Object)
        {
            continue;
        }

        IXamlResolutionContext* pContext = interface_cast<IXamlResolutionContext>(m_parserContextStack[stackIndex].pObject);
        if(pContext != NULL)
        {
            return pContext;
        }
    }

    return NULL;
}

void XamlParser::AddToInitializeList(Object* pObject)
{
    if(pObject->get_Type().HasInterface(GET_TYPE(ILoadInitialize)))
    {
        ILoadInitialize* pInitialize = interface_cast<ILoadInitialize>(pObject);

        pInitialize->BeginInit();

        InitializeItem item;

        item.pObject = pObject;
        item.pInitialize = pInitialize;

        m_initializeList.AddItem(item);
    }
}

#include <stdio.h>
#include <stdarg.h>

void XamlParser::XamlParseError(const char* szMessage, ...)
{
    char szBuf[240];
    char szFullText[240];
    va_list vl;

    va_start(vl, szMessage);

    vsprintf_s(szBuf, sizeof(szBuf) / sizeof(szBuf)[0], szMessage, vl);

    va_end(vl);

    if(m_currentLine != -1)
    {
        sprintf_s(szFullText, sizeof(szFullText) / sizeof(szFullText[0]), "Xaml parse error at line %d: %s", m_currentLine, szBuf);
    }
    else
    {
        sprintf_s(szFullText, sizeof(szFullText) / sizeof(szFullText[0]), "Xaml parse error: %s", szBuf);
    }
    LibraryCriticalErrorMessage(szFullText);
}



}; // namespace AVUI
