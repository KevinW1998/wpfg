#include <AVUICommon.h>
#include <AVUIAnimatable.h>
#include <AVUIClockGroup.h>
#include <AVUIAnimationStorage.h>
#include <AVUITypeManager.h>
#include <AVUIDeferredReference.h>

REGISTER_ELEMENT_BEGIN(AVUI::Animatable)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {

void Animatable::BeginAnimation(const DependencyProperty& dp, AnimationTimeline* pTimeline, HandoffBehavior::Enum handoffBehavior)
{
    AnimationStorage::BeginAnimation(this, dp, pTimeline, handoffBehavior);
}


AnimatablePtr Animatable::Clone()
{
    AnimatablePtr pValue = CreateInstanceCore();

    pValue->CloneCore(this);

    return pValue;
}

AnimatablePtr Animatable::CreateInstanceCore()
{
    
    ObjectPtr pObject = get_Type().CreateInstance();

    return object_cast<Animatable>(pObject);
}

void Animatable::CloneCore(Animatable* pSource)
{
    CloneCoreCommon(pSource, false);
}

void Animatable::CloneCurrentValueCore(Animatable* pSource)
{
    CloneCoreCommon(pSource, true);
}

void Animatable::CloneCoreCommon(Animatable* pSource, bool useCurrentValue)
{
    int effectiveValuesCount = pSource->m_effectiveValuesCount;
    EffectiveValueEntry* rgEffectiveValues = pSource->m_rgEffectiveValues;

    for(int i = 0; i < effectiveValuesCount; i++)
    {
        const EffectiveValueEntry& entry = rgEffectiveValues[i];
        const DependencyProperty* pProperty = TypeManager::GetDependencyPropertyByID(entry.get_PropertyIndex());

        if(pProperty == NULL)
        {
            continue;
        }
        if(pProperty->get_Flags() & PropertyFlag::ReadOnly)
        {
            continue;
        }

        ObjectPtr pValue;

        EntryIndex entryIndex(i);
        if(useCurrentValue)
        {
            pValue = pSource->GetValueEntry(entryIndex, *pProperty, NULL, RequestFlags::FullyResolved).get_Value();
        }
        else
        {
            pValue = pSource->ReadLocalValueEntry(entryIndex, *pProperty, true);
            if(pValue == DependencyProperty::get_UnsetValue())
            {
                continue;
            }
            if(entry.get_IsExpression())
            {
                pValue = ((Expression*) (Object*)pValue)->Copy(this, *pProperty);
            }
        }

        Animatable* pAnimatable = object_cast<Animatable>(pValue);
        if(pAnimatable != NULL)
        {                                                           
            AnimatablePtr pAnimatableClone = pAnimatable->CreateInstanceCore();
            if(useCurrentValue)
            {
                pAnimatableClone->CloneCurrentValueCore(pAnimatable);
            }
            else
            {
                pAnimatableClone->CloneCore(pAnimatable);
            }
            pValue = pAnimatableClone;
        }

        SetValue(*pProperty, pValue);
    }
}

DependencyObject* Animatable::get_InheritanceContext()
{
    PruneInheritanceContexts();

    if(m_inheritanceContexts.get_Count() > 0)
    {
        return m_inheritanceContexts[0].Owner;
    }
    return NULL;
}

void Animatable::AddInheritanceContext(DependencyObject* pDO, const DependencyProperty* pDP)
{
    PruneInheritanceContexts();

    if(pDO == NULL)
    {
        return;
    }

    for(int idx = 0; idx < m_inheritanceContexts.get_Count(); idx++)
    {
        if(m_inheritanceContexts[idx].Owner == pDO && m_inheritanceContexts[idx].pProperty == pDP)
        {
            return;
        }
    }

    AnimatableContextPair ctxPair;

    ctxPair.Owner = pDO;
    ctxPair.pProperty = pDP;

    m_inheritanceContexts.AddItem(ctxPair);
}

void Animatable::RemoveInheritanceContext(DependencyObject* pDO, const DependencyProperty* pDP)
{
    PruneInheritanceContexts();

    for(int idx = 0; idx < m_inheritanceContexts.get_Count(); idx++)
    {
        AnimatableContextPair& ctxPair = m_inheritanceContexts[idx];

        if(ctxPair.Owner == pDO && ctxPair.pProperty == pDP)
        {
            m_inheritanceContexts.RemoveAt(idx);
            return;
        }
    }
}

bool Animatable::get_HasMultipleInheritanceContexts()
{
    PruneInheritanceContexts();

    return m_inheritanceContexts.get_Count() > 1;
}

void Animatable::PruneInheritanceContexts()
{
    for(int idx = 0; idx < m_inheritanceContexts.get_Count(); idx++)
    {
        if(m_inheritanceContexts[idx].Owner == NULL)
        {
            m_inheritanceContexts.RemoveAt(idx);
            idx--;
        }
    }
}

void Animatable::OnAnimatablePropertyChanged(DependencyObject* pOldValue, DependencyObject* pNewValue)
{
    if (pOldValue != NULL)
    {
        RemoveSelfAsInheritanceContext(pOldValue, NULL);
    }
    if (pNewValue != NULL)
    {
        ProvideSelfAsInheritanceContext(pNewValue, NULL);
    }
}

void Animatable::OnPropertyChanged(const DependencyPropertyChangedEventArgs& e) 
{
     DependencyObject::OnPropertyChanged(e); 

     if(!e.IsSubPropertyChange) 
     {
         WritePostscript();
     }
}

void Animatable::WritePostscript()
{
    FireChanged();
}

void Animatable::FireChanged()
{
    InvalidateSubProperties();
    invoke_Changed(this, NULL); 
}


void Animatable::InvalidateSubProperties()
{
    for(int idx = 0; idx < m_inheritanceContexts.get_Count(); idx++)
    {
        AnimatableContextPair& ctxPair = m_inheritanceContexts[idx];

        if(ctxPair.Owner != NULL)
        {
            if(ctxPair.Owner->IsType(GET_TYPE(Animatable)))
            {
                Animatable* pAnimatableValue = (Animatable*) (Object*)(ctxPair.Owner);

                pAnimatableValue->InvalidateSubProperties();
            }

            if(ctxPair.pProperty != NULL)
            {
                ctxPair.Owner->InvalidateSubProperty(*ctxPair.pProperty);
            }
        }
    }
}

void Animatable::EvaluateAnimatedValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pEntry)
{
    if (get_HasAnimatedProperties())
    {
        AnimationStorage* pStorage = AnimationStorage::GetStorage(this, dp);

        if (pStorage != NULL)
        {
            ObjectPtr pAnimationBaseValue = pEntry->GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
            if (pEntry->get_IsDeferredReference())
            {
                pAnimationBaseValue = ((DeferredReference*) (Object*) pAnimationBaseValue)->GetValue(pEntry->get_BaseValueSource());
                pEntry->SetAnimationBaseValue(pAnimationBaseValue);
                pEntry->set_IsDeferredReference(false);
            }
            ObjectPtr pAnimatedValue = AnimationStorage::GetCurrentPropertyValue(pStorage, this, dp, pMetadata, pAnimationBaseValue);
            pEntry->SetAnimatedValue(pAnimatedValue, pAnimationBaseValue);
        }
    }
}


}; // namespace AVUI
