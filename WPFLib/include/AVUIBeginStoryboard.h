#pragma once

#include <AVUITriggerAction.h>

namespace AVUI {

class Storyboard;

class BeginStoryboard : public TriggerAction
{
public:
    BeginStoryboard() { }

    DECLARE_ELEMENT(BeginStoryboard, TriggerAction);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Storyboard, Storyboard, NULL, PropertyFlag::IsContentProperty);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(HandoffBehavior, HandoffBehavior, HandoffBehavior::SnapshotAndReplace, PropertyFlag::None);


    void Invoke(DependencyObject* pDO, ControlTemplate* pTemplateScope);

    void Seal();

private:

    void Begin(DependencyObject* pTarget, int layer);

};


}; // namespace AVUI
