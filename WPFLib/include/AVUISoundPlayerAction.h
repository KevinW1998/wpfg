#pragma once

#include <AVUITriggerAction.h>

namespace AVUI {

class Storyboard;

class SoundPlayerAction : public TriggerAction
{
public:
    SoundPlayerAction() { }

    DECLARE_ELEMENT(SoundPlayerAction, TriggerAction);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Source, String, NULL, PropertyFlag::IsContentProperty);

    void Invoke(DependencyObject* pDO, ControlTemplate* pTemplateScope);

    void Seal();

private:

    void Begin(DependencyObject* pTarget, int layer);

};


}; // namespace AVUI
