#include <AVUICommon.h>
#include <AVUIDependencyObject.h>
#include <AVUINameScope.h>

REGISTER_ELEMENT_BEGIN(AVUI::NameScope)

    REGISTER_INTERFACE(INameScope)
    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()

REGISTER_INTERFACE_TYPE(AVUI::INameScope);


namespace AVUI {

UncommonField<NameScope> NameScope::NameScopeField;

Object* NameScope::FindName(const String& Name)
{
    for(int i = 0; i < m_nameEntries.get_Count(); i++)
    {
        if(*(m_nameEntries[i].Name) == Name)
        {
            return m_nameEntries[i].ObjectEntry;
        }
    }
    return NULL;
}

void NameScope::RegisterName(const String& Name, Object* pObject)
{
    for(int i = 0; i < m_nameEntries.get_Count(); i++)
    {
        if(*(m_nameEntries[i].Name) == Name)
        {
            LibraryCriticalError();
        }
    }

    StringPtr pString(new String(Name), false);

    m_nameEntries.AddItem(NameMapEntry(pString, pObject));
}

void NameScope::UnregisterName(const String& Name)
{
    for(int i = 0; i < m_nameEntries.get_Count(); i++)
    {
        if(*(m_nameEntries[i].Name) == Name)
        {
            m_nameEntries.RemoveAt(i);
            return;
        }
    }
}

void NameScope::SetNameScope(DependencyObject* pDO, NameScope* pNameScope)
{
    NameScopeField.SetValue(pDO, pNameScope);
}

NameScope* NameScope::GetNameScope(DependencyObject* pDO)
{
    return NameScopeField.GetValue(pDO);
}


}; // namespace AVUI
