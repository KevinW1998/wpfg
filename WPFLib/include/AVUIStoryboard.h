#pragma once

#include <AVUIParallelTimeline.h>
#include <AVUIUncommonField.h>
#include <AVUIItemStructList.h>
#include <AVUIItemStructMap.h>

namespace AVUI {

class String;
class PropertyPath;
class UIElement;
class DependencyObject;
class Clock;
class AnimationClock;
class ControlTemplate;
class MediaClock;

class Storyboard : public ParallelTimeline
{
    friend class ChangeListener;
    struct CloneCacheEntry;
public:
    DECLARE_ELEMENT(Storyboard, ParallelTimeline);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(TargetName, String, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(TargetProperty, PropertyPath, NULL, PropertyFlag::None);

    Storyboard() { };

    struct Layers
    {
        static const int Code = 1;
        static const int ElementEventTrigger = 1;
        static const int PropertyTriggerStartLayer = 2;
    };

    struct ObjectPropertyPair
    {
        ObjectPropertyPair() : pProperty(NULL) { }
        ObjectPropertyPair(DependencyObject* pDO, const DependencyProperty* pProperty) : pDO(pDO), pProperty(pProperty) { }
        DependencyObjectPtr pDO;
        const DependencyProperty* pProperty;
    };

    struct ClockMapping
    {
        ObjectPropertyPair m_key;
        ObjectPtr m_pData;
    };

    typedef ItemStructList<ClockMapping> ClockMappingList;

    void Begin(DependencyObject* pContainingObject, ControlTemplate* pTemplate, HandoffBehavior::Enum handoffBehavior = HandoffBehavior::SnapshotAndReplace, bool isControllable = false);

    void BeginCommon(DependencyObject* pContainingObject, ControlTemplate* pTemplate, HandoffBehavior::Enum handoffBehavior, bool isControllable, int layer);


    static void GetComplexPathValue(DependencyObject* pTargetObject, const DependencyProperty& targetProperty, EffectiveValueEntry* pEntry, DependencyPropertyMetadata* pMetadata);

private:

    static void CloneEffectiveValue(EffectiveValueEntry* pEntry, CloneCacheEntry* pCacheEntry);

    class ChangeListener : public Object
    {
    public:
        ChangeListener(DependencyObject* pTarget, Animatable* pClone, const DependencyProperty& property, Animatable* pOriginal);

    private:
        void InvalidatePropertyOnCloneChange(Object* pSender, EventArgs* pEventArgs);
        void InvalidatePropertyOnOriginalChange(Object* pSender, EventArgs* pEventArgs);
        void Setup();
        void Cleanup();

        Animatable* m_pOriginal;
        Animatable* m_pClone;
        const DependencyProperty* m_pProperty;
        DependencyObject* m_pTarget;
    };

    struct CloneCacheEntry 
    {
    public:
        CloneCacheEntry() { };
        CloneCacheEntry(Object* pSource, Object* pClone) : pSource(pSource), pClone(pClone) { };
        ObjectPtr pClone;
        ObjectPtr pSource;
        TRefCountedPtr<ChangeListener> pChangeListener;
    };

    typedef ItemStructList<TRefCountedPtr<AnimationClock> > AnimationClockList;


    void ClockTreeWalkRecursive(Clock* pClock, DependencyObject* pContainingObject, ControlTemplate* pTemplate, String* pParentObjectName, PropertyPath* pParentPropertyPath, HandoffBehavior::Enum, ClockMappingList* pMappings, int layer);
    void ApplyAnimationClocks(ClockMappingList* pMappings, HandoffBehavior::Enum handoffBehavior, int layer);
    void SetStoryboardClock(DependencyObject* pContainingObject, Clock* pClock);
    static DependencyObject* ResolveTargetName(String* pTargetName, ControlTemplate* pTemplate, DependencyObject* pContainingElement);
    static void VerifyAnimationIsValid(const DependencyProperty* pTargetProperty, AnimationClock* pAnimationClock);
    static void VerifyPathIsAnimatable(PropertyPath* pPath);
    static void VerifyComplexPathSupport(Object* pObject);
    static void UpdateMappings(ClockMappingList* pMappings, const ObjectPropertyPair& mappingKey, AnimationClock* pAnimationClock);
    static ClockMapping* FindMapping(ClockMappingList* pMappings, const ObjectPropertyPair& mappingKey);
    static CloneCacheEntry* GetComplexPathClone(DependencyObject* pDO, const DependencyProperty& dp);
    void ProcessComplexPath(ClockMappingList* pMappings, DependencyObject* pTargetObject, PropertyPath* pPath, AnimationClock* pAnimationClock, HandoffBehavior::Enum handoffBehavior, long layer);
    void ListenToChangesOnAnimatable(DependencyObject* pTargetObject, Animatable* pClone, const DependencyProperty& property, Animatable* pCurrentValue);
    static bool IsPropertyCloningRequired(DependencyObject* pTargetObject, Object* pCurrentValue, const DependencyProperty& dp);
    static void SetComplexPathClone(DependencyObject* pDO, const DependencyProperty& dp, Object* pSource, Object* pClone);
    static void ApplyMediaClock(ControlTemplate* pControlTemplate, DependencyObject* pContainingObject, String* pTargetName, MediaClock* pClock);

    struct StoryboardClockTreeItem
    {
        StoryboardClockTreeItem() : pStoryboard(NULL) { }
        StoryboardClockTreeItem(Storyboard* pStoryboard, Clock* pClock) : pStoryboard(pStoryboard), pClock(pClock) { }
        Storyboard* pStoryboard;
        TWeakPtr<Clock> pClock;
    };
    typedef ItemStructList<StoryboardClockTreeItem> StoryboardClockTreeItemList;

    static UncommonField<StoryboardClockTreeItemList> StoryboardClockTreesField;  
    static UncommonField<ItemStructMap<CloneCacheEntry> > ComplexPathCloneField;  
};


}; // namespace AVUI
