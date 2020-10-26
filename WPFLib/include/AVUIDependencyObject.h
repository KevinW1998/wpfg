#pragma once

#include <AVUIEventableObject.h>
#include <AVUIItemStructMap.h>
#include <AVUIDependencyProperty.h>
#include <AVUIDependentList.h>
#include <AVUIUncommonField.h>
#include <AVUIExpression.h>

namespace AVUI {

class Expression;
class EventRoute;
class DeferredReference;
class Dispatcher;

class InheritanceBehavior
{
public:
    enum Enum
    {
        Default,
        SkipToAppNow,
        SkipToAppNext,
        SkipToThemeNow,
        SkipToThemeNext,
        SkipAllNow,
        SkipAllNext        
    };
};


struct EntryIndex
{
    EntryIndex() { }
    EntryIndex(int index, bool found = true) : Index(index), Found(found) { };
    bool Found;
    unsigned int Index;
};

class ModifiedValue : public Object
{
public:
    DECLARE_ELEMENT(ModifiedValue, Object);

    ObjectPtr ExpressionValue;
    ObjectPtr CoercedValue;
    ObjectPtr AnimatedValue;
    ObjectPtr BaseValue;
};


class EffectiveValueEntry
{
public:
    EffectiveValueEntry()  { Clear(); }

    EffectiveValueEntry(const DependencyProperty& dp)
    {
        m_propertyIndex = (unsigned short) dp.get_PropertyID();
        m_source = 0;
    }

    EffectiveValueEntry(const DependencyProperty& dp, BaseValueSource::Enum valueSource)
    {
        m_propertyIndex = (unsigned short) dp.get_PropertyID();
        m_value = DependencyProperty::get_UnsetValue();
        m_source = (unsigned short) valueSource;
    }

    EffectiveValueEntry(const DependencyProperty& dp, FullValueSource::Enum valueSource)
    {
        m_propertyIndex = (unsigned short) dp.get_PropertyID();
        m_value = DependencyProperty::get_UnsetValue();
        m_source = (unsigned short) valueSource;
    }

    static EffectiveValueEntry CreateDefaultValueEntry(const DependencyProperty& dp, Object* pValue)
    {
        EffectiveValueEntry entry = EffectiveValueEntry(dp, BaseValueSource::Default);
        entry.set_Value(pValue);

        return entry;
    }

    EffectiveValueEntry GetFlattenedEntry(RequestFlags::Enum requests)
    {
        if ((m_source & (FullValueSource::IsCoerced | FullValueSource::IsAnimated | FullValueSource::HasExpressionMarker | FullValueSource::IsExpression)) == 0)
        {
            return *this;
        }

        if (!get_HasModifiers())
        {
            EffectiveValueEntry entry;

            entry.set_BaseValueSource(get_BaseValueSource());
            entry.set_PropertyIndex(get_PropertyIndex());

            return entry;
        }

        EffectiveValueEntry entry2;
        entry2.set_BaseValueSource(get_BaseValueSource());
        entry2.set_PropertyIndex(get_PropertyIndex());
        entry2.set_IsDeferredReference(get_IsDeferredReference());

        ModifiedValue* pModifiedValue = get_ModifiedValue();
        if (get_IsCoerced())
        {
            if ((requests & RequestFlags::CoercionBaseValue) == RequestFlags::FullyResolved)
            {
                entry2.set_Value(pModifiedValue->CoercedValue);
                return entry2;
            }
            if (get_IsAnimated() && ((requests & RequestFlags::AnimationBaseValue) == RequestFlags::FullyResolved))
            {
                entry2.set_Value(pModifiedValue->AnimatedValue);
                return entry2;
            }
            if (get_IsExpression())
            {
                entry2.set_Value(pModifiedValue->ExpressionValue);
                return entry2;
            }
            entry2.set_Value(pModifiedValue->BaseValue);
            return entry2;
        }
        if (get_IsAnimated())
        {
            if ((requests & RequestFlags::AnimationBaseValue) == RequestFlags::FullyResolved)
            {
                entry2.set_Value(pModifiedValue->AnimatedValue);
                return entry2;
            }
            if (get_IsExpression())
            {
                entry2.set_Value(pModifiedValue->ExpressionValue);
                return entry2;
            }
            entry2.set_Value(pModifiedValue->BaseValue);
            return entry2;
        }
        Object* pExpressionValue = pModifiedValue->ExpressionValue;
        entry2.set_Value(pExpressionValue);

        if(!get_HasExpressionMarker() && entry2.m_value != NULL && entry2.m_value->IsType(GET_TYPE(DeferredReference)))
        {
            entry2.set_IsDeferredReference(true);
        }

        return entry2;
    }
    
