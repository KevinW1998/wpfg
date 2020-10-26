#include <AVUICommon.h>
#include <AVUIType.h>
#include <AVUIDependencyProperty.h>
#include <AVUIHash.h>
#include <AVUIRoutedCommand.h>
#include <AVUITypeTypeConverter.h>

#include <string.h>

REGISTER_ELEMENT_BEGIN(AVUI::Type)
    REGISTER_TYPE_CONVERTER(AVUI::TypeTypeConverter)
REGISTER_ELEMENT_END()

namespace AVUI {

Type::Type(const char* szType, bool isValueType, bool isEnumType) : m_pPropertyFirst(NULL), 
                                 m_pBaseType(NULL), m_pInterfaceFirst(NULL), m_pRoutedEventFirst(NULL), m_pCommandFirst(NULL),
                                 m_uiTypeID(get_UniqueTypeID()), m_isEnumType(isEnumType), m_isValueType(isValueType)
{
    m_szType = szType;

    const char* pchEnd = m_szType + strlen(m_szType) - 1;

    while(pchEnd > m_szType)
    {
        if(*pchEnd == ':')
        {
            m_szType = pchEnd + 1;
            break;
        }
        pchEnd -= 1;
    }

    CalculateHashCode();
}

bool Type::operator!=(const Type& rhs) const
{
    return !((*this) == rhs);
}

bool Type::operator==(const Type& rhs) const
{
    if(this == &rhs)
    {
        return true;
    }

    if(m_uiHashCode != rhs.m_uiHashCode)
    {
        return false;
    }

    return !strcmp(m_szType, rhs.m_szType);
}

void Type::CalculateHashCode()
{
    m_uiHashCode = Hash::HashData((const unsigned char*)m_szType, (int) strlen(m_szType), 0);
}

bool Type::IsAssignableFrom(const Type& typeCompare) const
{
    return typeCompare.IsTypeOrBase(*this);
}


bool Type::IsTypeOrBase(const Type& typeCompare) const
{
    const Type* pTypeCur = this;

    while(pTypeCur != NULL)
    {
        if(*pTypeCur == typeCompare)
        {
            return true;
        }

        pTypeCur = pTypeCur->m_pBaseType;
    }

    return false;
}

const void* Type::CastToInterface(const Type& interfaceName, const void* pbBaseData) const
{
    InterfaceInfo* pInterfaceCur = m_pInterfaceFirst;

    while(pInterfaceCur != NULL)
    {
        if(pInterfaceCur->get_InterfaceType() == interfaceName)
        {
            return pInterfaceCur->Cast(pbBaseData);
        }
        pInterfaceCur = pInterfaceCur->m_pInterfaceNext;
    }

    if(m_pBaseType != NULL)
    {
        return m_pBaseType->CastToInterface(interfaceName, pbBaseData);
    }

    return false;
}

void* Type::CastToInterface(const Type& interfaceName, void* pbBaseData) const
{
    InterfaceInfo* pInterfaceCur = m_pInterfaceFirst;

    while(pInterfaceCur != NULL)
    {
        if(pInterfaceCur->get_InterfaceType() == interfaceName)
        {
            return pInterfaceCur->Cast(pbBaseData);
        }
        pInterfaceCur = pInterfaceCur->m_pInterfaceNext;
    }

    if(m_pBaseType != NULL)
    {
        return m_pBaseType->CastToInterface(interfaceName, pbBaseData);
    }

    return false;
}

const Property* Type::get_Property(const String& propertyName) const
{
    const Property* pPropertyRet = get_PropertyLocal(propertyName);

    if(pPropertyRet == NULL)
    {
        if(m_pBaseType != NULL)
        {
            pPropertyRet = m_pBaseType->get_Property(propertyName);
        }
    }

    return pPropertyRet;
}


const RoutedEvent* Type::get_RoutedEvent(const String& eventName) const
{
    const RoutedEvent* pRoutedEvent = get_RoutedEventLocal(eventName);

    if(pRoutedEvent == NULL)
    {
        if(m_pBaseType != NULL)
        {
            pRoutedEvent = m_pBaseType->get_RoutedEvent(eventName);
        }
    }

    return pRoutedEvent;
}

const RoutedCommand* Type::get_Command(const String& commandName) const
{
    const RoutedCommand* pCommand = get_CommandLocal(commandName);

    if(pCommand == NULL)
    {
        if(m_pBaseType != NULL)
        {
            pCommand = m_pBaseType->get_Command(commandName);
        }
    }

    return pCommand;
}


bool Type::HasInterface(const Type& interfaceName) const
{
    InterfaceInfo* pInterfaceCur = m_pInterfaceFirst;

    while(pInterfaceCur != NULL)
    {
        if(pInterfaceCur->get_InterfaceType() == interfaceName)
        {
            return true;
        }
        pInterfaceCur = pInterfaceCur->m_pInterfaceNext;
    }

    if(m_pBaseType != NULL)
    {
        return m_pBaseType->HasInterface(interfaceName);
    }

    return false;
}

const Property* Type::get_ContentProperty() const
{
    const Property* pPropertyRet = get_ContentPropertyLocal();

    if(pPropertyRet == NULL)
    {
        if(m_pBaseType != NULL)
        {
            pPropertyRet = m_pBaseType->get_ContentProperty();
        }
    }

    return pPropertyRet;
}


void Type::RegisterProperty(PropertyInfo* pProperty)
{
    pProperty->m_pPropertyInfoNext = m_pPropertyFirst;
    m_pPropertyFirst = pProperty;
}


const Property* Type::get_PropertyLocal(const String& propertyName) const
{
    PropertyInfo* pPropertyCur = m_pPropertyFirst;

    while(pPropertyCur != NULL)
    {
        if(propertyName.Equals(pPropertyCur->m_property.get_PropertyName().ToString()))
        {
            return &(pPropertyCur->m_property);
        }

        pPropertyCur = pPropertyCur->m_pPropertyInfoNext;
    }

    return NULL;
}

const Property* Type::get_ContentPropertyLocal() const
{
    PropertyInfo* pPropertyCur = m_pPropertyFirst;

    while(pPropertyCur != NULL)
    {
        if(pPropertyCur->m_property.get_IsContentProperty())
        {
            return &(pPropertyCur->m_property);
        }

        pPropertyCur = pPropertyCur->m_pPropertyInfoNext;
    }

    return NULL;
}

const RoutedEvent* Type::get_RoutedEventLocal(const String& eventName) const
{
    RoutedEventInfo* pRoutedEventInfoCur = m_pRoutedEventFirst;

    while(pRoutedEventInfoCur != NULL)
    {
        String str(pRoutedEventInfoCur->m_event.get_Name());
        if(eventName.Equals(&str))
        {
            return &(pRoutedEventInfoCur->m_event);
        }
        pRoutedEventInfoCur = pRoutedEventInfoCur->m_pRoutedEventInfoNext;
    }

    return NULL;
}

const RoutedCommand* Type::get_CommandLocal(const String& commandName) const
{
    RoutedCommandInfo* pCommandInfoCur = m_pCommandFirst;

    while(pCommandInfoCur != NULL)
    {
        String str(pCommandInfoCur->m_command.get_Name());
        if(commandName.Equals(&str))
        {
            return &(pCommandInfoCur->m_command);
        }
        pCommandInfoCur = pCommandInfoCur->m_pCommandInfoNext;
    }

    return NULL;
}



bool Type::IsType(const Type& rhs) const { return get_StaticType().IsTypeOrBase(rhs); } 

void Type::RegisterBase(const Type& baseType)
{
    m_pBaseType = &baseType;
}

ObjectPtr Type::CreateInstance() const
{
    if(m_pfnFactory != NULL)
    {
        return m_pfnFactory(NULL, 0);
    }
    return NULL;
}

void Type::RegisterInterface(InterfaceInfo* pInterface)
{
    pInterface->m_pInterfaceNext = m_pInterfaceFirst;
    m_pInterfaceFirst = pInterface;
}

void Type::RegisterRoutedEvent(RoutedEventInfo* pRoutedEvent)
{
    pRoutedEvent->m_pRoutedEventInfoNext = m_pRoutedEventFirst;
    m_pRoutedEventFirst = pRoutedEvent;
}

void Type::RegisterRoutedCommand(RoutedCommandInfo* pCommandInfo)
{
    pCommandInfo->m_pCommandInfoNext = m_pCommandFirst;
    m_pCommandFirst = pCommandInfo;
}

}; // namespace AVUI
