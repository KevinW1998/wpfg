#pragma once

#include <AVUISetter.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class SetterCollection : public TObjectCollection<Setter>
{
public:
    DECLARE_ELEMENT(SetterCollection, ObjectCollectionBase);
};


}; // namespace AVUI
