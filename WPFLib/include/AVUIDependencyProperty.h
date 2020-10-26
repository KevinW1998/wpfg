#pragma once

#include <AVUIPropertyName.h>

namespace AVUI {

class DependencyObject;
class DependencyPropertyChangedEventArgs;
class Property;
class TypeConverter;
class DependencyProperty;
class TypeManager;

typedef ObjectPtr (*pfnGetReadOnlyValueCallback)(DependencyObject* pDO, BaseValueSource::Enum* pValueSource);
typedef ObjectPtr (*pfnCoerceValueCallback)(DependencyObject* pDO, Object* pBaseValue);
typedef bool (*pfnValidateValueCallback)(Object*);
typedef void (*pfnPropertyChangedCallback)(Object* pDO, const DependencyPropertyChangedEventArgs&);
typedef ObjectPtr (*pfnGetValue)(const Property* pProperty, Object*);
typedef void (*pfnSetValue)(const Property* pProperty, Object* pObject, Object* pValue);

class DependencyPropertyMetadata
{
    friend class DependencyProperty;

public:
    
    DependencyPropertyMetadata(
                     int flags, 
                     pfnPropertyChangedCallback propertyChangedCallback = NULL,
                     pfnValidateValueCallback validateValueCallback = NULL) : m_flags(flags), m_pMetadataNext(NULL),
                                                                              m_propertyChangedCallback(propertyChangedCallback), 
                                                                              m_validateValueCallback(validateValueCallback),
                                                                              m_coerceValueCallback(NULL), m_getReadOnlyValueCallback(NULL) { }

    DependencyPropertyMetadata(
                     int flags, 
                     pfnGetReadOnlyValueCallback getReadOnlyValueCallback,
                     pfnPropertyChangedCallback propertyChangedCallback = NULL) : m_flags(flags), m_pMetadataNext(NULL),
                                                                              m_propertyChangedCallback(propertyChangedCallback), 
                                                                              m_validateValueCallback(NULL),
                                                                              m_coerceValueCallback(NULL), m_getReadOnlyValueCallback(getReadOnlyValueCallback) { }
    DependencyPropertyMetadata(
                     int flags, 
                     pfnPropertyChangedCallback propertyChangedCallback,
                     pfnCoerceValueCallback coerceValueCallback) : m_flags(flags), m_pMetadataNext(NULL),
                                                                   m_propertyChangedCallback(propertyChangedCallback), 
                                                                   m_validateValueCallback(NULL),
                                                                   m_coerceValueCallback(coerceValueCallback), m_getReadOnlyValueCallback(NULL) { }

    unsigned int get_Flags() const { return m_flags; }
    pfnPropertyChangedCallback get_PropertyChangedCallback() const { return m_propertyChangedCallback; }
    pfnValidateValueCallback get_ValidateValueCallback() const { return m_validateValueCallback; }
    bool get_IsInherited() const { return !!(m_flags & PropertyFlag::Inherits); }
    bool get_IsReadOnly() const { return !!(m_flags & PropertyFlag::ReadOnly); }
    bool get_IsDataBindingAllowed() const { return true; } // TBD - Additional databinding work?


    pfnGetReadOnlyValueCallback get_GetReadOnlyValueCallback() { return m_getReadOnlyValueCallback; }
    pfnCoerceValueCallback get_CoerceValueCallback() const { return m_coerceValueCallback; }
    Object* get_DefaultValue() const { return m_pDefaultValue; }
    Object* GetDefaultValue(DependencyObject* pDO, const DependencyProperty& dp) const { return m_pDefaultValue; }
    const Type& get_MetadataType() const { return *m_pMetadataType; }

    bool get_BindsTwoWayByDefault() const { return false; }
    UpdateSourceTrigger::Enum get_DefaultUpdateSourceTrigger() { return UpdateSourceTrigger::PropertyChanged; }

private:

    void CleanupAllocatedData() { set_DefaultValue(NULL); }

    void set_DefaultValue(Object* pDefaultValue) { m_pDefaultValue = pDefaultValue; }
    void set_MetadataType(const Type& type) { m_pMetadataType = &type; }

