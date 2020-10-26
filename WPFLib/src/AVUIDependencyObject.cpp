#include <AVUICommon.h>
#include <AVUIDependencyObject.h>
#include <AVUIDependencyProperty.h>
#include <AVUIUIString.h>
#include <AVUIExpression.h>
#include <AVUIDependentList.h>
#include <AVUIDeferredReference.h>
#include <AVUITypeManager.h>
#include <AVUIAnimatable.h>
#include <AVUIDispatcher.h>

REGISTER_ELEMENT_BEGIN(AVUI::DependencyObject)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::DependencyPropertyChangedEventArgs)

REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::ModifiedValue)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::DeferredReference)

REGISTER_ELEMENT_END()


namespace AVUI {

UncommonField<ItemStructMap<DependentListPtr> > DependencyObject::DependentListMapField;  

DependencyObject::DependencyObject() : m_rgEffectiveValues(NULL), m_effectiveValuesCount(0), m_effectiveValuesCapacity(0), m_isSealed(false), m_templateChildIndex(-1), m_hasAnimatedProperties(false), m_inheritableEffectiveValuesCount(0), m_isSelfInheritanceParent(false)
{
};

DependencyObject::~DependencyObject()
{
    ClearEffectiveValues();
}

void DependencyObject::ClearEffectiveValues()
{
    for (int i = 0; i < m_effectiveValuesCount; i++)
    {
        m_rgEffectiveValues[i].Clear();
        m_rgEffectiveValues[i].~EffectiveValueEntry();
    }
    m_effectiveValuesCount = 0;

    if (m_rgEffectiveValues != NULL)
    {
        Object::FreeMemory(m_rgEffectiveValues);
        m_rgEffectiveValues = NULL;
    }
}

ObjectPtr DependencyObject::GetValue(const DependencyProperty& dp)
{
    return GetValueEntry(LookupEntry(dp.get_PropertyID()), dp, NULL, RequestFlags::FullyResolved).get_Value();
}

void DependencyObject::CoerceValue(const DependencyProperty& dp)
{
    EffectiveValueEntry newEntry(dp, FullValueSource::IsCoerced);
    UpdateEffectiveValue(LookupEntry(dp.get_PropertyID()), dp, dp.GetMetadata(get_Type()), EffectiveValueEntry(), &newEntry, false, OperationType::Unknown);
}

 
BaseValueSource::Enum DependencyObject::GetValueSource(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, bool* pHasModifiers)
{
    bool isExpression;
    bool isAnimated;
    bool isCoerced;
    return GetValueSource(dp, pMetadata, pHasModifiers, &isExpression, &isAnimated, &isCoerced);
}

BaseValueSource::Enum DependencyObject::GetValueSource(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, bool* pHasModifiers, bool* pIsExpression, bool* pIsAnimated, bool* pIsCoerced)
{
    EntryIndex entryIndex = LookupEntry(dp.get_PropertyID());
    if (entryIndex.Found)
    {
        EffectiveValueEntry entry = m_rgEffectiveValues[entryIndex.Index];
        *pHasModifiers = entry.get_HasModifiers();
        *pIsExpression = entry.get_IsExpression();
        *pIsAnimated = entry.get_IsAnimated();
        *pIsCoerced = entry.get_IsCoerced();
        return entry.get_BaseValueSource();
    }
    *pIsExpression = false;
    *pIsAnimated = false;
    *pIsCoerced = false;
    if (dp.get_IsReadOnly())
    {
        if (pMetadata == NULL)
        {
            pMetadata = dp.GetMetadata(get_Type());
        }

        pfnGetReadOnlyValueCallback getReadOnlyValueCallback = pMetadata->get_GetReadOnlyValueCallback();
        if (getReadOnlyValueCallback != NULL)
        {
            BaseValueSource::Enum source;

            getReadOnlyValueCallback(this, &source);

            *pHasModifiers = false;

            return source;
        }
    }
    if (dp.get_IsPotentiallyInherited())
    {
        if (pMetadata == NULL)
        {
            pMetadata = dp.GetMetadata(get_Type());
        }
        if (pMetadata->get_IsInherited())
        {
            DependencyObject* pInheritanceParent = get_InheritanceParent();
            if (pInheritanceParent != NULL && pInheritanceParent->LookupEntry(dp.get_PropertyID()).Found)
            {
                *pHasModifiers = false;
                return BaseValueSource::Inherited;
            }
        }
    }
    *pHasModifiers = false;
    return BaseValueSource::Default;
}


 


EffectiveValueEntry DependencyObject::GetValueEntry(const EntryIndex& entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, RequestFlags::Enum requests)
{
    EffectiveValueEntry entry;

    if (dp.get_IsReadOnly())
    {
        if (pMetadata == NULL)
        {
            pMetadata = dp.GetMetadata(get_Type());
        }

        pfnGetReadOnlyValueCallback getReadOnlyValueCallback = pMetadata->get_GetReadOnlyValueCallback();
        if (getReadOnlyValueCallback != NULL)
        {
            BaseValueSource::Enum source;
            entry = EffectiveValueEntry(dp);
            entry.set_Value(getReadOnlyValueCallback(this, &source));
            entry.set_BaseValueSource(source);
            return entry;
        }
    }

    if (entryIndex.Found)
    {
        if ((requests & RequestFlags::RawEntry) != RequestFlags::FullyResolved)
        {
            entry = m_rgEffectiveValues[entryIndex.Index];
        }
        else
        {
            entry = GetEffectiveValue(entryIndex, dp, requests);
        }
    }
    else
    {
        entry = EffectiveValueEntry(dp, BaseValueSource::Unknown);
    }
    if (entry.get_Value() != DependencyProperty::get_UnsetValue())
    {
        return entry;
    }
    if (dp.get_IsPotentiallyInherited())
    {
        if (pMetadata == NULL)
        {
            pMetadata = dp.GetMetadata(get_Type());
        }
        if (pMetadata->get_IsInherited())
        {
            DependencyObject* pInheritanceParent = get_InheritanceParent();
            if (pInheritanceParent != NULL)
            {
                EntryIndex newEntryIndex = pInheritanceParent->LookupEntry(dp.get_PropertyID());
                if (newEntryIndex.Found)
                {
                    entry = pInheritanceParent->GetEffectiveValue(newEntryIndex, dp, (RequestFlags::Enum)(requests & RequestFlags::DeferredReferences));
                    entry.set_BaseValueSource(BaseValueSource::Inherited);
                }
            }
        }
        if (entry.get_Value() != DependencyProperty::get_UnsetValue())
        {
            return entry;
        }
    }
    if ((requests & RequestFlags::SkipDefault) != RequestFlags::FullyResolved)
    {
        return entry;
    }
    /*
    if (dp.get_IsPotentiallyUsingDefaultValueFactory())
    {
        if (pMetadata == NULL)
        {
            pMetadata = dp.GetMetadata(get_Type());
        }
        if (((requests & (RequestFlags::RawEntry | RequestFlags::DeferredReferences)) != RequestFlags::FullyResolved) && pMetsdata->get_UsingDefaultValueFactory())
        {
            entry.set_BaseValueSource(BaseValueSource::Default);
            entry.set_IsDeferredReference(true);
            entry.set_Value(object_allocate<DeferredMutableDefaultReference>(pMetadata, this, dp));
            return entry;
        }
    }
    else 
    */

    if (!dp.get_IsDefaultValueChanged())
    {
        return EffectiveValueEntry::CreateDefaultValueEntry(dp, dp.get_DefaultMetadata()->get_DefaultValue());
    }
    if (pMetadata == NULL)
    {
        pMetadata = dp.GetMetadata(get_Type());
    }
    return EffectiveValueEntry::CreateDefaultValueEntry(dp, pMetadata->GetDefaultValue(this, dp));
}

EffectiveValueEntry DependencyObject::GetEffectiveValue(const EntryIndex& entryIndex, const DependencyProperty& dp, RequestFlags::Enum requests)
{
    EffectiveValueEntry entry = m_rgEffectiveValues[entryIndex.Index];
    EffectiveValueEntry flattenedEntry = entry.GetFlattenedEntry(requests);
    if (((requests & (RequestFlags::RawEntry | RequestFlags::DeferredReferences)) == RequestFlags::FullyResolved) && flattenedEntry.get_IsDeferredReference())
    {
        if (!entry.get_HasModifiers())
        {
            if (entry.get_HasExpressionMarker())
            {
                return flattenedEntry;
            }
            ObjectPtr pDeferredRefValue = ((DeferredReference*) entry.get_Value())->GetValue(entry.get_BaseValueSource());
            if (!dp.IsValidValue(pDeferredRefValue))
            {
                LibraryCriticalError();
            }
            EntryIndex newEntryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
            entry.set_Value(pDeferredRefValue);
            entry.set_IsDeferredReference(false);
            m_rgEffectiveValues[newEntryIndex.Index] = entry;
            return entry;
        }
        ModifiedValue* pModifiedValue = entry.get_ModifiedValue();
        ObjectPtr pNewDeferredRefValue = ((DeferredReference*) (Object*) pModifiedValue->ExpressionValue)->GetValue(entry.get_BaseValueSource());
        if (!dp.IsValidValue(pNewDeferredRefValue))
        {
            LibraryCriticalError();
        }
        EntryIndex newEntryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
        pModifiedValue->ExpressionValue = pNewDeferredRefValue;
        entry.set_IsDeferredReference(false);
        m_rgEffectiveValues[newEntryIndex.Index] = entry;
        flattenedEntry.set_IsDeferredReference(false);
        flattenedEntry.set_Value(pNewDeferredRefValue);
    }
    return flattenedEntry;
}




EntryIndex DependencyObject::LookupEntry(int targetIndex) const
{
    unsigned int left = 0;
    unsigned int right = get_EffectiveValuesCount();
    if (right > 0)
    {
        do
        {
            unsigned int middle = (right + left) / 2;
            int propertyIndex = m_rgEffectiveValues[middle].get_PropertyIndex();
            if (targetIndex == propertyIndex)
            {
                return EntryIndex(middle);
            }
            if (targetIndex <= propertyIndex)
            {
                right = middle;
            }
            else
            {
                left = middle + 1;
            }
        }
        while (left < right);
    }
    return EntryIndex(left, false);
}


void DependencyObject::SetValue(const DependencyProperty& dp, Object* pValue)
{
    DependencyPropertyMetadata* pMetadata = SetupPropertyChange(dp);
    SetValueCommon(dp, pValue, pMetadata, false, OperationType::Unknown, false);
}

void DependencyObject::SetValueCommon(const DependencyProperty& dp, Object* pValue, DependencyPropertyMetadata* pMetadata, bool coerceWithDeferredReference, OperationType::Enum operationType, bool isInternal)
{
    if (get_IsSealed())
    {
        LibraryCriticalError();
    }

    Expression* pExpr = NULL;
    TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > pNewSources;

    EntryIndex entryIndex = LookupEntry(dp.get_PropertyID());
    if (pValue == DependencyProperty::get_UnsetValue())
    {
        ClearValueCommon(entryIndex, dp, pMetadata);
    }
    else
    {
        EffectiveValueEntry oldEntry;
        EffectiveValueEntry newEntry;
        bool flag = false;
        bool isExpressionInAlternativeStore = pValue == EffectiveValueEntry::get_ExpressionInAlternativeStore();
        if (!isExpressionInAlternativeStore)
        {
            bool flag3 = isInternal ? dp.IsValidValueInternal(pValue) : dp.IsValidValue(pValue);
            if (!flag3 || dp.get_IsObjectType())
            {
                pExpr = object_cast<Expression>(pValue);
                if (pExpr != NULL)
                {
                    if (!pExpr->get_Attachable())
                    {
                        LibraryCriticalError();
                    }
                    pNewSources = pExpr->GetSources();

                    // No sources validation needs to be performed, deviation
                    // ValidateSources(this, pNewSources, pExpr);
                }
                else
                {
                    flag = pValue != NULL && pValue->IsType(GET_TYPE(DeferredReference));
                    if (!flag && !flag3)
                    {
                        LibraryCriticalError();
                    }
                }
            }
        }
        if (operationType == OperationType::ChangeMutableDefaultValue)
        {
            oldEntry = EffectiveValueEntry(dp, BaseValueSource::Default);
            oldEntry.set_Value(pValue);
        }
        else
        {
            oldEntry = GetValueEntry(entryIndex, dp, pMetadata, RequestFlags::RawEntry);
        }

        Object* pLocalValue = oldEntry.get_LocalValue();
        TRefCountedPtr<Expression> pOldExpression = NULL;
        TRefCountedPtr<Expression> pExpressionInAlternativeStore = NULL;
        if (oldEntry.get_HasExpressionMarker())
        {
            if (pExpr == NULL)
            {
                // TODO -- Expression in alternate store ?
                // pExpressionInAlternativeStore = _getExpressionCore(this, dp, pMetadata);
            }
            if (pExpressionInAlternativeStore != NULL)
            {
                pLocalValue = pExpressionInAlternativeStore;
                pOldExpression = pExpressionInAlternativeStore;
            }
            else
            {
                pLocalValue = DependencyProperty::get_UnsetValue();
            }
        }
        else
        {
            pOldExpression = oldEntry.get_IsExpression() ? object_cast<Expression>(pLocalValue) : NULL;
        }
        bool flag5 = false;
        if ((pOldExpression != NULL) && (pExpr == NULL))
        {
            ObjectPtr pDeferredRefValue;
            if (flag)
            {                
                pDeferredRefValue = ((DeferredReference*) pValue)->GetValue(BaseValueSource::Local);
                pValue = pDeferredRefValue;
                flag = false;
            }
            flag5 = pOldExpression->SetValue(this, dp, pValue);
            entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
        }
        if (flag5)
        {
            if (entryIndex.Found)
            {
                newEntry = m_rgEffectiveValues[entryIndex.Index];
            }
            else
            {
                newEntry = EffectiveValueEntry::CreateDefaultValueEntry(dp, pMetadata->GetDefaultValue(this, dp));
            }
        }
        else
        {
            newEntry = EffectiveValueEntry(dp, BaseValueSource::Local);
            if (pOldExpression != NULL && pOldExpression != pExpressionInAlternativeStore)
            {
                TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > pSources = pOldExpression->GetSources();
                UpdateSourceDependentLists(this, dp, pSources, pOldExpression, false);
                pOldExpression->OnDetach(this, dp);
                entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
            }
            if (pExpr == NULL)
            {
                newEntry.set_IsDeferredReference(flag);
                newEntry.set_Value(pValue);
                newEntry.set_HasExpressionMarker(isExpressionInAlternativeStore);
            }
            else
            {
                SetEffectiveValue(entryIndex, &dp, dp.get_PropertyID(), pMetadata, pExpr, BaseValueSource::Local);
                entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
                UpdateSourceDependentLists(this, dp, pNewSources, pExpr, true);
                pExpr->MarkAttached();
                pExpr->OnAttach(this, dp);
                entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
                newEntry = EvaluateExpression(entryIndex, dp, pExpr, pMetadata, oldEntry, m_rgEffectiveValues[entryIndex.Index]);
                entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
            }
        }
        UpdateEffectiveValue(entryIndex, dp, pMetadata, oldEntry, &newEntry, coerceWithDeferredReference, operationType);
    }
}


UpdateResult::Enum DependencyObject::UpdateEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry oldEntry, EffectiveValueEntry* pNewEntry, bool coerceWithDeferredReference, OperationType::Enum operationType)
{
    int targetIndex = dp.get_PropertyID();
    if (oldEntry.get_BaseValueSource() == BaseValueSource::Unknown)
    {
        oldEntry = GetValueEntry(entryIndex, dp, pMetadata, RequestFlags::RawEntry);
    }
    ObjectPtr pOldValue = oldEntry.GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
    if ((pNewEntry->get_BaseValueSource() != BaseValueSource::Unknown) && (pNewEntry->get_BaseValueSource() < oldEntry.get_BaseValueSource()))
    {
        return UpdateResult::None;
    }
    bool isNewValueUnset = false;
    bool isCoerced = false;
    bool isNewValueInitiallyLocal = false;
    if (pNewEntry->get_Value() == DependencyProperty::get_UnsetValue())
    {
        isCoerced = pNewEntry->get_FullValueSource() == FullValueSource::IsCoerced;
        isNewValueUnset = true;
        if (pNewEntry->get_BaseValueSource() == BaseValueSource::Local)
        {
            isNewValueInitiallyLocal = true;
        }
    }
    if (isNewValueUnset || (!pNewEntry->get_IsAnimated() && (oldEntry.get_IsAnimated() || ((oldEntry.get_IsExpression() && pNewEntry->get_IsExpression()) && (pNewEntry->get_ModifiedValue()->BaseValue == oldEntry.get_ModifiedValue()->BaseValue)))))
    {
        if (!isCoerced)
        {
            *pNewEntry = EvaluateEffectiveValue(entryIndex, dp, pMetadata, oldEntry, *pNewEntry, operationType);
            entryIndex = CheckEntryIndex(entryIndex, targetIndex);
            bool isNewValueNowSet = pNewEntry->get_Value() != DependencyProperty::get_UnsetValue();
            if (!isNewValueNowSet && pMetadata->get_IsInherited())
            {
                DependencyObject* pInheritanceParent = get_InheritanceParent();
                if (pInheritanceParent != NULL)
                {
                    EntryIndex entry = pInheritanceParent->LookupEntry(dp.get_PropertyID());
                    if (entry.Found)
                    {
                        isNewValueNowSet = true;
                        *pNewEntry = pInheritanceParent->m_rgEffectiveValues[entry.Index].GetFlattenedEntry(RequestFlags::FullyResolved);
                        pNewEntry->set_BaseValueSource(BaseValueSource::Inherited);
                    }
                }
            }
            if (!isNewValueNowSet)
            {
                *pNewEntry = EffectiveValueEntry::CreateDefaultValueEntry(dp, pMetadata->GetDefaultValue(this, dp));
            }
        }
        else if (!oldEntry.get_HasModifiers())
        {
            *pNewEntry = oldEntry;
        }
        else
        {
            *pNewEntry = EffectiveValueEntry(dp, oldEntry.get_BaseValueSource());
            ModifiedValue* pModifiedValue = oldEntry.get_ModifiedValue();
            Object* pBaseValue = pModifiedValue->BaseValue;
            pNewEntry->set_Value(pBaseValue);
            pNewEntry->set_IsDeferredReference(oldEntry.get_IsDeferredReference());
            pNewEntry->set_HasExpressionMarker(oldEntry.get_HasExpressionMarker());

            if (oldEntry.get_IsExpression())
            {
                pNewEntry->SetExpressionValue(pModifiedValue->ExpressionValue, pBaseValue);
            }
            if (oldEntry.get_IsAnimated())
            {
                pNewEntry->SetAnimatedValue(pModifiedValue->AnimatedValue, pBaseValue);
            }
        }
    }
    if (pMetadata->get_CoerceValueCallback() != NULL && (!isNewValueInitiallyLocal || pNewEntry->get_FullValueSource() != (FullValueSource::Enum) BaseValueSource::Default))
    {
        ObjectPtr pCoersionBaseValue = pNewEntry->GetFlattenedEntry(RequestFlags::CoercionBaseValue).get_Value();
        bool isDeferredReference = oldEntry.get_IsDeferredReference();
        if (pNewEntry->get_IsDeferredReference()) // TODO -- How can we do this? && (!coerceWithDeferredReference || (dp.get_OwnerType() != metadata.CoerceValueCallback.Method.DeclaringType)))
        {
            pCoersionBaseValue = ((DeferredReference*) (Object*) pCoersionBaseValue)->GetValue(pNewEntry->get_BaseValueSource());
            pNewEntry->SetCoersionBaseValue(pCoersionBaseValue);
            pNewEntry->set_IsDeferredReference(false);
            entryIndex = CheckEntryIndex(entryIndex, targetIndex);
        }
        ObjectPtr pCoercedValue = pMetadata->get_CoerceValueCallback()(this, pCoersionBaseValue);
        entryIndex = CheckEntryIndex(entryIndex, targetIndex);
        if (!Equals(dp, pCoercedValue, pCoersionBaseValue))
        {
            if (pCoercedValue == DependencyProperty::get_UnsetValue())
            {
                if (isDeferredReference)
                {
                    pOldValue = ((DeferredReference*) (Object*) pOldValue)->GetValue(oldEntry.get_BaseValueSource());
                    isDeferredReference = false;
                    entryIndex = CheckEntryIndex(entryIndex, targetIndex);
                }
                pCoercedValue = pOldValue;
            }
            if (!dp.IsValidValue(pCoercedValue))
            {
                LibraryCriticalError();
            }
            pNewEntry->SetCoercedValue(pCoercedValue, pCoersionBaseValue);
        }
    }
    if (pNewEntry->get_FullValueSource() != (FullValueSource::Enum) BaseValueSource::Default)
    {
        if (pNewEntry->get_BaseValueSource() == BaseValueSource::Inherited && !get_IsSelfInheritanceParent())
        {
            UnsetEffectiveValue(entryIndex, dp, pMetadata);
        }
        else
        {
            SetEffectiveValue(entryIndex, dp, pMetadata, *pNewEntry, oldEntry);
        }
    }
    else
    {
        UnsetEffectiveValue(entryIndex, dp, pMetadata);
    }
    bool isAValueChange = !Equals(dp, pOldValue, pNewEntry->GetFlattenedEntry(RequestFlags::FullyResolved).get_Value());
    UpdateResult::Enum result = isAValueChange ? UpdateResult::ValueChanged : UpdateResult::None;
    if (isAValueChange || 
        (operationType == OperationType::ChangeMutableDefaultValue && oldEntry.get_BaseValueSource() != pNewEntry->get_BaseValueSource()) || 
        (pMetadata->get_IsInherited() && oldEntry.get_BaseValueSource() != pNewEntry->get_BaseValueSource() && operationType != OperationType::AddChild && operationType != OperationType::RemoveChild && operationType != OperationType::Inherit)
       )
    {
        result = (UpdateResult::Enum) (result | UpdateResult::NotificationSent);
        
        NotifyPropertyChange(DependencyPropertyChangedEventArgs(dp, pMetadata, isAValueChange, &oldEntry, pNewEntry, operationType));
    }

    bool isOldValueLocal = oldEntry.get_FullValueSource() == (FullValueSource::Enum) BaseValueSource::Local;
    bool isNewValueLocal = pNewEntry->get_FullValueSource() == (FullValueSource::Enum) BaseValueSource::Local;
    if (result != UpdateResult::None || isOldValueLocal != isNewValueLocal)
    {
        if (isOldValueLocal)
        {
            RemoveSelfAsInheritanceContext(oldEntry.get_LocalValue(), &dp);
        }
        if (isNewValueLocal)
        {
            ProvideSelfAsInheritanceContext(pNewEntry->get_LocalValue(), &dp);
        }
    }
    return result;
}

