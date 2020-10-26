#pragma once

#define REGISTER_PROPERTY(propertyName) \
        { \
            static AVUI::PropertyInfo propertyInfo(_thisType::get_##propertyName##Property()); \
            s_type.RegisterProperty(&propertyInfo); \
        } \

namespace AVUI
{

void* GetPtrFromTypeHelper(void* p);

template<class T>
T* GetPtrFromType(void* p)
{
    if((get_Type( (T*) 0).get_IsValueType()))
    {
        p = GetPtrFromTypeHelper(p);
    }

    return reinterpret_cast<T*>(p);
};

};


////////////////////////////////////////////////////////////////////////////
// Generic DP definition

#define IMPLEMENT_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata) \
    static const AVUI::DependencyProperty& get_##propertyName##Property() { static AVUI::DependencyPropertyMetadata metadata(propertyMetadata); const static AVUI::DependencyProperty property(#propertyName, GET_TYPE(_thisType), GET_TYPE(propertyType), defaultValue, &metadata); return property; } \

#define IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata)


// Reference object DP Definition
#define IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata); \
    propertyType* get_##propertyName() { return AVUI::object_cast<propertyType>(GetValue(get_##propertyName##Property())); } \
    void set_##propertyName(propertyType* pValue) { SetValue(get_##propertyName##Property(), (Object*) pValue); } \

// Readonly reference object DP
#define IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata); \
    propertyType* get_##propertyName() { return AVUI::object_cast<propertyType>(GetValue(get_##propertyName##Property())); } \

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
// Value DP definition
#define IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, AVUI::BoxValue(defaultValue), propertyMetadata)

#define IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata); \
    const propertyType &get_##propertyName() { return AVUI::UnboxValue< propertyType >(GetValue(get_##propertyName##Property())); } \
    void set_##propertyName(const propertyType& value) { SetValue(get_##propertyName##Property(), AVUI::BoxValue(value)); } \

#define IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata); \
    const propertyType &get_##propertyName() { return AVUI::UnboxValue< propertyType >(GetValue(get_##propertyName##Property())); } \
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Enum DP definition
#define IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, AVUI::BoxValue((int)defaultValue), propertyMetadata)

#define IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata); \
    const propertyType::Enum get_##propertyName() { return (propertyType::Enum) UnboxValue< int >(GetValue(get_##propertyName##Property())); } \
    void set_##propertyName(const propertyType::Enum& value) { SetValue(get_##propertyName##Property(), AVUI::BoxValue((int)value)); } \

#define IMPLEMENT_READONLY_ENUM_DEPENDENCY_PROPERTY(propertyName, propertyType, defaultValue, propertyMetadata) \
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(propertyName, propertyType, defaultValue, propertyMetadata); \
    const propertyType::Enum get_##propertyName() { return (propertyType::Enum) UnboxValue< int >(GetValue(get_##propertyName##Property())); } \
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Reference property definition
#define IMPLEMENT_REFERENCE_PROPERTY(propertyName, propertyType, isContent) \
    static const AVUI::SimpleProperty& get_##propertyName##Property() { const static AVUI::SimpleProperty property(#propertyName, GET_TYPE(_thisType), GET_TYPE(propertyType), isContent, __Get##propertyName##ValueThunk, __Set##propertyName##ValueThunk); return property; } \
    private: \
    static AVUI::ObjectPtr __Get##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject) { return (AVUI::Object*) AVUI::GetPtrFromType<_thisType>(pObject)->get_##propertyName(); } \
    static void __Set##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject, AVUI::Object* pValue) { AVUI::GetPtrFromType<_thisType>(pObject)->set_##propertyName(AVUI::object_cast<propertyType>(pValue)); } \
    public: \

#define IMPLEMENT_READONLY_REFERENCE_PROPERTY(propertyName, propertyType, isContent) \
    static const AVUI::SimpleProperty& get_##propertyName##Property() { const static AVUI::SimpleProperty property(#propertyName, GET_TYPE(_thisType), GET_TYPE(propertyType), isContent, __Get##propertyName##ValueThunk, NULL); return property; } \
    private: \
    static AVUI::ObjectPtr __Get##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject) { return (AVUI::Object*) AVUI::GetPtrFromType<_thisType>(pObject)->get_##propertyName(); } \
    public: \
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Value property definition
#define IMPLEMENT_VALUE_PROPERTY(propertyName, propertyType, isContent) \
    static const AVUI::SimpleProperty& get_##propertyName##Property() { const static AVUI::SimpleProperty property(#propertyName, GET_TYPE(_thisType), GET_TYPE(propertyType), isContent, __Get##propertyName##ValueThunk, __Set##propertyName##ValueThunk); return property; } \
    private: \
    static AVUI::ObjectPtr __Get##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject) { return BoxValue((AVUI::GetPtrFromType<_thisType>(pObject)->get_##propertyName())); } \
    static void __Set##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject, AVUI::Object* pValue) { AVUI::GetPtrFromType<_thisType>(pObject)->set_##propertyName(UnboxValue<propertyType>(pValue)); } \
    public: \

#define IMPLEMENT_READONLY_VALUE_PROPERTY(propertyName, propertyType, isContent) \
    static const AVUI::SimpleProperty& get_##propertyName##Property() { const static AVUI::SimpleProperty property(#propertyName, GET_TYPE(_thisType), GET_TYPE(propertyType), isContent, __Get##propertyName##ValueThunk, NULL); return property; } \
    private: \
    static AVUI::ObjectPtr __Get##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject) { return BoxValue(AVUI::GetPtrFromType<_thisType>(pObject)->get_##propertyName()); } \
    public: \


/////////////////////////////////////////////////////////////////////////////////
// Enum property definition
#define IMPLEMENT_ENUM_PROPERTY(propertyName, propertyType, isContent) \
    static const AVUI::SimpleProperty& get_##propertyName##Property() { const static AVUI::SimpleProperty property(#propertyName, GET_TYPE(_thisType), GET_TYPE(propertyType), isContent, __Get##propertyName##ValueThunk, __Set##propertyName##ValueThunk); return property; } \
    private: \
    static AVUI::ObjectPtr __Get##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject) { return BoxValue((int)((AVUI::GetPtrFromType<_thisType>(pObject)->get_##propertyName()))); } \
    static void __Set##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject, AVUI::Object* pValue) { AVUI::GetPtrFromType<_thisType>(pObject)->set_##propertyName( (propertyType::Enum) UnboxValue<int>(pValue)); } \
    public: \

#define IMPLEMENT_READONLY_ENUM_PROPERTY(propertyName, propertyType, isContent) \
    static const AVUI::SimpleProperty& get_##propertyName##Property() { const static AVUI::SimpleProperty property(#propertyName, GET_TYPE(_thisType), GET_TYPE(propertyType), isContent, __Get##propertyName##ValueThunk, NULL); return property; } \
    private: \
    static AVUI::ObjectPtr __Get##propertyName##ValueThunk(const AVUI::Property* pProperty, AVUI::Object* pObject) { return BoxValue((int)((AVUI::GetPtrFromType<_thisType>(pObject)->get_##propertyName()))); } \
    public: \
    
