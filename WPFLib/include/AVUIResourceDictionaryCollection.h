#pragma once

#include <AVUIObjectCollection.h>

namespace AVUI {

class ResourceDictionary;
    
class ResourceDictionaryCollection : public TObjectCollection<ResourceDictionary>
{
public:
    DECLARE_ELEMENT(ResourceDictionaryCollection, ObjectCollectionBase);
    ResourceDictionaryCollection() { }
};


}; // namespace AVUI