EffectiveValueEntry DependencyObject::EvaluateExpression(EntryIndex entryIndex, const DependencyProperty& dp, Expression* pExpression, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry oldEntry, EffectiveValueEntry newEntry)
{
    ObjectPtr pUnsetValue = pExpression->GetValue(this, dp);
    bool flag = false;
    if (pUnsetValue != DependencyProperty::get_UnsetValue() && pUnsetValue != Expression::get_NoValue())
    {
        flag = pUnsetValue != NULL && pUnsetValue->IsType(GET_TYPE(DeferredReference));
        if (!flag && !dp.IsValidValue(pUnsetValue))
        {
            LibraryCriticalError();
        }
    }
    else
    {
        if (pUnsetValue == Expression::get_NoValue())
        {
            newEntry.SetExpressionValue(Expression::get_NoValue(), pExpression);
            if (!dp.get_IsReadOnly())
            {
                EvaluateBaseValueCore(dp, pMetadata, &newEntry);
                pUnsetValue = newEntry.GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
            }
            else
            {
                pUnsetValue = DependencyProperty::get_UnsetValue();
            }
        }
        if (pUnsetValue == DependencyProperty::get_UnsetValue())
        {
            pUnsetValue = pMetadata->GetDefaultValue(this, dp);
        }
    }
    newEntry.set_IsDeferredReference(flag);
    newEntry.SetExpressionValue(pUnsetValue, pExpression);
    return newEntry;
}