    void Clear()
    {
        m_source = 0;
        m_propertyIndex = 0xFFFF;
        m_value = NULL;
    }


    ModifiedValue* get_ModifiedValue()
    {
        if(m_value != NULL)
        {
            return object_cast<ModifiedValue>(m_value);
        }
        return NULL;
    }

    const ModifiedValue* get_ModifiedValue() const
    {
        if(m_value != NULL)
        {
            return object_cast<ModifiedValue>(m_value);
        }
        return NULL;
    }


    ModifiedValue* EnsureModifiedValue()
    {
        if(m_value == NULL)
        {
            TRefCountedPtr<ModifiedValue> pModifiedValue = object_allocate<ModifiedValue>();
            m_value = pModifiedValue;

            return pModifiedValue;
        }

        ModifiedValue* pModifiedValueExisting = get_ModifiedValue();
        if(pModifiedValueExisting != NULL)
        {
            return pModifiedValueExisting;
        }

        TRefCountedPtr<ModifiedValue> pModifiedValue = object_allocate<ModifiedValue>();
        pModifiedValue->BaseValue = m_value;
        m_value = pModifiedValue;

        return pModifiedValue;
    }
    

    void ResetAnimatedValue()
    {
        if(get_IsAnimated())
        {
            set_IsAnimated(false);
            ModifiedValue* pModifiedValue = get_ModifiedValue();
            pModifiedValue->AnimatedValue = NULL;

            ObjectPtr baseValue = pModifiedValue->BaseValue;
            if(!get_HasModifiers())
            {
                m_value = baseValue;
            }
        }
    }

    void ResetExpressionValue()
    {
        if(get_IsExpression())
        {
            set_IsExpression(false);
            ModifiedValue* pModifiedValue = get_ModifiedValue();
            pModifiedValue->ExpressionValue = NULL;

            ObjectPtr baseValue = pModifiedValue->BaseValue;
            if(!get_HasModifiers())
            {
                m_value = baseValue;
            }
        }
    }

    void ResetCoercedValue()
    {
        if (get_IsCoerced())
        {
            ModifiedValue* pModifiedValue = get_ModifiedValue();
            pModifiedValue->CoercedValue = NULL;
            set_IsCoerced(false);

            ObjectPtr baseValue = pModifiedValue->BaseValue;
            if (!get_HasModifiers())
            {
                m_value = pModifiedValue->BaseValue;
            }
        }
    }

 

    void ResetValue(Object* pValue, bool hasExpressionMarker)
    {
        m_source = ((short) (m_source & FullValueSource::ValueSourceMask));
        m_value = pValue;
        if (hasExpressionMarker)
        {
            set_HasExpressionMarker(true);
        }
    }


    void RestoreExpressionMarker()
    {
        if (get_HasModifiers())
        {
            ModifiedValue* pModifiedValue = get_ModifiedValue();
            pModifiedValue->ExpressionValue = pModifiedValue->BaseValue;
            pModifiedValue->BaseValue = get_ExpressionInAlternativeStore();
            m_source = ((short) (m_source | FullValueSource::HasExpressionMarker | FullValueSource::IsExpression));
        }
        else
        {
            ObjectPtr pObj2 = get_Value();
            set_Value(get_ExpressionInAlternativeStore());
            SetExpressionValue(pObj2, get_ExpressionInAlternativeStore());
            m_source = ((short) (m_source | FullValueSource::HasExpressionMarker));
        }
    }

    void SetAnimatedValue(Object* pValue, Object* pBaseValue)
    {
        ModifiedValue* pModifiedValue = EnsureModifiedValue();
        pModifiedValue->AnimatedValue = pValue;
        set_IsAnimated(true);
    }

    void SetAnimationBaseValue(Object* pAnimationBaseValue)
    {
        if (!get_HasModifiers())
        {
            set_Value(pAnimationBaseValue);
        }
        else
        {
            ModifiedValue* pModifiedValue = get_ModifiedValue();
            if (get_IsExpression())
            {
                pModifiedValue->ExpressionValue = pAnimationBaseValue;
            }
            else
            {
                pModifiedValue->BaseValue = pAnimationBaseValue;
            }
        }
    }

    void SetCoercedValue(Object* pValue, Object* pBaseValue)
    {
        ModifiedValue* pModifiedValue = EnsureModifiedValue();
        pModifiedValue->CoercedValue = pValue;
        set_IsCoerced(true);
    }