    const Type* m_pMetadataType;
    ObjectPtr m_pDefaultValue;
    DependencyPropertyMetadata* m_pMetadataNext;
    unsigned int m_flags;
    pfnValidateValueCallback m_validateValueCallback;
    pfnPropertyChangedCallback m_propertyChangedCallback;
    pfnCoerceValueCallback m_coerceValueCallback;
    pfnGetReadOnlyValueCallback m_getReadOnlyValueCallback;
};

class Property : public Object
{
    friend class Type;
    friend class TypeManager;
public:
    DECLARE_ELEMENT(Property, Object);

    int AddRef() { return 1; /* NOOP - Property is a STATIC only construct */ }
    int Release() { return 1; /* NOOP - Property is a STATIC only construct */ }

    Property(const PropertyName& propertyName, const Type& definingType, const Type& propertyType,
             pfnGetValue getValue, pfnSetValue setValue) :
                 m_propertyName(propertyName), m_definingType(definingType), m_propertyType(propertyType), 
                 m_pfnGetValue(getValue), m_pfnSetValue(setValue), m_uiPropertyID(get_UniqueGlobalID()), m_pPropertyConverter(NULL) { }

    const PropertyName& get_PropertyName() const { return m_propertyName; }
    const Type& get_DefiningType() const { return m_definingType; }
    const Type& get_PropertyType() const { return m_propertyType; }
    unsigned int get_PropertyID() const { return m_uiPropertyID; }

    bool operator==(const Property& rhs) const { return get_PropertyID() == rhs.get_PropertyID(); }
    bool operator!=(const Property& rhs) const { return get_PropertyID() != rhs.get_PropertyID(); }

    virtual bool get_IsContentProperty() const = 0;

    static unsigned int get_UniqueGlobalID()
    {
        static unsigned int globalID = 0;
        return globalID++;
    }

    void SetValue(Object* pObjectToSetOn, Object* pObjectValue) const { m_pfnSetValue(this, pObjectToSetOn, pObjectValue); }
    ObjectPtr GetValue(Object* pObject) const { return m_pfnGetValue(this, pObject); }

    void RegisterPropertyConverter(const TypeConverter* pPropertyConverter) const { m_pPropertyConverter = pPropertyConverter; }

    const TypeConverter* get_PropertyConverter() const { return m_pPropertyConverter; }

private:

    Property(const Property&);

    pfnGetValue m_pfnGetValue;
    pfnSetValue m_pfnSetValue;

    mutable const TypeConverter* m_pPropertyConverter;

    PropertyName m_propertyName;
    const Type& m_propertyType;
    const Type& m_definingType;
    unsigned int m_uiPropertyID;
    Property* m_pPropertyNext;
};

class DependencyProperty : public Property
{
    friend class TypeManager;
public:
    DECLARE_ELEMENT(DependencyProperty, Property);

    DependencyProperty(const PropertyName& propertyName, const Type& definingType, const Type& propertyType,
                       Object* defaultValue, DependencyPropertyMetadata* pPropertyMetadata);
    ~DependencyProperty() { };

    const DependencyProperty& AddOwner(const Type& addingType, Object* pNewDefaultValue, DependencyPropertyMetadata* pPropertyMetadata) const;
    void OverrideMetadata(const Type& addingType, Object* pNewDefaultValue, DependencyPropertyMetadata* pPropertyMetadata) const;

    static Object* get_UnsetValue() { static UnsetValue unsetValue; return &unsetValue; }

    unsigned int get_Flags() const { return m_pPropertyMetadata->get_Flags(); }
    bool get_IsContentProperty() const { return !!(get_Flags() & PropertyFlag::IsContentProperty); }

    pfnPropertyChangedCallback get_PropertyChangedCallback() const { return m_pPropertyMetadata->get_PropertyChangedCallback(); }
    pfnValidateValueCallback get_ValidateValueCallback() const { return m_pPropertyMetadata->get_ValidateValueCallback(); }

    bool IsValidValue(Object* pValue) const;
    bool IsValidValueInternal(Object* pValue) const { return IsValidValue(pValue); }