Object* DependencyObject::ReadLocalValueEntry(EntryIndex entryIndex, const DependencyProperty& dp, bool allowDeferredReferences)
{
    if (!entryIndex.Found)
    {
        return DependencyProperty::get_UnsetValue();
    }
    EffectiveValueEntry entry = m_rgEffectiveValues[entryIndex.Index];
    ObjectPtr pLocalValue = entry.get_LocalValue();
    if (!allowDeferredReferences && entry.get_IsDeferredReference())
    {
        DeferredReference* pReference = object_cast<DeferredReference>(pLocalValue);
        if (pReference != NULL)
        {
            pLocalValue = pReference->GetValue(entry.get_BaseValueSource());
        }
    }
    if (pLocalValue == EffectiveValueEntry::get_ExpressionInAlternativeStore())
    {
        pLocalValue = DependencyProperty::get_UnsetValue();
    }
    return pLocalValue;
}

Object* DependencyObject::ReadLocalValue(const DependencyProperty& dp)
{
    EntryIndex index = LookupEntry(dp.get_PropertyID());

    return ReadLocalValueEntry(index, dp, false);
}

EffectiveValueEntry DependencyObject::EvaluateEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry oldEntry, EffectiveValueEntry newEntry, OperationType::Enum operationType)
{
    ObjectPtr pUnsetValue = DependencyProperty::get_UnsetValue();
    bool flag = newEntry.get_BaseValueSource() == BaseValueSource::Local;
    bool flag2 = flag && (newEntry.get_Value() == DependencyProperty::get_UnsetValue());
    bool flag3 = false;
    if (flag2)
    {
        newEntry.set_BaseValueSource(BaseValueSource::Unknown);
    }
    else
    {
        pUnsetValue = flag ? newEntry.get_LocalValue() : oldEntry.get_LocalValue();
        if (pUnsetValue == EffectiveValueEntry::get_ExpressionInAlternativeStore())
        {
            pUnsetValue = DependencyProperty::get_UnsetValue();
        }
        else
        {
            flag3 = flag ? newEntry.get_IsExpression() : oldEntry.get_IsExpression();
        }
    }
    if (pUnsetValue != DependencyProperty::get_UnsetValue())
    {
        newEntry = EffectiveValueEntry(dp, BaseValueSource::Local);
        newEntry.set_Value(pUnsetValue);
        newEntry.set_IsDeferredReference(pUnsetValue != NULL && pUnsetValue->IsType(GET_TYPE(DeferredReference)));
        if (flag3)
        {
            newEntry = EvaluateExpression(entryIndex, dp, (Expression*) (Object*)pUnsetValue, pMetadata, oldEntry, newEntry);
            entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
            pUnsetValue = newEntry.get_ModifiedValue()->ExpressionValue;
        }
    }
    if (!dp.get_IsReadOnly())
    {
        EvaluateBaseValueCore(dp, pMetadata, &newEntry);
        if (newEntry.get_BaseValueSource() == BaseValueSource::Unknown)
        {
            newEntry = EffectiveValueEntry::CreateDefaultValueEntry(dp, pMetadata->GetDefaultValue(this, dp));
        }
        pUnsetValue = newEntry.GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
        entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
        if (oldEntry.get_IsAnimated())
        {
            newEntry.ResetCoercedValue();
            EvaluateAnimatedValueCore(dp, pMetadata, &newEntry);
            pUnsetValue = newEntry.GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
        }
    }
    if (pUnsetValue == DependencyProperty::get_UnsetValue())
    {
        newEntry = EffectiveValueEntry::CreateDefaultValueEntry(dp, pMetadata->GetDefaultValue(this, dp));
    }
    return newEntry;
}