    void SetCoersionBaseValue(Object* pCoersionBaseValue)
    {
        if (!get_HasModifiers())
        {
            set_Value(pCoersionBaseValue);
        }
        else
        {
            ModifiedValue* pModifiedValue = get_ModifiedValue();
            if (get_IsAnimated())
            {
                pModifiedValue->AnimatedValue = pCoersionBaseValue;
            }
            else if (get_IsExpression())
            {
                pModifiedValue->ExpressionValue = pCoersionBaseValue;
            }
            else
            {
                pModifiedValue->BaseValue = pCoersionBaseValue;
            }
        }
    }


    void SetExpressionValue(Object* pValue, Object* pBaseValue)
    {
        ModifiedValue* pModifiedValue = EnsureModifiedValue();
        pModifiedValue->ExpressionValue = pValue;
        set_IsExpression(true);
    }



    bool ReadPrivateFlag(FullValueSource::Enum bit) const
    {
        return (m_source & bit) != 0;
    }

    void WritePrivateFlag(FullValueSource::Enum bit, bool value)
    {
        if(value)
        {
            m_source = (unsigned short) (((unsigned short) m_source) | bit);
        }
        else
        {
            m_source = (unsigned short) (m_source & ~bit);
        }
    }

    bool get_HasModifiers() const { return (m_source & FullValueSource::ModifiersMask) != 0; }

    bool get_IsAnimated() const { return ReadPrivateFlag(FullValueSource::IsAnimated); };
    void set_IsAnimated(bool isAnimated) { WritePrivateFlag(FullValueSource::IsAnimated, isAnimated); }

    bool get_IsCoerced() const { return ReadPrivateFlag(FullValueSource::IsCoerced); };
    void set_IsCoerced(bool isCoerced) { WritePrivateFlag(FullValueSource::IsCoerced, isCoerced); }

    bool get_IsExpression() const { return ReadPrivateFlag(FullValueSource::IsExpression); };
    void set_IsExpression(bool isExpression) { WritePrivateFlag(FullValueSource::IsExpression, isExpression); }

    bool get_IsDeferredReference() const { return ReadPrivateFlag(FullValueSource::IsDeferredReference); };
    void set_IsDeferredReference(bool isDeferredReference) { WritePrivateFlag(FullValueSource::IsDeferredReference, isDeferredReference); }

    bool get_HasExpressionMarker() const { return ReadPrivateFlag(FullValueSource::HasExpressionMarker); };
    void set_HasExpressionMarker(bool hasExpressionMarker) { WritePrivateFlag(FullValueSource::HasExpressionMarker, hasExpressionMarker); }

	    int get_PropertyIndex() const { return m_propertyIndex; };
    void set_PropertyIndex(int propertyIndex) { m_propertyIndex = (unsigned short) propertyIndex; };

    void set_BaseValueSource(BaseValueSource::Enum valueSource)
    {
        m_source = (unsigned short) (((FullValueSource::Enum) ((unsigned short) (m_source & ~FullValueSource::ValueSourceMask))) | ((FullValueSource::Enum) valueSource));
    }

    BaseValueSource::Enum get_BaseValueSource() const
    {
        return (BaseValueSource::Enum) ((unsigned short) (m_source & FullValueSource::ValueSourceMask));
    }

    FullValueSource::Enum get_FullValueSource() const
    {
        return (FullValueSource::Enum) m_source;
    }

    void set_LocalValue(Object* pValue)
    {
        if(!get_HasModifiers())
        {
            m_value = pValue;
        }
        else
        {
            get_ModifiedValue()->BaseValue = pValue;
        }
    }

    Object* get_Value() const { return m_value; }
    void set_Value(Object* pValue) { m_value = pValue; }

    Object* get_LocalValue()
    {
        if(get_BaseValueSource() != BaseValueSource::Local)
        {
            return DependencyProperty::get_UnsetValue();
        }

        if(!get_HasModifiers())
        {
            return m_value;
        }

        return get_ModifiedValue()->BaseValue;
    }

    static Object* get_ExpressionInAlternativeStore() { static Object expressionInAlternativeStore; return &expressionInAlternativeStore; }

private:
    unsigned short m_source;
    unsigned short m_propertyIndex;
    ObjectPtr m_value;
};


class DependencyPropertyChangedEventArgs : public EventArgs
{
public:

    DECLARE_ELEMENT(DependencyPropertyChangedEventArgs, EventArgs);

