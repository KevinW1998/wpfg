#pragma once

#include <AVUITriggerAction.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class TriggerActionCollection : public TObjectCollection<TriggerAction>
{
public:
    DECLARE_ELEMENT(TriggerActionCollection, ObjectCollectionBase);
};


}; // namespace AVUI