void DependencyObject::SetEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry newEntry, EffectiveValueEntry oldEntry)
{
   if (((pMetadata != NULL) && pMetadata->get_IsInherited()) && ((newEntry.get_BaseValueSource() != BaseValueSource::Inherited) && !get_IsSelfInheritanceParent()))
    {
        SetIsSelfInheritanceParent();
        entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
    }
    bool needsExpressionMarkerRestored = false;
    if (oldEntry.get_HasExpressionMarker() && !newEntry.get_HasExpressionMarker())
    {
        BaseValueSource::Enum baseValueSource = newEntry.get_BaseValueSource();
        needsExpressionMarkerRestored = baseValueSource == BaseValueSource::ThemeStyle || 
               baseValueSource == BaseValueSource::ThemeStyleTrigger || 
               baseValueSource == BaseValueSource::Style || 
               baseValueSource == BaseValueSource::TemplateTrigger || 
               baseValueSource == BaseValueSource::StyleTrigger || 
               baseValueSource == BaseValueSource::ParentTemplate ||
               baseValueSource == BaseValueSource::ParentTemplateTrigger;
    }
    if (needsExpressionMarkerRestored)
    {
        newEntry.RestoreExpressionMarker();
    }
    else if (oldEntry.get_IsExpression() && oldEntry.get_ModifiedValue()->ExpressionValue == Expression::get_NoValue())
    {
// Not sure about this one -- In initial implementation, it's just 
//        newEntry.SetExpressionValue(newEntry.get_Value(), oldEntry.get_ModifiedValue()->BaseValue); -- newEntry must be flattened or else this is catastrophic

        newEntry.SetExpressionValue(newEntry.GetFlattenedEntry(RequestFlags::FullyResolved).get_Value(), oldEntry.get_ModifiedValue()->BaseValue);
    }

    if (entryIndex.Found)
    {
        m_rgEffectiveValues[entryIndex.Index] = newEntry;
    }
    else
    {
        InsertEntry(newEntry, entryIndex.Index);
        if (pMetadata != NULL && pMetadata->get_IsInherited())
        {
            set_InheritableEffectiveValuesCount(get_InheritableEffectiveValuesCount() + 1);
        }
    }
}

