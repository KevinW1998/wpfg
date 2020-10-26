#include <AVUICommon.h>
#include <AVUIEventableObject.h>


namespace AVUI
{

void EventableObject::AddEventHandler(const void* pvKey, Delegate* pEventHandler)
{
    if(m_pList == NULL)
    {
        m_pList = object_allocate<ItemStructList<EventRegistration> >();
    }

    for(int i = 0; i < m_pList->get_Count(); i++)
    {
        EventRegistration& eventRegistration = (*m_pList)[i];
        if(eventRegistration.Key == pvKey)
        {
            EventHelper::AddEventHandler(&(eventRegistration.eventHandlerList), pEventHandler);
            return;
        }
    }

    m_pList->AddNew();
    (*m_pList)[m_pList->get_Count() - 1].Key = pvKey;
    (*m_pList)[m_pList->get_Count() - 1].eventHandlerList.AddItem(pEventHandler);
}

void EventableObject::RemoveEventHandler(const void* pvKey, Delegate* pEventHandler)
{
    if(m_pList == NULL)
    {
        return;
    }

    for(int i = 0; i < m_pList->get_Count(); i++)
    {
        EventRegistration& eventRegistration = (*m_pList)[i];
        if(eventRegistration.Key == pvKey)
        {
            EventHelper::RemoveEventHandler(&(eventRegistration.eventHandlerList), pEventHandler);
            break;
        }
    }
}

void EventableObject::InvokeEventHandler(const void* pvKey, Object* pSender, Object* pArgs)
{
    if(m_pList == NULL)
    {
        return;
    }

    for(int i = 0; i < m_pList->get_Count(); i++)
    {
        EventRegistration& eventRegistration = (*m_pList)[i];
        if(eventRegistration.Key == pvKey)
        {
            EventHelper::InvokeEventHandler(&(eventRegistration.eventHandlerList), pSender, pArgs);
            break;
        }
    }
}

void EventableObject::AddInstanceHandlersToRoute(EventRoute* pRoute)
{
    if(m_pList == NULL)
    {
        return;
    }

    for(int i = 0; i < m_pList->get_Count(); i++)
    {
        EventRegistration& eventRegistration = (*m_pList)[i];
        if(eventRegistration.Key == pRoute->get_RoutedEvent())
        {
            for(int idx = 0; idx < eventRegistration.eventHandlerList.get_Count(); idx++)
            {
                pRoute->Add(this, eventRegistration.eventHandlerList[idx], false);
            }
            break;
        }
    }
}


};


