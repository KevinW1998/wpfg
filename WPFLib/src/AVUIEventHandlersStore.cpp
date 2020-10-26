#include <AVUICommon.h>
#include <AVUIEventHandlersStore.h>


namespace AVUI
{

void EventHandlersStore::AddRoutedEventHandler(const RoutedEvent* pRoutedEvent, Delegate* pEventHandler)
{
    for(int idx = 0; idx < m_handlerRecords.get_Count(); idx++)
    {
        if(m_handlerRecords[idx].pRoutedEvent == pRoutedEvent)
        {
            m_handlerRecords[idx].pHandler = pEventHandler;
            return;
        }
    }

    m_handlerRecords.AddItem(EventHandlerRecord(pRoutedEvent, pEventHandler));
}

void EventHandlersStore::AddHandlersToRoute(Object* pSource, EventRoute* pRoute)
{
    for(int idx = 0; idx < m_handlerRecords.get_Count(); idx++)
    {
        if(m_handlerRecords[idx].pRoutedEvent == pRoute->get_RoutedEvent())
        {
            pRoute->Add(pSource, m_handlerRecords[idx].pHandler, false);
            break;
        }
    }
}


};