    DependencyPropertyChangedEventArgs(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadataIn, bool isValueChange, EffectiveValueEntry* pOldEntry, EffectiveValueEntry* pNewEntry, OperationType::Enum operationType)
    : DependencyPropertyChanged(dp), IsValueChange(isValueChange), OperationType(operationType), IsSubPropertyChange(false), pMetadata(pMetadataIn)
    {        
        pOldValue = pOldEntry->GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
        pNewValue = pNewEntry->GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
    }

    DependencyPropertyChangedEventArgs(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadataIn, Object* pValue)
    : DependencyPropertyChanged(dp), IsValueChange(false), pOldValue(pValue), pNewValue(pValue), OperationType(OperationType::Unknown), IsSubPropertyChange(true), pMetadata(pMetadataIn) { }

    DependencyPropertyChangedEventArgs(const DependencyProperty& dp, Object* pOldValue, Object* pNewValue)
    : DependencyPropertyChanged(dp), IsValueChange(true), pOldValue(pOldValue), pNewValue(pNewValue), OperationType(OperationType::Unknown), IsSubPropertyChange(false), pMetadata(dp.get_DefaultMetadata()) { }


    const DependencyProperty& DependencyPropertyChanged;
    bool IsValueChange;
    bool IsSubPropertyChange;
    Object* pOldValue;
    Object* pNewValue;
    DependencyPropertyMetadata* pMetadata;
    OperationType::Enum OperationType;
};

DEFINE_EVENTHANDLER(DependencyPropertyChanged);


class DependencyObject : public EventableObject
{
    friend class ContentPresenter;
    friend class UIElement;
    friend class Visual;
    friend class Animatable;
    friend class XamlParser;
    friend class FrameworkTemplate;
    friend class BindingExpression;
    friend class AnimationStorage;
    friend class Helper;
    friend class RelativeObjectRef;
    friend class GridSplitter;
    friend class PopupControlService;
    friend class UncommonFieldHelper;

    class EffectiveValue;
public:
    DependencyObject();
    virtual ~DependencyObject();

    DECLARE_ELEMENT(DependencyObject, Object);

    ObjectPtr GetValue(const DependencyProperty& dependencyProperty);
    void CoerceValue(const DependencyProperty& dependencyProperty);

    EffectiveValueEntry GetValueEntry(const EntryIndex& entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, RequestFlags::Enum requests);
    EffectiveValueEntry GetEffectiveValue(const EntryIndex& entryIndex, const DependencyProperty& dp, RequestFlags::Enum requests);


    void SetExpressionValue(EntryIndex entryIndex, Object* pValue, Object* pBaseValue);
    void SetAnimatedValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, Object* pValue, Object* pBaseValue);
    EffectiveValueEntry SetCoercedValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, Object* pValue, Object* pBaseValue);
    void SetValue(const DependencyProperty& dependencyProperty, Object* pValue);
    void SetValueCommon(const DependencyProperty& dp, Object* pValue, DependencyPropertyMetadata* pMetadata, bool coerceWithDeferredReference, OperationType::Enum operationType, bool isInternal);

    void ClearValueCommon(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata);
    Object* ReadLocalValue(const DependencyProperty& dp);
    Object* ReadLocalValueEntry(EntryIndex entryIndex, const DependencyProperty& dp, bool allowDeferredReferences);
    void ResetEffectiveValue(unsigned int entryIndex, const DependencyProperty& dp);

    // No implementation for DO
    virtual DependencyObject* get_InheritanceContext() { return NULL; }
    virtual void AddInheritanceContext(DependencyObject*, const DependencyProperty* pDP) { }
    virtual void RemoveInheritanceContext(DependencyObject*, const DependencyProperty* pDP) { }
    virtual bool get_HasMultipleInheritanceContexts() { return false; }

    bool ProvideSelfAsInheritanceContext(Object* pValue, const DependencyProperty* pDP);
    bool RemoveSelfAsInheritanceContext(Object* pValue, const DependencyProperty* pDP);


    UpdateResult::Enum UpdateEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, 
                                            EffectiveValueEntry oldEntry, EffectiveValueEntry* pNewEntry, bool coerceWithDeferredReference, 
                                            OperationType::Enum operationType);

    EffectiveValueEntry EvaluateEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry oldEntry, EffectiveValueEntry newEntry, OperationType::Enum operationType);
    virtual void EvaluateBaseValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pNewEntry) { };
    virtual void EvaluateAnimatedValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pNewEntry) { }

    EntryIndex CheckEntryIndex(EntryIndex entryIndex, int targetIndex);

    void UnsetEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata);

    void SetEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry newEntry, EffectiveValueEntry oldEntry);
    void SetEffectiveValue(EntryIndex entryIndex, const DependencyProperty* pDP, int targetIndex, DependencyPropertyMetadata* pMetadata, Object* pValue, BaseValueSource::Enum valueSource);


    void RemoveEntry(unsigned int entryIndex);
    void InsertEntry(const EffectiveValueEntry& entry, unsigned int entryIndex);

    void ClearValue(const DependencyProperty& dependencyProperty);
    void InvalidateProperty(const DependencyProperty& dependencyProperty);
    void InvalidateSubProperty(const DependencyProperty& dp);

    EntryIndex LookupEntry(int targetIndex) const;

    int get_EffectiveValuesCount() const { return m_effectiveValuesCount; }

    virtual void Seal() { if(!get_IsSealed()) m_isSealed = true; };
    void CheckSealed() { if(get_IsSealed()) LibraryCriticalError(); }
    bool get_IsSealed() const { return m_isSealed; }

    int get_TemplateChildIndex() { return m_templateChildIndex; }

    bool get_HasAnimatedProperties() const { return m_hasAnimatedProperties; }
    bool get_IsSelfInheritanceParent() const { return m_isSelfInheritanceParent; }

    static void ChangeExpressionSources(Expression* pExpr, DependencyObject* pDO, const DependencyProperty* pDP, ItemStructList<TRefCountedPtr<DependencySource> >* pNewSources);

    IMPLEMENT_EVENT(InheritanceContextChanged, EventArgs);