void DependencyObject::SetEffectiveValue(EntryIndex entryIndex, const DependencyProperty* pDP, int targetIndex, DependencyPropertyMetadata* pMetadata, Object* pValue, BaseValueSource::Enum valueSource)
{
   EffectiveValueEntry entry;
    if (((pMetadata != NULL) && pMetadata->get_IsInherited()) && ((valueSource != BaseValueSource::Inherited) && !get_IsSelfInheritanceParent()))
    {
        SetIsSelfInheritanceParent();
        entryIndex = CheckEntryIndex(entryIndex, pDP->get_PropertyID());
    }
    if (entryIndex.Found)
    {
        entry = m_rgEffectiveValues[entryIndex.Index];
    }
    else
    {
        entry.set_PropertyIndex(targetIndex);
        InsertEntry(entry, entryIndex.Index);
        if (pMetadata != NULL && pMetadata->get_IsInherited())
        {
            set_InheritableEffectiveValuesCount(get_InheritableEffectiveValuesCount() + 1);
        }
    }
    bool hasExpressionMarker = pValue == EffectiveValueEntry::get_ExpressionInAlternativeStore();
    if (!hasExpressionMarker && entry.get_HasExpressionMarker() && 
            (valueSource == BaseValueSource::ThemeStyle || 
             valueSource == BaseValueSource::ThemeStyleTrigger || 
             valueSource == BaseValueSource::Style || 
             valueSource == BaseValueSource::TemplateTrigger || 
             valueSource == BaseValueSource::StyleTrigger || 
             valueSource == BaseValueSource::ParentTemplate || 
             valueSource == BaseValueSource::ParentTemplateTrigger))
    {
        entry.set_BaseValueSource(valueSource);
        entry.set_IsDeferredReference(false);
        entry.SetExpressionValue(pValue, EffectiveValueEntry::get_ExpressionInAlternativeStore());
        entry.ResetAnimatedValue();
        entry.ResetCoercedValue();
    }
    else if (entry.get_IsExpression() && entry.get_ModifiedValue()->ExpressionValue == Expression::get_NoValue())
    {
        entry.SetExpressionValue(pValue, entry.get_ModifiedValue()->BaseValue);
    }
    else
    {
        entry.set_BaseValueSource(valueSource);
        entry.ResetValue(pValue, hasExpressionMarker);
        if (valueSource != BaseValueSource::Default)
        {
            entry.set_IsDeferredReference(pValue != NULL && pValue->IsType(GET_TYPE(DeferredReference)));
        }
        else
        {
            entry.set_IsDeferredReference(false);
        }
    }
    m_rgEffectiveValues[entryIndex.Index] = entry;
}

