#pragma once

#include <AVUIItemStructList.h>
#include <AVUIResourceDictionaryCollection.h>

namespace AVUI {

class ResourceDictionary : public Object
{
public:
    DECLARE_ELEMENT(ResourceDictionary, Object);

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(MergedDictionaries, ResourceDictionaryCollection, false);
    IMPLEMENT_REFERENCE_PROPERTY(Source, String, false);

    ResourceDictionary() { };

    void Add(Object* pKey, Object* pValue);

    Object* GetValue(const Object* pKey) const;
    bool Contains(Object* pKey);
    void Remove(Object* pKey);

    void Clear();


    ResourceDictionaryCollection* get_MergedDictionaries() { return &m_mergedDictionaries; }

    void set_Source(String* pSource);
    String* get_Source() { return m_pSource; }

    void Startup();

private:

    struct ResourceValue
    {
        ResourceValue() { }
        ResourceValue(Object* pKeyIn, Object* pValueIn) : pKey(pKeyIn), pValue(pValueIn) { }
        ObjectPtr pKey;
        ObjectPtr pValue;
    };

    ItemStructList<ResourceValue> m_valuesTable;
    mutable ResourceDictionaryCollection m_mergedDictionaries;
    TRefCountedPtr<String> m_pSource;
};



}; // namespace AVUI
