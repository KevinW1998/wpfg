#include <AVUICommon.h>
#include <AVUIEventRoute.h>

namespace AVUI {

void EventRoute::Add(Object* pSource, Delegate* pEventHandler, bool handledEventsToo)
{
    EventRouteItem newItem;

    newItem.pObject = pSource;
    newItem.pEventHandler = pEventHandler;
    newItem.HandledEventsToo = handledEventsToo;

    m_items.AddItem(newItem);
}

void EventRoute::InvokeHandlers(Object* pSource, RoutedEventArgs* pArgs)
{
    RoutingStrategy::Enum routingStrategy = pArgs->get_RoutedEvent()->get_RoutingStrategy();
    if(routingStrategy == RoutingStrategy::Bubble || routingStrategy == RoutingStrategy::Direct)
    {
        for(int i = 0; i < (int) m_items.get_Count(); i++)
        {
            if(!pArgs->get_IsHandled() || m_items[i].HandledEventsToo)
            {
                m_items[i].pEventHandler->Invoke(m_items[i].pObject, pArgs);
            }
        }
    }
    else
    {
        for(int i = (int) m_items.get_Count() - 1; i >=0 ; i--)
        {
            if(!pArgs->get_IsHandled() || m_items[i].HandledEventsToo)
            {
                m_items[i].pEventHandler->Invoke(m_items[i].pObject, pArgs);
            }
        }
    }
}


}; // namespace AVUI