void DependencyObject::UnsetEffectiveValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata)
{
    if (entryIndex.Found)
    {
        RemoveEntry(entryIndex.Index);
        if (pMetadata != NULL && pMetadata->get_IsInherited())
        {
            set_InheritableEffectiveValuesCount(get_InheritableEffectiveValuesCount() - 1);
        }
    }
}

void DependencyObject::InvalidateProperty(const DependencyProperty& dp)
{
   EffectiveValueEntry newEntry(dp, BaseValueSource::Unknown);
   UpdateEffectiveValue(LookupEntry(dp.get_PropertyID()), dp, dp.GetMetadata(get_Type()), EffectiveValueEntry(), &newEntry, false, OperationType::Unknown);
}

void DependencyObject::ClearValue(const DependencyProperty& dp)
{
    DependencyPropertyMetadata* pMetadata = SetupPropertyChange(dp);
    EntryIndex entryIndex = LookupEntry(dp.get_PropertyID());
    ClearValueCommon(entryIndex, dp, pMetadata);
}



void DependencyObject::InvalidateSubProperty(const DependencyProperty& dp)
{
    NotifyPropertyChange(DependencyPropertyChangedEventArgs(dp, dp.GetMetadata(get_Type()), GetValue(dp)));
}

void DependencyObject::RemoveEntry(unsigned int entryIndex)
{

    for (int i = entryIndex; i < (unsigned short) (m_effectiveValuesCount - 1); i++)
    {
        m_rgEffectiveValues[i] = m_rgEffectiveValues[i + 1];
    }

    m_rgEffectiveValues[m_effectiveValuesCount - 1].Clear();

    m_effectiveValuesCount-= 1;
}


EntryIndex DependencyObject::CheckEntryIndex(EntryIndex entryIndex, int targetIndex)
{
    if(get_EffectiveValuesCount() > 0 && m_effectiveValuesCapacity > entryIndex.Index)
    {
        if (m_rgEffectiveValues[entryIndex.Index].get_PropertyIndex() == targetIndex)
        {
            return EntryIndex(entryIndex.Index);
        }
    }

    return LookupEntry(targetIndex);
}


void DependencyObject::NotifyPropertyChange(const DependencyPropertyChangedEventArgs& args)
{
    OnPropertyChanged(args);

    ItemStructMap<DependentListPtr>* pMap = DependentListMapField.GetValue(this);
    if(pMap != NULL)
    {
        int index = pMap->Search(args.DependencyPropertyChanged.get_PropertyID());
        if(index >= 0)
        {
            DependentList* pMapValue = (*pMap)[index];

            if(pMapValue != NULL)
            {
                pMapValue->InvalidateDependents(this, args);
            }
        }
    }
}

void DependencyObject::NotifySubPropertyChange(const DependencyProperty& dp)
{
    InvalidateSubProperty(dp);

    Animatable* pAnimatable = object_cast<Animatable>(this);
    if (pAnimatable != NULL)
    {
        pAnimatable->FireChanged();
    }
}



void DependencyObject::OnPropertyChanged(const DependencyPropertyChangedEventArgs& e) 
{ 
    if (e.pMetadata->get_PropertyChangedCallback() != NULL)
    {
        e.pMetadata->get_PropertyChangedCallback()(this, e);
    }

}

void DependencyObject::ClearValueCommon(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata)
{
    if (get_IsSealed())
    {
        LibraryCriticalError();
    }
    EffectiveValueEntry oldEntry = GetValueEntry(entryIndex, dp, pMetadata, RequestFlags::RawEntry);
    Object* pLocalValue = oldEntry.get_LocalValue();

    Expression* pExpression = oldEntry.get_IsExpression() ? object_cast<Expression>(pLocalValue) : NULL;
    if (pExpression != NULL)
    {
        TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > pSources = pExpression->GetSources();

        UpdateSourceDependentLists(this, dp, pSources, pExpression, false);

        pExpression->OnDetach(this, dp);
        entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
    }

    EffectiveValueEntry newEntry(dp, BaseValueSource::Local);

    UpdateEffectiveValue(entryIndex, dp, pMetadata, oldEntry, &newEntry, false, OperationType::Unknown);
}

