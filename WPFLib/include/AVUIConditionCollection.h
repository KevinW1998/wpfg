#pragma once

#include <AVUICondition.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class ConditionCollection : public TObjectCollection<Condition>
{
public:
    DECLARE_ELEMENT(ConditionCollection, ObjectCollectionBase);
};


}; // namespace AVUI
