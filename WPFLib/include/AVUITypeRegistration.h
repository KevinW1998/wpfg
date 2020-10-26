#pragma once

namespace AVUI {

typedef ObjectPtr (*pfnObjectGenericMethod)(Object** rgpParams, int cParams);

template<class T>
inline ObjectPtr DefaultObjectFactory(Object** rgpParams, int cParams)
{
    if(cParams != 0)
    {
        LibraryCriticalError();
    }
    return ObjectPtr(new T(), false);
}

class RoutedEvent;

class RoutedEventInfo
{
    friend class Type;
public:
    RoutedEventInfo(const RoutedEvent& event) : m_event(event), m_pRoutedEventInfoNext(NULL) { }

private:
    const RoutedEvent& m_event;
    RoutedEventInfo* m_pRoutedEventInfoNext;

};

class Property;
class PropertyInfo
{
    friend class Type;
    friend class TypeManager;
public:
    PropertyInfo(const Property& property) : m_property(property), m_pPropertyInfoNext(NULL) { }

private:
    const Property& m_property;
    PropertyInfo* m_pPropertyInfoNext;

};



class RoutedCommand;
class RoutedCommandInfo
{
    friend class Type;
public:
    RoutedCommandInfo(const RoutedCommand& command) : m_command(command), m_pCommandInfoNext(NULL) { }

private:
    const RoutedCommand& m_command;
    RoutedCommandInfo* m_pCommandInfoNext;

};

};

#include <AVUIPropertyRegistration.h>

namespace AVUI
{

class InterfaceInfo
{
    friend class Type;
public:
    InterfaceInfo() : m_pInterfaceNext(NULL) { }
    ~InterfaceInfo() { }