protected:
                                                           
    // Functions for working with dependency properties
    virtual void OnPropertyChanged(const DependencyPropertyChangedEventArgs&);

    // Functions for working with effective values table directly.
    EffectiveValue* FindEffectiveValue(const DependencyProperty& dependencyProperty) const;
    void ClearExistingEffectiveValue(const DependencyProperty& dependencyProperty);

    Dispatcher* get_Dispatcher();

private:

    void ClearEffectiveValues();
    void SynchronizeInheritanceParent(DependencyObject* pParent);
    void NotifyPropertyChange(const DependencyPropertyChangedEventArgs&);
    void NotifySubPropertyChange(const DependencyProperty& dp);

    BaseValueSource::Enum GetValueSource(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, bool* pHasModifiers);
    BaseValueSource::Enum GetValueSource(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, bool* pHasModifiers, bool* pIsExpression, bool* pIsAnimated, bool* pIsCoerced);


    void set_InheritableEffectiveValuesCount(unsigned int value) { m_inheritableEffectiveValuesCount = value; }
    unsigned int get_InheritableEffectiveValuesCount() { return m_inheritableEffectiveValuesCount; }
    virtual DependencyObject* get_InheritanceParent() { return NULL; }
    EffectiveValueEntry* get_EffectiveValues() { return m_rgEffectiveValues; }

    void SetIsSelfInheritanceParent();
    void SetInheritanceParent(DependencyObject* pNewParent);
    void MergeInheritableProperties(DependencyObject* pInheritanceParent);

    DependencyPropertyMetadata* SetupPropertyChange(const DependencyProperty& dp);
    EffectiveValueEntry EvaluateExpression(EntryIndex entryIndex, const DependencyProperty& dp, Expression* pExpression, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry oldEntry, EffectiveValueEntry newEntry);

    bool Equals(const DependencyProperty& dp, Object* pObject1, Object* pObject2);

    void set_HasAnimatedProperties(bool hasAnimatedProperties) { m_hasAnimatedProperties = hasAnimatedProperties; }

    static UncommonField<ItemStructMap<DependentListPtr> > DependentListMapField;  
    static void UpdateSourceDependentLists(DependencyObject* pDO, const DependencyProperty& dp, ItemStructList<TRefCountedPtr<DependencySource> >* pSources, Expression* pExpression, bool isAdd);

    void set_TemplateChildIndex(int index) { m_templateChildIndex = index; }

    EffectiveValueEntry* m_rgEffectiveValues;
    unsigned short m_effectiveValuesCount;
    unsigned short m_effectiveValuesCapacity;

    TWeakPtr<Object> m_pContextStorage;

    bool m_isSealed : 1;
    bool m_isSelfInheritanceParent : 1;
    bool m_hasAnimatedProperties : 1;
    int m_templateChildIndex : 15;
    unsigned int m_inheritableEffectiveValuesCount : 14;
};


typedef TRefCountedPtr<DependencyObject> DependencyObjectPtr;
typedef TWeakPtr<DependencyObject> DependencyObjectWeakPtr;



}; // namespace AVUI
