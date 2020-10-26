#include <AVUICommon.h>
#include <AVUIResourceDictionary.h>
#include <AVUIDependencyProperty.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>

REGISTER_ELEMENT_BEGIN(AVUI::ResourceDictionaryCollection)

REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::ResourceDictionary)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(MergedDictionaries)
    REGISTER_PROPERTY(Source)
REGISTER_ELEMENT_END()

namespace AVUI {

void ResourceDictionary::Add(Object* pKey, Object* pValue)
{
    m_valuesTable.AddItem(ResourceValue(pKey, pValue));
}

Object* ResourceDictionary::GetValue(const Object* pKey) const
{
    for(int i = 0; i < m_valuesTable.get_Count(); i++)
    {
        if(Object::Equals(pKey, m_valuesTable[i].pKey))
        {
            return m_valuesTable[i].pValue;
        }
    }

    for(int i = 0; i < m_mergedDictionaries.get_Count(); i++)
    {
        Object* pValue = m_mergedDictionaries[i]->GetValue(pKey);

        if(pValue != DependencyProperty::get_UnsetValue())
        {
            return pValue;
        }
    }

    return DependencyProperty::get_UnsetValue();
}

bool ResourceDictionary::Contains(Object* pKey)
{
    for(int i = 0; i < m_valuesTable.get_Count(); i++)
    {
        if(Object::Equals(pKey, m_valuesTable[i].pKey))
        {
            return true;
        }
    }

    for(int i = 0; i < m_mergedDictionaries.get_Count(); i++)
    {
        Object* pValue = m_mergedDictionaries[i]->GetValue(pKey);

        if(pValue != DependencyProperty::get_UnsetValue())
        {
            return true;
        }
    }

    return false;
}

void ResourceDictionary::Remove(Object* pKey)
{
    for(int i = 0; i < m_valuesTable.get_Count(); i++)
    {
        if(Object::Equals(pKey, m_valuesTable[i].pKey))
        {
            m_valuesTable.RemoveAt(i);
            break;
        }
    }
}

void ResourceDictionary::Clear()
{
    m_valuesTable.Clear();
}


void ResourceDictionary::set_Source(String* pSource)
{
    m_pSource = pSource;
}

void ResourceDictionary::Startup()
{
    if(m_pSource != NULL)
    {
        IClientNavResult* pNavResult = Application::get_Current()->get_ClientRendererInterface()->NavigateTo(m_pSource->get_Ptr());

        if(pNavResult->GetNavStatus() == NavigationStatus::Navigated)
        {
            ObjectPtr pObjResult = pNavResult->GetNavResult();

            if(pObjResult->IsType(GET_TYPE(ResourceDictionary)))
            {
                m_valuesTable = object_cast<ResourceDictionary>(pObjResult)->m_valuesTable;
            }
            else
            {
                LibraryCriticalError();
            }
        }
        pNavResult->Destroy();
    }

    for(int idx = 0; idx < m_mergedDictionaries.get_Count(); idx++)
    {
        m_mergedDictionaries[idx]->Startup();
    }
}



}; // namespace AVUI
