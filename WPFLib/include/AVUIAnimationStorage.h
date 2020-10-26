#pragma once

class AnimationTimeline;
class DependencyObject;

#include <AVUIDependencyProperty.h>
#include <AVUIUncommonField.h>
#include <AVUIItemStructMap.h>
#include <AVUIItemStructList.h>
#include <AVUIAnimationClock.h>

namespace AVUI {

class AnimationStorage : public Object
{
public:
    DECLARE_ELEMENT(AnimationStorage, Object);

    static void BeginAnimation(DependencyObject* pDO, const DependencyProperty& dp, AnimationTimeline* pAnimation, HandoffBehavior::Enum handoffBehavior);
    static void EndAnimation(DependencyObject* pDO, const DependencyProperty& dp);
    static AnimationStorage* GetStorage(DependencyObject* pDO, const DependencyProperty& dp);
    static void ApplyAnimationClocks(DependencyObject* pDO, const DependencyProperty& dp, AnimationClockList& animationClocks, HandoffBehavior::Enum handoffBehavior);
    static void ApplyAnimationClocksToLayer(DependencyObject* pDO, const DependencyProperty& dp, AnimationClockList& animationClocks, HandoffBehavior::Enum handoffBehavior, int layer);
    static ObjectPtr GetCurrentPropertyValue(AnimationStorage* pStorage, DependencyObject* pDO, const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, Object* pBaseValue);

    void ClearAnimations();
    void WritePostscript();
    bool get_IsEmpty() const; 

    AnimationStorage();
    ~AnimationStorage();
private:
    void AttachAnimationClock(AnimationClock* pAnimationClock);
    void DetachAnimationClock(AnimationClock* pAnimationClock);
    void Initialize(DependencyObject* pDO, const DependencyProperty& dp);
    static TRefCountedPtr<AnimationStorage> CreateStorage(DependencyObject* pDO, const DependencyProperty& dp);

    void OnCurrentTimeInvalidated(Object* pSender, EventArgs* pEventArgs);

    TWeakPtr<DependencyObject> m_pDependencyObject;
    const DependencyProperty* m_pDependencyProperty;
    ObjectPtr m_pSnapshotValue;
    ObjectPtr m_pBaseValue;
    ItemStructList<AnimationClockPtr> m_animationClocks;
    static UncommonField<ItemStructMap<ObjectPtr> > AnimatedPropertyMapField;  
};


}; // namespace AVUI