void DependencyObject::ResetEffectiveValue(unsigned int entryIndex, const DependencyProperty& dp)
{
    EffectiveValueEntry& entry = m_rgEffectiveValues[entryIndex];
    if (entry.get_IsAnimated())
    {                             
        entry.ResetAnimatedValue();
    }
    if (entry.get_IsExpression())
    {                             
        entry.ResetExpressionValue();
    }

    Object* pLocalValue = entry.get_LocalValue();
    if (pLocalValue == DependencyProperty::get_UnsetValue())
    {
        entry.set_BaseValueSource(BaseValueSource::Unknown);
        entry.ResetExpressionValue();
        entry.set_Value(NULL);
    }
}

void DependencyObject::InsertEntry(const EffectiveValueEntry& entry, unsigned int entryIndex)
{
    if (m_effectiveValuesCapacity == m_effectiveValuesCount)	
    {
        unsigned short oldCapacity = m_effectiveValuesCapacity;
        unsigned short newCapacity = m_effectiveValuesCapacity * 2;

        if (oldCapacity == newCapacity)
        {
            newCapacity+= 1;
        }

        EffectiveValueEntry* rgNewArray = (EffectiveValueEntry*) Object::AllocateMemory(newCapacity * sizeof(EffectiveValueEntry));

        for (int i = 0; i < m_effectiveValuesCount; i++)
        {
            new (rgNewArray + i) EffectiveValueEntry(m_rgEffectiveValues[i]);
            m_rgEffectiveValues[i].~EffectiveValueEntry();
        }

        for(unsigned int i = m_effectiveValuesCount; i < newCapacity; i++)
        {
            new (rgNewArray + i) EffectiveValueEntry();
        }

        if (m_rgEffectiveValues != NULL)
        {
            Object::FreeMemory(m_rgEffectiveValues);
        }

        m_rgEffectiveValues = rgNewArray;
        m_effectiveValuesCapacity = newCapacity;
    }

    for (int i = m_effectiveValuesCount - 1; i >= (int) entryIndex; i--)
    {
        m_rgEffectiveValues[i + 1] = m_rgEffectiveValues[i];
    }

    m_rgEffectiveValues[entryIndex] = entry;
    m_effectiveValuesCount+= 1;
}



void DependencyObject::SetAnimatedValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, Object* pValue, Object* pBaseValue)
{
    if (pBaseValue == DependencyProperty::get_UnsetValue())
    {
        pBaseValue = pMetadata->GetDefaultValue(this, dp);
        if (!entryIndex.Found || m_rgEffectiveValues[entryIndex.Index].get_BaseValueSource() == BaseValueSource::Unknown)
        {
            SetEffectiveValue(entryIndex, &dp, dp.get_PropertyID(), pMetadata, pBaseValue, BaseValueSource::Default);
            entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
        }
    }
    else if ((!entryIndex.Found && !get_IsSelfInheritanceParent()) && pMetadata->get_IsInherited())
    {
        SetIsSelfInheritanceParent();
        entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
    }

    EffectiveValueEntry& entry = m_rgEffectiveValues[entryIndex.Index];
    entry.SetAnimatedValue(pValue, pBaseValue);
    entry.ResetCoercedValue();
}

EffectiveValueEntry DependencyObject::SetCoercedValue(EntryIndex entryIndex, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, Object* pValue, Object* pBaseValue)
{
    Object* pDefaultValue = pMetadata->GetDefaultValue(this, dp);
    if (Equals(dp, pBaseValue, pDefaultValue))
    {
        if (!entryIndex.Found || (m_rgEffectiveValues[entryIndex.Index].get_BaseValueSource() == BaseValueSource::Unknown))
        {
            SetEffectiveValue(entryIndex, &dp, dp.get_PropertyID(), pMetadata, pDefaultValue, BaseValueSource::Default);
            entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
        }
    }
    else if (!entryIndex.Found && !get_IsSelfInheritanceParent() && pMetadata->get_IsInherited())
    {
        SetIsSelfInheritanceParent();
        entryIndex = CheckEntryIndex(entryIndex, dp.get_PropertyID());
    }
    EffectiveValueEntry& entry = m_rgEffectiveValues[entryIndex.Index];
    entry.SetCoercedValue(pValue, pBaseValue);

    return entry;
}


void DependencyObject::SetExpressionValue(EntryIndex entryIndex, Object* pValue, Object* pBaseValue)
{
    EffectiveValueEntry& entry = m_rgEffectiveValues[entryIndex.Index];
    if (pBaseValue != EffectiveValueEntry::get_ExpressionInAlternativeStore())
    {
        entry.set_IsDeferredReference(pValue != NULL && pValue->IsType(GET_TYPE(DeferredReference)));
    }
    else
    {
        entry.set_IsDeferredReference(false);
    }
    entry.SetExpressionValue(pValue, pBaseValue);
    entry.ResetAnimatedValue();
    entry.ResetCoercedValue();
}

 

