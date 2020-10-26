#include <AVUICommon.h>
#include <AVUITypeManager.h>
#include <AVUIDependencyProperty.h>

namespace AVUI {


Type* TypeManager::ms_pTypeFirst = NULL;

void TypeManager::RegisterType(Type* pType)
{
    pType->m_pTypeNext = ms_pTypeFirst;
    ms_pTypeFirst = pType;
}

Type* TypeManager::FindType(const String& type)
{
    Type* pTypeCur = ms_pTypeFirst;

    while(pTypeCur != NULL)
    {
        if(type.Equals(pTypeCur->ToString()))
        {
            return pTypeCur;
        }

	        pTypeCur = pTypeCur->m_pTypeNext;
    }
    return NULL;
}

const DependencyProperty* TypeManager::GetDependencyPropertyByID(unsigned int globalID)
{
    Type* pTypeCur = ms_pTypeFirst;

    while(pTypeCur != NULL)
    {
        PropertyInfo* pCur = pTypeCur->m_pPropertyFirst;
        while(pCur != NULL)
        {
            if(pCur->m_property.get_PropertyID() == globalID)
            {
                return (const DependencyProperty*) &(pCur->m_property);
            }
            pCur = pCur->m_pPropertyInfoNext;
        }
        pTypeCur = pTypeCur->m_pTypeNext;
    }
    return NULL;
}


void TypeManager::CleanupAllocatedData()
{
    Type* pTypeCur = ms_pTypeFirst;

    while(pTypeCur != NULL)
    {
        PropertyInfo* pCur = pTypeCur->m_pPropertyFirst;
        while(pCur != NULL)
        {            
            if(pCur->m_property.IsType(GET_TYPE(DependencyProperty)))
            {
                const DependencyProperty* pProperty = (const DependencyProperty*) &(pCur->m_property);

                pProperty->CleanupAllocatedData();
            }

            pCur = pCur->m_pPropertyInfoNext;
        }
        pTypeCur = pTypeCur->m_pTypeNext;
    }
}




}; // namespace AVUI
