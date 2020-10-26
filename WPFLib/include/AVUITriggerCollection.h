#pragma once

#include <AVUITriggerBase.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class TriggerCollection : public TObjectCollection<TriggerBase>
{
public:
    DECLARE_ELEMENT(TriggerCollection, ObjectCollectionBase);
};


}; // namespace AVUI