    bool get_IsPotentiallyInherited() const { return m_pPropertyMetadata->get_IsInherited(); }
    bool get_IsDefaultValueChanged() const { return true; }
    bool get_IsReadOnly() const { return m_pPropertyMetadata->get_IsReadOnly(); }
    bool get_IsObjectType() const { return get_PropertyType() == GET_TYPE(Object); }

    DependencyPropertyMetadata* GetMetadata(const Type& metadataType) const;
    DependencyPropertyMetadata* get_DefaultMetadata() const
    {
        return m_pPropertyMetadata; 
    }

private:

    void CleanupAllocatedData() const;

    static void SetDependencyPropertyValue(const Property* pProperty, Object* pObject, Object* pValue);
    static ObjectPtr GetDependencyPropertyValue(const Property* pProperty, Object* pObject);

    class UnsetValue : public Object
    {
    public:

        int AddRef() { return 1; /* NOOP - UnsetValue is a STATIC only construct */ }
        int Release() { return 1; /* NOOP - UnsetValue is a STATIC only construct */ }
    };

    DependencyPropertyMetadata* m_pPropertyMetadata;
};

class SimpleProperty : public Property
{
public:
    DECLARE_ELEMENT(SimpleProperty, Property);

    SimpleProperty(const PropertyName& propertyName, const Type& definingType, const Type& propertyType,
                   bool isContentProperty, pfnGetValue getValue, pfnSetValue setValue);
    ~SimpleProperty() { };

    bool get_IsContentProperty() const { return m_isContentProperty; }

private:
    bool m_isContentProperty;
};




/*


class PropertyMetadata : public Object
{
public:

    PropertyMetadata() { }
    PropertyMetadata(Object* pDefaultValue) { set_DefaultValue(pDefaultValue); }
    PropertyMetadata(PropertyChangedCallback propertyChangedCallback) { set_PropertyChangedCallback(propertyChangedCallback); }
    PropertyMetadata(Object* pDefaultValue, PropertyChangedCallback propertyChangedCallback, CoerceValueCallback coerceValueCallback) 
    { 
        set_DefaultValue(pDefaultValue);
        set_PropertyChangedCallback(propertyChangedCallback);
        set_CoerceValueCallback(coerceValueCallback); 
    }

    void _ClearCachedDefaultValue(DependencyObject* pOwner, DependencyProperty* pProperty);
    TRefCountedPtr<PropertyMetadata> _Copy(DependencyProperty* pDP);   
    virtual TRefCountedPtr<PropertyMetadata> _CreateInstance();
    bool _DefaultValueWasSet();
    Object* _GetDefaultValue(DependencyObject* pOwner, DependencyProperty* pProperty);
    void _InvokeMerge(PropertyMetadata* pBaseMetadata, DependencyProperty* pDP);
    static void _PromoteAllCachedDefaultValues(DependencyObject* pOwner);
    bool _ReadFlag(MetadataFlags::Enum flag);
    static void _RemoveAllCachedDefaultValues(Freezable* pOwner);
    void _Seal(DependencyProperty* pDP, const Type* pTargetType);

protected:

    virtual void Merge(PropertyMetadata* pBaseMetadata, DependencyProperty* pDP);
    virtual void OnApply(DependencyProperty* pDP, const Type* pTargetType);

private:

    void DefaultValueCachePromotionCallback(ArrayList* pList, int key, Object* pValue);
    void DefaultValueCacheRemovalCallback(ArrayList* pList, int key, Object* pValue);
    Object* GetCachedDefaultValue(DependencyObject* pOwner, DependencyProperty* pDP);
    bool IsModified(MetadataFlags::Enum flags);
    void SetCachedDefaultValue(DependencyObject* pOwner, DependencyProperty* pProperty, Object* pValue);
    void SetModified(MetadataFlags::Enum flag);
    void WriteFlag(MetadataFlags::Flag, bool value);
    
 
    



    CoerceValueCallback m_pfnCoerceValueCallback;
    ObjectPtr m_pDefaultValue;
    static UncommonField<FrugalMapBase> m_defaultValueFactoryCache;
    MetadataFlags::Enum m_flags;
    static FrugalMapIterationCallback m_promotionCallback;
    PropertyChangedCallback m_propertyChangedCallback;
    static FrugalMapIterationCallback m_removalCallback;
};


*/









}; // namespace AVUI