    virtual const Type& get_DerivedType() const = 0;
    virtual const Type& get_InterfaceType() const = 0;
    virtual void* Cast(void* pvBase) const = 0;
    virtual const void* Cast(const void* pvBase) const = 0;

private:
    InterfaceInfo* m_pInterfaceNext;
};

template<class interfaceType, class derivedType>
class InterfaceInfoInstance : public InterfaceInfo
{
public:
    const AVUI::Type& get_DerivedType() const { return AVUI::get_Type( (derivedType*) 0); }
    const AVUI::Type& get_InterfaceType() const { return AVUI::get_Type( (interfaceType*) 0); }
    void* Cast(void* pvBase) const { return (void*) ((interfaceType*) (derivedType*) pvBase); }
    const void* Cast(const void* pvBase) const { return (const void*) ((const interfaceType*) (const derivedType*) pvBase); }
};


#define DECLARE_ELEMENT(type, baseType) \
        typedef baseType BaseType; \
        typedef type _thisType; \
        virtual const AVUI::Type& get_Type() const { return get_StaticType(); }; \
        virtual bool IsType(const AVUI::Type& rhs) const \
        { \
            return get_StaticType().IsTypeOrBase(rhs); \
        } \
        static const AVUI::Type& get_StaticType();

#define DECLARE_VALUE_ELEMENT(type) \
        typedef type _thisType; \


#define REGISTER_ELEMENT_BEGIN(type) \
        namespace AVUI { \
        const Type& get_Type(type *) { return type::get_StaticType(); } \
        } \
        const AVUI::Type& type::get_StaticType() \
        { \
            typedef type _thisType; \
            static AVUI::Type s_type(#type); \
            static bool isRegistered = false; \
            if(!isRegistered) \
            { \
                isRegistered = true; \
                s_type.RegisterBase(GET_TYPE(_thisType::BaseType)); \


#define REGISTER_ELEMENT_END() \
            } \
            return s_type; \
        } \

#define REGISTER_ELEMENT(type) \
    REGISTER_ELEMENT_BEGIN(type) \
    REGISTER_ELEMENT_END() \


#define REGISTER_FACTORY(pfnFactory) \
        s_type.RegisterFactory(pfnFactory);

#define REGISTER_DEFAULT_FACTORY() \
        REGISTER_FACTORY(AVUI::DefaultObjectFactory<_thisType>); \

#define REGISTER_STATIC_CONSTRUCTOR(ctorName) \
    _thisType::ctorName(); \

#define REGISTER_INTERFACE(interfaceType) \
        { \
            static AVUI::InterfaceInfoInstance<interfaceType, _thisType> interfaceRegistration; \
            s_type.RegisterInterface(&interfaceRegistration); \
        } \

#define REGISTER_VALUE_TYPE_BEGIN(type) \
        namespace AVUI { \
        const Type& get_Type(type *) { \
            typedef type _thisType; \
            static AVUI::Type s_type(#type, true); \
            static bool isRegistered = false; \
            if(!isRegistered) \
            { \
                isRegistered = true; \

#define REGISTER_VALUE_TYPE_END() \
            } \
            return s_type; \
        } \
    }\

#define BEGIN_REGISTER_ENUM_TYPE(type) \
        namespace AVUI { \
        const Type& get_Type(type *) { \
            static AVUI::Type s_type(#type, false, true); \
            static bool isRegistered = false; \
            if(!isRegistered) \
            { \
                isRegistered = true; \

#define END_REGISTER_ENUM_TYPE() \
            } \
            return s_type; \
        } \
    } \


#define REGISTER_ENUM_TYPE(type) \
        BEGIN_REGISTER_ENUM_TYPE(type) \
        REGISTER_TYPE_CONVERTER(type) \
        END_REGISTER_ENUM_TYPE() \


#define REGISTER_INTERFACE_TYPE(type) \
        REGISTER_VALUE_TYPE_BEGIN(type) \
        REGISTER_VALUE_TYPE_END() \


#define REGISTER_TYPE_CONVERTER(converterClass) \
    static const converterClass typeConverter; \
    s_type.RegisterTypeConverter(&typeConverter); \


#define IMPLEMENT_EVENT_COMMON(eventName, eventArgs) \
    protected: \
    void invoke_##eventName(Object* pSender, eventArgs* pArgs) { InvokeEventHandler(&(get_##eventName##Event()), pSender, (Object*) pArgs); } \
    public: \
    void add_##eventName(AVUI::TDelegate<eventArgs>* pEventHandler) { AddEventHandler(&(get_##eventName##Event()), pEventHandler); } \
    void remove_##eventName(AVUI::TDelegate<eventArgs>* pEventHandler) { RemoveEventHandler(&(get_##eventName##Event()), pEventHandler); } \

#define IMPLEMENT_EVENT(eventName, eventArgs) \
    private: \
    static const AVUI::Object& get_##eventName##Event() { const static AVUI::Object event; return event; } \
    IMPLEMENT_EVENT_COMMON(eventName, eventArgs) \

#define IMPLEMENT_ROUTED_EVENT(routedEventName, routedEventArgs, routingStrategy) \
    public: \
    static const AVUI::RoutedEvent& get_##routedEventName##Event() { const static AVUI::RoutedEvent routedEvent(#routedEventName, GET_TYPE(_thisType), routingStrategy); return routedEvent; } \
    IMPLEMENT_EVENT_COMMON(routedEventName, routedEventArgs) \

#define ALIAS_ROUTED_EVENT(ownerType, routedEventName, routedEventArgs) \
    static const AVUI::RoutedEvent& get_##routedEventName##Event() { const static AVUI::RoutedEvent& routedEvent = ownerType::get_##routedEventName##Event(); return routedEvent; } \
    IMPLEMENT_EVENT_COMMON(routedEventName, routedEventArgs) \

#define REGISTER_ROUTED_EVENT(routedEventName) \
        { \
            static AVUI::RoutedEventInfo routedEventInfo(get_##routedEventName##Event()); \
            s_type.RegisterRoutedEvent(&routedEventInfo); \
        } \

#define DEFINE_EVENTHANDLER(eventType) \
    typedef AVUI::TDelegate<eventType##EventArgs> eventType##EventHandler;

#define IMPLEMENT_ROUTED_COMMAND(routedCommandName) \
    public: \
    static const AVUI::RoutedCommand& get_##routedCommandName() { const static AVUI::RoutedCommand command(#routedCommandName, GET_TYPE(_thisType)); return command; } \

#define REGISTER_ROUTED_COMMAND(routedCommandName) \
        { \
            static AVUI::RoutedCommandInfo commandInfo(get_##routedCommandName()); \
            s_type.RegisterRoutedCommand(&commandInfo); \
        } \

}
