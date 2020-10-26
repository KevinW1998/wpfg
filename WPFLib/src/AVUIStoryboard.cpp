#include <AVUICommon.h>
#include <AVUIStoryboard.h>
#include <AVUIMediaClock.h>
#include <AVUIAnimationClock.h>
#include <AVUIClockGroup.h>
#include <AVUIUIElement.h>
#include <AVUIPropertyPath.h>
#include <AVUIAnimationStorage.h>
#include <AVUIControlTemplate.h>
#include <AVUIDeferredReference.h>
#include <AVUIGeometryModel3D.h>
#include <AVUIMediaElement.h>
#include <AVUIMediaTimeline.h>

REGISTER_ELEMENT_BEGIN(AVUI::Storyboard)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(TargetName);
    REGISTER_PROPERTY(TargetProperty);
REGISTER_ELEMENT_END()

namespace AVUI {

UncommonField<Storyboard::StoryboardClockTreeItemList> Storyboard::StoryboardClockTreesField;  
UncommonField<ItemStructMap<Storyboard::CloneCacheEntry> > Storyboard::ComplexPathCloneField;  

void Storyboard::Begin(DependencyObject* pContainingObject, ControlTemplate* pTemplate, HandoffBehavior::Enum handoffBehavior, bool isControllable)
{
    BeginCommon(pContainingObject, pTemplate, handoffBehavior, isControllable, Layers::Code);
}

void Storyboard::BeginCommon(DependencyObject* pContainingObject, ControlTemplate* pTemplate, HandoffBehavior::Enum handoffBehavior, bool isControllable, int layer)
{
    if(pContainingObject == NULL)
    {
        LibraryCriticalError();
    }

    if (get_BeginTime().get_HasValue())
    {
        TRefCountedPtr<ClockMappingList> pMappings(new ClockMappingList(), false);
        ClockPtr pClock = Timeline::CreateClock(isControllable);

        ClockTreeWalkRecursive(pClock, pContainingObject, pTemplate, NULL, NULL, handoffBehavior, pMappings, layer);
        ApplyAnimationClocks(pMappings, handoffBehavior, layer);

        if (isControllable)
        {
            SetStoryboardClock(pContainingObject, pClock);
        }
    }    
}


void Storyboard::ClockTreeWalkRecursive(Clock* pClock, DependencyObject* pContainingObject, ControlTemplate* pControlTemplate, String* pParentObjectName, PropertyPath* pParentPropertyPath, HandoffBehavior::Enum handoffBehavior, ClockMappingList* pMappings, int layer)
{
    Timeline* pTimeline = pClock->get_Timeline();
    String *pTargetName = pParentObjectName;
    PropertyPath* pPropertyPath = pParentPropertyPath;
    String* pTimelineTargetName = (String *) (Object*) pTimeline->GetValue(Storyboard::get_TargetNameProperty());
    if (pTimelineTargetName != NULL)
    {
        pTargetName = pTimelineTargetName;
    }
    PropertyPath* pTimelinePropertyPath = (PropertyPath*) (Object*) pTimeline->GetValue(Storyboard::get_TargetPropertyProperty());

    if (pTimelinePropertyPath != NULL)
    {
        pPropertyPath = pTimelinePropertyPath;
    }

    if (pClock->IsType(GET_TYPE(AnimationClock)))
    {
        DependencyObject* pTarget = NULL;
        const DependencyProperty* pProperty = NULL;
        AnimationClock* pAnimationClock = object_cast<AnimationClock>(pClock);
        if (pTargetName != NULL)
        {
            pTarget = ResolveTargetName(pTargetName, pControlTemplate, pContainingObject);
        }
        else
        {
            pTarget = pContainingObject;
        }
        if (pPropertyPath == NULL)
        {
            // Target path required
            LibraryCriticalError();
        }

        TRefCountedPtr<Object> pCtx = pPropertyPath->SetContext(pTarget);
        if (pPropertyPath->get_Length() < 1)
        {
            // Property path required
            LibraryCriticalError();
        }

        VerifyPathIsAnimatable(pPropertyPath);
        if (pPropertyPath->get_Length() == 1)
        {
            pProperty = (const DependencyProperty*) pPropertyPath->GetAccessor(0);

            if(pProperty == NULL)
            {
                LibraryCriticalError();
            }

            VerifyAnimationIsValid(pProperty, pAnimationClock);

            ObjectPropertyPair pair(pTarget, pProperty);

            UpdateMappings(pMappings, pair, pAnimationClock);
        }
        else
        {
            ProcessComplexPath(pMappings, pTarget, pPropertyPath, pAnimationClock, handoffBehavior, layer);
        }
    }
    else if (pClock->IsType(GET_TYPE(MediaClock)))
    {
        ApplyMediaClock(pControlTemplate, pContainingObject, pTargetName, (MediaClock*) pClock);
    }
    else
    {
        ClockGroup* pGroup = object_cast<ClockGroup>(pClock);
        if (pGroup != NULL)
        {
            ItemStructList<ClockPtr>& children = pGroup->get_Children();
            for (int i = 0; i < children.get_Count(); i++)
            {
                ClockTreeWalkRecursive(children[i], pContainingObject, pControlTemplate, pTargetName, pPropertyPath, handoffBehavior, pMappings, layer);
            }
        }
    }


}

void Storyboard::ApplyMediaClock(ControlTemplate* pControlTemplate, DependencyObject* pContainingObject, String* pTargetName, MediaClock* pClock)
{
    MediaElement* pElement = NULL;

    if(pTargetName != NULL)
    {
        pElement = object_cast<MediaElement>(ResolveTargetName(pTargetName, pControlTemplate, pContainingObject));
        if(pElement == NULL)
        {
            LibraryCriticalError();
        }
    }
    else
    {
        pElement = object_cast<MediaElement>(pContainingObject);
    }

    if(pElement == NULL)
    {
        LibraryCriticalError();
    }

    // Completely, utterly wrong. But allows us to demo movie playing
    pElement->set_Source(pClock->get_Timeline()->get_Source());
}


void Storyboard::VerifyAnimationIsValid(const DependencyProperty* pTargetProperty, AnimationClock* pAnimationClock)
{
    // NO IMPLEMENTATION - Verify Later
}

void Storyboard::VerifyPathIsAnimatable(PropertyPath* pPath)
{
    // NO IMPLEMENTATION - Verify Later
}

DependencyObject* Storyboard::ResolveTargetName(String* pTargetName, ControlTemplate* pTemplate, DependencyObject* pContainingElement)
{
    // NO IMPLEMENTATION - TBD later - Namescope?

    UIElement* pUIElement = object_cast<UIElement>(pContainingElement);

    if(pUIElement == NULL)
    {
        LibraryCriticalError();
    }

    DependencyObject* pDO = NULL;
    if(pTemplate != NULL)
    {
        pDO = object_cast<DependencyObject>(pTemplate->FindName(*pTargetName, pUIElement));
    }
    else
    {
        pDO = object_cast<DependencyObject>(pUIElement->FindName(*pTargetName));
    }
    if(pDO == NULL)
    {
        LibraryCriticalError();
    }

    return pDO;
}


void Storyboard::UpdateMappings(ClockMappingList* pMappings, const ObjectPropertyPair& mappingKey, AnimationClock* pAnimationClock)
{
    ClockMapping* pMapping = FindMapping(pMappings, mappingKey);
    if(pMapping == NULL)
    {
        ClockMapping clockMapping;

        clockMapping.m_key = mappingKey;
        clockMapping.m_pData = pAnimationClock;

        pMappings->AddItem(clockMapping);
    }
    else if(pMapping->m_pData->IsType(GET_TYPE(AnimationClock)))
    {
        TRefCountedPtr<AnimationClockList> pClockList(new AnimationClockList(), false);

        pClockList->AddItem(object_cast<AnimationClock>(pMapping->m_pData));
        pClockList->AddItem(pAnimationClock);

        pMapping->m_pData = pClockList;
    }
    else
    {
        AnimationClockList* pClockList = (AnimationClockList*) pMapping->m_pData.get_Ptr();

        pClockList->AddItem(pAnimationClock);
    }

} 

Storyboard::ClockMapping* Storyboard::FindMapping(ClockMappingList* pMappings, const ObjectPropertyPair& mappingKey)
{
    for(int i = 0; i < pMappings->get_Count(); i++)
    {
        ClockMapping& mapping = (*pMappings)[i];

        if(mapping.m_key.pDO == mappingKey.pDO &&
           mapping.m_key.pProperty == mappingKey.pProperty)
        {
            return &mapping;
        }
    }
    return NULL;
}


void Storyboard::ApplyAnimationClocks(ClockMappingList* pMappings, HandoffBehavior::Enum handoffBehavior, int layer)
{
    for(int i = 0; i < pMappings->get_Count(); i++)
    {
        ClockMapping& mapping = (*pMappings)[i];

        if(mapping.m_pData->IsType(GET_TYPE(AnimationClock)))
        {
            AnimationClockList clockList;

            clockList.AddItem(object_cast<AnimationClock>(mapping.m_pData));

            AnimationStorage::ApplyAnimationClocksToLayer(mapping.m_key.pDO, *(mapping.m_key.pProperty), clockList,
                                                   handoffBehavior, layer);
        }
        else
        {
            AnimationStorage::ApplyAnimationClocksToLayer(mapping.m_key.pDO, *(mapping.m_key.pProperty), *((AnimationClockList*)(mapping.m_pData.get_Ptr())),
                                                   handoffBehavior, layer);
        }
    }
}

void Storyboard::SetStoryboardClock(DependencyObject* pContainingObject, Clock* pClock)
{

    StoryboardClockTreeItemList* pClockTree = StoryboardClockTreesField.GetValue(pContainingObject);
    if(pClockTree == NULL)
    {
        TRefCountedPtr<StoryboardClockTreeItemList> pNewClockTree(new StoryboardClockTreeItemList(), false);

        StoryboardClockTreesField.SetValue(pContainingObject, pNewClockTree);

        pClockTree = pNewClockTree;
    }

    StoryboardClockTreeItem* pItem = NULL;

    for(int i = 0; i < pClockTree->get_Count(); pClockTree++)
    {
        StoryboardClockTreeItem& item = (*pClockTree)[i];

        if(item.pStoryboard == this)
        {
            pItem = &item;
        }
    }

    if(pItem == NULL)
    {
        pClockTree->AddItem(StoryboardClockTreeItem(this, pClock));
    }
    else
    {
        pItem->pClock = pClock;
    }
}


void Storyboard::ProcessComplexPath(ClockMappingList* pMappings, DependencyObject* pTargetObject, PropertyPath* pPath, AnimationClock* pAnimationClock, HandoffBehavior::Enum handoffBehavior, long layer)
{
    const DependencyProperty* pProperty = object_cast<DependencyProperty>(pPath->GetAccessor(0));
    if(pProperty == NULL)
    {
        LibraryCriticalError();
    }
    Object* pCurrentValue = pTargetObject->GetValue(*pProperty);
    DependencyObject* pAnimationTarget = object_cast<DependencyObject>(pPath->get_LastItem());
    const DependencyProperty* pPropertyTarget = object_cast<DependencyProperty>(pPath->get_LastAccessor());

    if(pPropertyTarget == NULL)
    {
        LibraryCriticalError();
    }

    VerifyAnimationIsValid(pPropertyTarget, pAnimationClock);
    if(IsPropertyCloningRequired(pTargetObject, pCurrentValue, *pProperty))
    {
        VerifyComplexPathSupport(pTargetObject);
        AnimatablePtr pClone = ((Animatable*) pCurrentValue)->Clone();
        SetComplexPathClone(pTargetObject, *pProperty, pCurrentValue, pClone);

        pTargetObject->InvalidateProperty(*pProperty);
        pTargetObject->GetValue(*pProperty);

        TRefCountedPtr<Object> pCtx = pPath->SetContext(pTargetObject);
       
        pAnimationTarget = object_cast<DependencyObject>(pPath->get_LastItem());
        pPropertyTarget = object_cast<DependencyProperty>(pPath->get_LastAccessor());

        ListenToChangesOnAnimatable(pTargetObject, pClone, *pProperty, (Animatable*) pCurrentValue);
    }

    UpdateMappings(pMappings, ObjectPropertyPair(pAnimationTarget, pPropertyTarget), pAnimationClock);
}

void Storyboard::VerifyComplexPathSupport(Object* pObject)
{
    if(!pObject->IsType(GET_TYPE(UIElement)))
    {
        // Only UI Elements are complex path animatable.
        LibraryCriticalError();
    }
}

bool Storyboard::IsPropertyCloningRequired(DependencyObject* pTargetObject, Object* pCurrentValue, const DependencyProperty& targetProperty)
{
    // Blend support modification -- Don't ever clone in the case of geometry model 3d.
    // This works in WPF due to freezable support
    if(pCurrentValue->IsType(GET_TYPE(Animatable)) && !pTargetObject->IsType(GET_TYPE(GeometryModel3D)))
    {
        CloneCacheEntry* pEntry = GetComplexPathClone(pTargetObject, targetProperty);
        if(pEntry != NULL && pEntry->pClone == pCurrentValue)
        {
            return false;
        }

        return true;
    }
    return false;
}

void Storyboard::GetComplexPathValue(DependencyObject* pTargetObject, const DependencyProperty& targetProperty, EffectiveValueEntry* pEntry, DependencyPropertyMetadata* pMetadata)
{
    CloneCacheEntry* pCacheEntry = GetComplexPathClone(pTargetObject, targetProperty);
    if (pCacheEntry != NULL)
    {
        ObjectPtr pObj2 = pEntry->get_Value();
        if (pObj2 == DependencyProperty::get_UnsetValue() && pCacheEntry->pSource == pMetadata->GetDefaultValue(pTargetObject, targetProperty))
        {
            pEntry->set_BaseValueSource(BaseValueSource::Default);
            pEntry->SetAnimatedValue(pCacheEntry->pClone, DependencyProperty::get_UnsetValue());
        }
        else
        {
            DeferredReference* pReference = object_cast<DeferredReference>(pObj2);
            if (pReference != NULL)
            {
                pObj2 = pReference->GetValue(pEntry->get_BaseValueSource());
                pEntry->set_Value(pObj2);
                pEntry->set_IsDeferredReference(false);
            }
            if (pCacheEntry->pSource == pObj2)
            {
                CloneEffectiveValue(pEntry, pCacheEntry);
            }
            else
            {
                SetComplexPathClone(pTargetObject, targetProperty, DependencyProperty::get_UnsetValue(), DependencyProperty::get_UnsetValue());
            }
        }
    }
}

void Storyboard::CloneEffectiveValue(EffectiveValueEntry* pEntry, CloneCacheEntry* pCacheEntry)
{
    Object* pClone = pCacheEntry->pClone;
    if (!pEntry->get_IsExpression())
    {
        pEntry->set_Value(pClone);
    }
    else
    {
        pEntry->get_ModifiedValue()->ExpressionValue = pClone;
    }
}


Storyboard::CloneCacheEntry* Storyboard::GetComplexPathClone(DependencyObject* pDO, const DependencyProperty& dp)
{
    ItemStructMap<CloneCacheEntry>* pMap = reinterpret_cast<ItemStructMap<CloneCacheEntry>*>(ComplexPathCloneField.GetValue(pDO));

    if(pMap == NULL)
    {
        return NULL;
    }

    int index = pMap->Search(dp.get_PropertyID());
    if(index >= 0)
    {   
        return &((*pMap)[index]);
    }

    return NULL;
}



void Storyboard::SetComplexPathClone(DependencyObject* pDO, const DependencyProperty& dp, Object* pSource, Object* pClone)
{
    ItemStructMap<CloneCacheEntry>* pMap = reinterpret_cast<ItemStructMap<CloneCacheEntry>*>(ComplexPathCloneField.GetValue(pDO));
    if(pMap == NULL)
    {
        TRefCountedPtr<ItemStructMap<CloneCacheEntry> > pEntryList(new ItemStructMap<CloneCacheEntry>, false);
        ComplexPathCloneField.SetValue(pDO, pEntryList);

        pMap = pEntryList;
    }

    if(pClone != DependencyProperty::get_UnsetValue())
    {
        int index = pMap->EnsureEntry(dp.get_PropertyID());

        (*pMap)[index] = CloneCacheEntry(pSource, pClone);
    }
    else
    {
        int index = pMap->Search(dp.get_PropertyID());
        if(index >= 0)
        {
            pMap->RemoveEntry(index);
        }
    }
}

void Storyboard::ListenToChangesOnAnimatable(DependencyObject* pTargetObject, Animatable* pClone, const DependencyProperty& dp, Animatable* pCurrentValue)
{
    CloneCacheEntry* pEntry = GetComplexPathClone(pTargetObject, dp);

    if(pEntry == NULL)
    {
        LibraryCriticalError();
    }

    pEntry->pChangeListener = TRefCountedPtr<ChangeListener>(new ChangeListener(pTargetObject, pClone, dp, pCurrentValue), false);
}



Storyboard::ChangeListener::ChangeListener(DependencyObject* pTarget, Animatable* pClone, const DependencyProperty& property, Animatable* pOriginal) : 
    m_pTarget(pTarget),
    m_pClone(pClone),
    m_pProperty(&property),
    m_pOriginal(pOriginal)
{
    Setup();
}

void Storyboard::ChangeListener::InvalidatePropertyOnCloneChange(Object* pSender, EventArgs* pEventArgs)
{
    Storyboard::CloneCacheEntry* pEntry = Storyboard::GetComplexPathClone(m_pTarget, *m_pProperty);

    if(pEntry != NULL && pEntry->pClone == m_pClone)
    {
        m_pTarget->InvalidateSubProperty(*m_pProperty);
    }
    else
    {
        Cleanup();
    }
}

void Storyboard::ChangeListener::InvalidatePropertyOnOriginalChange(Object* pSender, EventArgs* pEventArgs)
{
    m_pTarget->InvalidateProperty(*m_pProperty);
    Cleanup();
}

void Storyboard::ChangeListener::Setup()
{
    m_pOriginal->add_Changed(EventHandler::Create(this, &ChangeListener::InvalidatePropertyOnOriginalChange));
    m_pClone->add_Changed(EventHandler::Create(this, &ChangeListener::InvalidatePropertyOnCloneChange));
}

void Storyboard::ChangeListener::Cleanup()
{
    if(m_pOriginal != NULL)
    {
        m_pOriginal->remove_Changed(EventHandler::Create(this, &ChangeListener::InvalidatePropertyOnOriginalChange));
    }

    m_pClone->remove_Changed(EventHandler::Create(this, &ChangeListener::InvalidatePropertyOnCloneChange));
    m_pTarget = NULL;
    m_pProperty = NULL;
    m_pClone = NULL;
    m_pOriginal = NULL;
}


}; // namespace AVUI
