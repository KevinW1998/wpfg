#include <AVUICommon.h>
#include <AVUIClockGroup.h>
#include <AVUIAnimationClock.h>
#include <AVUIAnimationStorage.h>
#include <AVUIDependencyObject.h>
#include <AVUIDeferredReference.h>

REGISTER_ELEMENT_BEGIN(AVUI::AnimationStorage)

REGISTER_ELEMENT_END()


namespace AVUI {

UncommonField<ItemStructMap<ObjectPtr> > AnimationStorage::AnimatedPropertyMapField;

AnimationStorage::AnimationStorage() 
{
    m_pSnapshotValue = DependencyProperty::get_UnsetValue();
    m_pBaseValue = DependencyProperty::get_UnsetValue();
}

AnimationStorage::~AnimationStorage()
{
    ClearAnimations();
}

void AnimationStorage::EndAnimation(DependencyObject* pDO, const DependencyProperty& dp)
{
    TRefCountedPtr<AnimationStorage> pStorage = AnimationStorage::GetStorage(pDO, dp);

    if(pStorage != NULL)
    {
        pStorage->ClearAnimations();
    }
}

void AnimationStorage::BeginAnimation(DependencyObject* pDO, const DependencyProperty& dp, AnimationTimeline* pAnimation, HandoffBehavior::Enum handoffBehavior)
{
    TRefCountedPtr<AnimationStorage> pStorage = AnimationStorage::GetStorage(pDO, dp);
    if (pAnimation == NULL)
    {
        if (pStorage == NULL || handoffBehavior == HandoffBehavior::Compose)
        {
            return;
        }

        pStorage->m_pSnapshotValue = DependencyProperty::get_UnsetValue();
        pStorage->ClearAnimations();
    }
    else
    {
        if (pAnimation->get_BeginTime().get_HasValue())
        {
            AnimationClockList clockList;

            clockList.AddItem(pAnimation->CreateClock());

            ApplyAnimationClocks(pDO, dp, clockList, handoffBehavior);
            return;
        }
        if (pStorage == NULL)
        {
            return;
        }
        if (handoffBehavior == HandoffBehavior::SnapshotAndReplace)
        {
            pStorage->m_pSnapshotValue = pDO->GetValue(dp);
            pStorage->ClearAnimations();
        }
    }
    pStorage->WritePostscript();

}

AnimationStorage* AnimationStorage::GetStorage(DependencyObject* pDO, const DependencyProperty& dp)
{
    ItemStructMap<ObjectPtr>* pMap = AnimatedPropertyMapField.GetValue(pDO);

    if(pMap == NULL)
    {
        TRefCountedPtr<ItemStructMap<ObjectPtr> > pNewMap(new ItemStructMap<ObjectPtr>(), false);

        AnimatedPropertyMapField.SetValue(pDO, pNewMap);

        pMap = pNewMap;
    }

    int index = pMap->Search(dp.get_PropertyID());

    if (index < 0)
    {
        return NULL;
    }

    return object_cast<AnimationStorage>((*pMap)[index]);
}

void AnimationStorage::ApplyAnimationClocksToLayer(DependencyObject* pDO, const DependencyProperty& dp, AnimationClockList& animationClocks, HandoffBehavior::Enum handoffBehavior, int layer)
{
    if(layer != 1)
    {
        // Layers NYI
        LibraryCriticalError();
    }

    ApplyAnimationClocks(pDO, dp, animationClocks, handoffBehavior);
}

void AnimationStorage::ApplyAnimationClocks(DependencyObject* pDO, const DependencyProperty& dp, AnimationClockList& animationClocks, HandoffBehavior::Enum handoffBehavior)
{
    TRefCountedPtr<AnimationStorage> pStorage = GetStorage(pDO, dp);
    if (handoffBehavior == HandoffBehavior::SnapshotAndReplace || pStorage == NULL)
    {
        if (pStorage != NULL)
        {
            pStorage->m_pSnapshotValue = pDO->GetValue(dp);
            pStorage->ClearAnimations();
        }
        else
        {
            pStorage = CreateStorage(pDO, dp);
        }
    }

    for (int num2 = 0; num2 < animationClocks.get_Count(); num2++)
    {
        pStorage->m_animationClocks.AddItem( animationClocks[num2]);
        pStorage->AttachAnimationClock( animationClocks[num2]);
    }

    pStorage->WritePostscript();
}

TRefCountedPtr<AnimationStorage> AnimationStorage::CreateStorage(DependencyObject* pDO, const DependencyProperty& dp)
{
    TRefCountedPtr<AnimationStorage> pStorage(new AnimationStorage(), false);

    pStorage->Initialize(pDO, dp);
    return pStorage;
}


void AnimationStorage::Initialize(DependencyObject* pDO, const DependencyProperty& dp)
{
    m_pDependencyObject = pDO;
    m_pDependencyProperty = &dp;
}


void AnimationStorage::ClearAnimations()
{
    for (int num1 = 0; num1 < m_animationClocks.get_Count(); num1++)
    {
        DetachAnimationClock(m_animationClocks[num1]);
    }
    m_animationClocks.Clear();
}

void AnimationStorage::WritePostscript()
{
    DependencyObject* pDependencyObject = m_pDependencyObject;
    if (pDependencyObject != NULL)
    {
        ItemStructMap<ObjectPtr>* pMap = AnimatedPropertyMapField.GetValue(pDependencyObject);
        if (pMap->get_Count() == 0 || !pMap->Contains(m_pDependencyProperty->get_PropertyID()))
        {
            if (!get_IsEmpty())
            {
                int index = pMap->EnsureEntry(m_pDependencyProperty->get_PropertyID());

                (*pMap)[index] = this;

                AnimatedPropertyMapField.SetValue(pDependencyObject, pMap);
                if (pMap->get_Count() == 1)
                {
                    pDependencyObject->set_HasAnimatedProperties(true);
                }
            }
        }
        else if (get_IsEmpty())
        {
            pMap->RemoveEntry(m_pDependencyProperty->get_PropertyID());

            if (pMap->get_Count() == 0)
            {
                AnimatedPropertyMapField.ClearValue(pDependencyObject);

                pDependencyObject->set_HasAnimatedProperties(false);
            }
            else
            {
                AnimatedPropertyMapField.SetValue(pDependencyObject, pMap);
            }
            if (m_pBaseValue != DependencyProperty::get_UnsetValue())
            {
                pDependencyObject->SetValue(*m_pDependencyProperty, m_pBaseValue);
            }
        }
        pDependencyObject->InvalidateProperty(*m_pDependencyProperty);
    }
}

bool AnimationStorage::get_IsEmpty() const
{
    if (m_animationClocks.get_Count() == 0)
    {
        return m_pSnapshotValue == DependencyProperty::get_UnsetValue();
    }
    return false;
}

ObjectPtr AnimationStorage::GetCurrentPropertyValue(AnimationStorage* pStorage, DependencyObject* pDO, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, Object* pBaseValue)
{
    if (pStorage->m_animationClocks.get_Count() == 0) // &&  (storage._propertyTriggerLayers == null))
    {
        return pStorage->m_pSnapshotValue;
    }
    ObjectPtr pAnimatedValue = pBaseValue;
    if (pAnimatedValue == DependencyProperty::get_UnsetValue())
    {
        pAnimatedValue = pMetadata->get_DefaultValue();
    }

    /*

    if (storage._propertyTriggerLayers != null)
    {
          int num4 = storage._propertyTriggerLayers.Count;
          IList<AnimationLayer> list2 = storage._propertyTriggerLayers.Values;
          for (int num2 = 0; num2 < num4; num2++)
          {
                obj1 = list2[num2].get_CurrentValue(obj1);
          }
    }
    */


    if (pStorage->m_animationClocks.get_Count() > 0)
    {
        ItemStructList<AnimationClockPtr>& animationClocks = pStorage->m_animationClocks;

        bool hasAnimatedValue = false;
        ObjectPtr pDestination = pAnimatedValue;
        ObjectPtr pOrigin = pAnimatedValue;
        if (pStorage->m_pSnapshotValue != DependencyProperty::get_UnsetValue())
        {
            pOrigin = pStorage->m_pSnapshotValue;
        }
        for (int i = 0; i < animationClocks.get_Count(); i++)
        {
            AnimationClock* pClock = animationClocks[i];

            if (pClock->get_CurrentState() != ClockState::Stopped)
            {
                hasAnimatedValue = true;
                pOrigin = pClock->GetCurrentValue(pOrigin, pDestination);
                if (pOrigin == DependencyProperty::get_UnsetValue())
                {
                    LibraryCriticalErrorMessage("Origin value must be valid.");
                }
            }
        }
        if (hasAnimatedValue)
        {
            pAnimatedValue = pOrigin;
        }
    }

    if (!dp.IsValidValue(pAnimatedValue))
    {
        LibraryCriticalErrorMessage("Calculated animation value is invalid.");
    }

    return pAnimatedValue;
}

void AnimationStorage::AttachAnimationClock(AnimationClock* pAnimationClock)
{
    pAnimationClock->add_CurrentTimeInvalidated(EventHandler::Create(this, &AnimationStorage::OnCurrentTimeInvalidated));
}

void AnimationStorage::DetachAnimationClock(AnimationClock* pAnimationClock)
{
    pAnimationClock->remove_CurrentTimeInvalidated(EventHandler::Create(this, &AnimationStorage::OnCurrentTimeInvalidated));
}

void AnimationStorage::OnCurrentTimeInvalidated(Object* pSender, EventArgs* pEventArgs)
{
    if (m_pDependencyObject == NULL)
    {
        DetachAnimationClock((AnimationClock*) pSender);
    }
    else
    {
        ObjectPtr pBaseValue;
        EffectiveValueEntry newEntry;
        EffectiveValueEntry oldEntry = m_pDependencyObject->GetValueEntry(m_pDependencyObject->LookupEntry(m_pDependencyProperty->get_PropertyID()), *m_pDependencyProperty, NULL, RequestFlags::RawEntry);
        if (!oldEntry.get_HasModifiers())
        {
            newEntry = oldEntry;
            pBaseValue = newEntry.get_Value();
            if (newEntry.get_IsDeferredReference())
            {
                pBaseValue = ((DeferredReference*) (Object*) pBaseValue)->GetValue(newEntry.get_BaseValueSource());
                newEntry.set_Value(pBaseValue);
                newEntry.set_IsDeferredReference(false);
            }
        }
        else
        {
            newEntry.set_BaseValueSource(oldEntry.get_BaseValueSource());
            newEntry.set_PropertyIndex(oldEntry.get_PropertyIndex());
            newEntry.set_HasExpressionMarker(oldEntry.get_HasExpressionMarker());
            pBaseValue = oldEntry.get_ModifiedValue()->BaseValue;
            if (oldEntry.get_IsDeferredReference())
            {
                DeferredReference* pReference2 = object_cast<DeferredReference>(pBaseValue);
                if (pReference2 != NULL)
                {
                    pBaseValue = pReference2->GetValue(newEntry.get_BaseValueSource());
                }
            }
            newEntry.set_Value(pBaseValue);
            if (oldEntry.get_IsExpression())
            {
                pBaseValue = oldEntry.get_ModifiedValue()->ExpressionValue;
                if (oldEntry.get_IsDeferredReference())
                {
                    DeferredReference* pReference = object_cast<DeferredReference>(pBaseValue);
                    if (pReference != NULL)
                    {
                        pBaseValue = pReference->GetValue(newEntry.get_BaseValueSource());
                    }
                }
                newEntry.SetExpressionValue(pBaseValue, newEntry.get_Value());
            }
        }
        DependencyPropertyMetadata* pMetadata = m_pDependencyProperty->GetMetadata(m_pDependencyObject->get_Type());
        ObjectPtr pNewValue = GetCurrentPropertyValue(this, m_pDependencyObject, *m_pDependencyProperty, pMetadata, pBaseValue);
        newEntry.SetAnimatedValue(pNewValue, pBaseValue);
        m_pDependencyObject->UpdateEffectiveValue(m_pDependencyObject->LookupEntry(m_pDependencyProperty->get_PropertyID()), *m_pDependencyProperty, pMetadata, oldEntry, &newEntry, false, OperationType::Unknown);
    }
}


}; // namespace AVUI
