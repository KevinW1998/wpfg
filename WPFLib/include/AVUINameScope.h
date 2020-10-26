#pragma once

#include <AVUIUncommonField.h>
#include <AVUIItemStructList.h>

namespace AVUI {

class DependencyObject;

class INameScope
{
public:
    virtual Object* FindName(const String& Name) = 0;
    virtual void RegisterName(const String& Name, Object* pObject) = 0;
    virtual void UnregisterName(const String& Name) = 0;
};


class NameScope : public Object, public INameScope
{
public:
    DECLARE_ELEMENT(NameScope, Object);

    virtual Object* FindName(const String& Name);
    virtual void RegisterName(const String& Name, Object* pObject);
    virtual void UnregisterName(const String& Name);

    static void SetNameScope(DependencyObject* pDO, NameScope* pNameScope);
    static NameScope* GetNameScope(DependencyObject* pDO);

private:
    static UncommonField<NameScope> NameScopeField;  

    struct NameMapEntry
    {
        NameMapEntry(String* pNameIn, Object* pObjectIn) : Name(pNameIn), ObjectEntry(pObjectIn) { }
        NameMapEntry() { }
        StringPtr Name;
        ObjectWeakPtr ObjectEntry;
    };

    ItemStructList<NameMapEntry> m_nameEntries;
};

typedef TRefCountedPtr<NameScope> NameScopePtr;

}; // namespace AVUI