void DependencyObject::UpdateSourceDependentLists(DependencyObject* pDO, const DependencyProperty& dp, ItemStructList<TRefCountedPtr<DependencySource> >* pSources,
                                                  Expression* pExpression, bool isAdd)
{
    if(pSources == NULL)
    {
        return;
    }

    for(int idx = 0; idx < pSources->get_Count(); idx++)
    {
        DependencyObject* pSourceObject = (*pSources)[idx]->get_DependencyObject();
        const DependencyProperty* pSourceProperty = (*pSources)[idx]->get_DependencyProperty();

        if(pSourceObject == NULL || pSourceProperty == NULL)
        {
            // No dependencies, source / target are null. Must be reupdated in the case where source or 
            // target changes

            continue;
        }

        ItemStructMap<DependentListPtr>* pMap = DependentListMapField.GetValue(pSourceObject);
        if(pMap == NULL)
        {
            TRefCountedPtr<ItemStructMap<DependentListPtr> > pNewMap(new ItemStructMap<DependentListPtr>(), false);

            DependentListMapField.SetValue(pSourceObject, pNewMap);

            pMap = pNewMap;
        }

        DependentListPtr pDependentList = NULL;

        if(pMap->Contains(pSourceProperty->get_PropertyID()))
        {
            pDependentList = (*pMap)[pMap->Search(pSourceProperty->get_PropertyID())];
        }

        if (isAdd)
        {
            if (pDependentList == NULL)
            {
                pDependentList = DependentListPtr(new DependentList(), false);

                (*pMap)[pMap->EnsureEntry(pSourceProperty->get_PropertyID())] = pDependentList;
            }

            pDependentList->Add(pDO, dp, pExpression);
        }
        else if (pDependentList != NULL)
        {
            pDependentList->Remove(pDO, dp, pExpression);
            if(pDependentList->get_IsEmpty())
            {
                pMap->RemoveEntry(pSourceProperty->get_PropertyID());
            }
        }
    }
}

 
bool DependencyObject::ProvideSelfAsInheritanceContext(Object* pValue, const DependencyProperty* pDP)
{
    if(pValue != NULL && pValue->IsType(GET_TYPE(Animatable)))
    {
        DependencyObject* pDOValue = (DependencyObject*) pValue;
        Object* pOldInheritanceContext = pDOValue->get_InheritanceContext();

        pDOValue->AddInheritanceContext(this, pDP);
        if(this == pDOValue->get_InheritanceContext())
        {
            return this != pOldInheritanceContext;
        }
    }

    return false;
}


bool DependencyObject::RemoveSelfAsInheritanceContext(Object* pValue, const DependencyProperty* pDP)
{
    if(pValue != NULL && pValue->IsType(GET_TYPE(Animatable)))
    {
        DependencyObject* pDOValue = (DependencyObject*) pValue;
        Object* pOldInheritanceContext = pDOValue->get_InheritanceContext();

        pDOValue->RemoveInheritanceContext(this, pDP);
        if(this == pOldInheritanceContext)
        {
            return pDOValue->get_InheritanceContext() != pOldInheritanceContext;
        }
    }

    return false;
}


DependencyPropertyMetadata* DependencyObject::SetupPropertyChange(const DependencyProperty& dp)
{
    if (dp.get_IsReadOnly())
    {
        LibraryCriticalError();
    }
    return dp.GetMetadata(get_Type());
}

 
bool DependencyObject::Equals(const DependencyProperty& dp, Object* pObject1, Object* pObject2)
{
    return Object::Equals(pObject1, pObject2);
}



void DependencyObject::SetIsSelfInheritanceParent()
{
    DependencyObject* pInheritanceParent = get_InheritanceParent();
    if (pInheritanceParent != NULL)
    {
        MergeInheritableProperties(pInheritanceParent);
        SetInheritanceParent(NULL);
    }
    m_isSelfInheritanceParent = true;
}

void DependencyObject::SetInheritanceParent(DependencyObject* pNewParent)
{
    if (m_pContextStorage != NULL)
    {
        m_pContextStorage = pNewParent;
    }
    else if (pNewParent != NULL)
    {
        if (get_IsSelfInheritanceParent())
        {
            MergeInheritableProperties(pNewParent);
        }
        else
        {
            m_pContextStorage = pNewParent;
        }
    }
}

void DependencyObject::MergeInheritableProperties(DependencyObject* pInheritanceParent)
{
    EffectiveValueEntry* rgEffectiveValues = pInheritanceParent->m_rgEffectiveValues;
    unsigned int effectiveValuesCount = pInheritanceParent->m_effectiveValuesCount;
    for (unsigned int i = 0; i < effectiveValuesCount; i++)
    {
        EffectiveValueEntry& entry = rgEffectiveValues[i];

        const DependencyProperty* pDP = TypeManager::GetDependencyPropertyByID(entry.get_PropertyIndex());

        if (pDP != NULL)
        {
            DependencyPropertyMetadata* pMetadata = pDP->GetMetadata(get_Type());
            if (pMetadata->get_IsInherited())
            {
                ObjectPtr pObj2 = pInheritanceParent->GetValueEntry(EntryIndex(i), *pDP, pMetadata, (RequestFlags::Enum)(RequestFlags::SkipDefault | RequestFlags::DeferredReferences)).get_Value();
                if (pObj2 != DependencyProperty::get_UnsetValue())
                {
                    EntryIndex entryIndex = LookupEntry(pDP->get_PropertyID());
                    SetEffectiveValue(entryIndex, pDP, pDP->get_PropertyID(), pMetadata, pObj2, BaseValueSource::Inherited);
                }
            }
        }
    }
}
 

void DependencyObject::ChangeExpressionSources(Expression* pExpr, DependencyObject* pDO, const DependencyProperty* pDP, ItemStructList<TRefCountedPtr<DependencySource> >* pNewSources)
{
    if (!pExpr->get_ForwardsInvalidations())
    {
        EntryIndex entry = pDO->LookupEntry(pDP->get_PropertyID());
        if (!entry.Found || (pDO->m_rgEffectiveValues[entry.Index].get_LocalValue() != pExpr))
        {
            LibraryCriticalError();
        }
    }

    TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > pSources = pExpr->GetSources();
    if (pSources != NULL)
    {
        UpdateSourceDependentLists(pDO, *pDP, pSources, pExpr, false);
    }
    if (pNewSources != NULL)
    {
        UpdateSourceDependentLists(pDO, *pDP, pNewSources, pExpr, true);
    }
}

 

void DependencyObject::SynchronizeInheritanceParent(DependencyObject* pParent)
{
    if (!get_IsSelfInheritanceParent())
    {
        if (pParent != NULL)
        {
            if (!pParent->get_IsSelfInheritanceParent())
            {
                SetInheritanceParent(pParent->get_InheritanceParent());
            }
            else
            {
                SetInheritanceParent(pParent);
            }
        }
        else
        {
            SetInheritanceParent(NULL);
        }
    }
}

Dispatcher* DependencyObject::get_Dispatcher()
{
    return Dispatcher::get_CurrentDispatcher();
}

}; // namespace AVUI
