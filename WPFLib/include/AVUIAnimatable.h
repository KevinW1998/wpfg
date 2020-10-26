#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class AnimationTimeline;
class Animatable : public DependencyObject
{
    friend class BindingExpression;
    friend class DependencyObject;
public:
    Animatable() { };
    ~Animatable() { };

    DECLARE_ELEMENT(Animatable, DependencyObject);

    void BeginAnimation(const DependencyProperty& dp, AnimationTimeline* pTimeline, HandoffBehavior::Enum handoffBehavior = HandoffBehavior::SnapshotAndReplace);

    TRefCountedPtr<Animatable> Clone();

    IMPLEMENT_EVENT(Changed, EventArgs);

    void OnPropertyChanged(const DependencyPropertyChangedEventArgs& e);

protected:
    virtual TRefCountedPtr<Animatable> CreateInstanceCore();
    virtual void CloneCore(Animatable* pSource);
    virtual void CloneCurrentValueCore(Animatable* pSource);
    void OnAnimatablePropertyChanged(DependencyObject* pOldValue, DependencyObject* pNewValue);


    virtual DependencyObject* get_InheritanceContext();
    virtual void AddInheritanceContext(DependencyObject*, const DependencyProperty* pDP);
    virtual void RemoveInheritanceContext(DependencyObject*, const DependencyProperty* pDP);
    virtual bool get_HasMultipleInheritanceContexts();

    void WritePostscript();

private:
    void FireChanged();

    virtual DependencyObject* get_InheritanceParent() { return object_cast<DependencyObject>(get_InheritanceContext()); }
    virtual void EvaluateAnimatedValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pEntry);

    void InvalidateSubProperties();

    void CloneCoreCommon(Animatable* pSource, bool useCurrentValue);
    void PruneInheritanceContexts();

    struct AnimatableContextPair
    {
        TWeakPtr<DependencyObject> Owner;
        const DependencyProperty* pProperty;
    };

    ItemStructList< AnimatableContextPair > m_inheritanceContexts;

};

typedef TRefCountedPtr<Animatable> AnimatablePtr;

}; // namespace AVUI
