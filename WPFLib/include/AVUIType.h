#pragma once

namespace AVUI {

class String;
class Property;
class EventRoute;
class RoutedEvent;
class RoutedCommand;
class TypeConverter;

class Type : public Object
{
    friend class TypeManager;
    friend class DependencyProperty;
    friend class SimpleProperty;

public:
    typedef Object BaseType;

    virtual const Type& get_Type() const { return get_StaticType(); };
    virtual bool IsType(const Type& rhs) const;
    static const Type& get_StaticType();

    int AddRef() { return 1; /* NOOP - Type is a STATIC only construct */ }
    int Release() { return 1; /* NOOP - Type is a STATIC only construct */ }

    Type(const char* szType, bool isValueType = false, bool isEnumType = false);
    bool operator==(const Type& rhs) const;
    bool operator!=(const Type& rhs) const;

    virtual const char* ToString() const { return m_szType; }

    unsigned int get_HashCode() { return m_uiHashCode; }
    bool IsAssignableFrom(const Type& typeCompare) const;
    bool IsTypeOrBase(const Type& typeCompare) const;
    bool HasInterface(const Type& interfaceName) const;
    const void* CastToInterface(const Type& interfaceName, const void* pvBaseData) const;
    void* CastToInterface(const Type& interfaceName, void* pvBaseData) const;

    const Property* get_Property(const String& propertyName) const;
    const Property* get_ContentProperty() const;
    const RoutedEvent* get_RoutedEvent(const String& routedEventName) const;
    const RoutedCommand* get_Command(const String& commandName) const;

    const Property* get_PropertyLocal(const String& propertyName) const;
    const Property* get_ContentPropertyLocal() const;
    const RoutedEvent* get_RoutedEventLocal(const String& eventName) const;
    const RoutedCommand* get_CommandLocal(const String& commandName) const;
    const Type* get_BaseType() const { return m_pBaseType; }
    const TypeConverter* get_TypeConverter() const { return m_pTypeConverter; }

    void RegisterProperty(PropertyInfo* pPropertyInfo);
    void RegisterBase(const Type& baseType);
    void RegisterInterface(InterfaceInfo* pInterfaceInfo);
    void RegisterRoutedEvent(RoutedEventInfo* pRoutedEvent);
    void RegisterRoutedCommand(RoutedCommandInfo* pCommand);
    void RegisterFactory(pfnObjectGenericMethod pfnFactory) { m_pfnFactory = pfnFactory; }
    void RegisterTypeConverter(const TypeConverter* pTypeConverter) { m_pTypeConverter = pTypeConverter; }


    ObjectPtr CreateInstance() const;
    unsigned int get_TypeID() const { return m_uiTypeID; }

    bool get_IsEnumType() const { return m_isEnumType; }
    bool get_IsValueType() const { return m_isValueType; }
    bool get_HasConstructor() const { return m_pfnFactory != NULL; }

private:

    static unsigned int get_UniqueTypeID()
    {
        static unsigned int globalID = 0;
        return globalID++;
    }

    void CalculateHashCode();

    const char* m_szType;
    unsigned int m_uiHashCode;
    int m_cch;

    Type* m_pTypeNext;

    PropertyInfo* m_pPropertyFirst;

    InterfaceInfo* m_pInterfaceFirst;

    RoutedEventInfo* m_pRoutedEventFirst;

    RoutedCommandInfo* m_pCommandFirst;

    const TypeConverter* m_pTypeConverter;

    pfnObjectGenericMethod m_pfnFactory;

    const Type* m_pBaseType;
    unsigned int m_uiTypeID;

    bool m_isEnumType;
    bool m_isValueType;
};

};
