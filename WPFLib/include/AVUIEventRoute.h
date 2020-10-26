#pragma once

#include <AVUIItemStructList.h>

namespace AVUI {

class RoutedEvent;
class EventRoute
{
public:
    EventRoute(const RoutedEvent* pEvent) : m_pEvent(pEvent) { };
    void AddSource(Object* pSource) { };
    void Add(Object* pSource, Delegate* pEventHandler, bool handledEventsToo);
    void InvokeHandlers(Object* pSource, RoutedEventArgs* pArgs);

    const RoutedEvent* get_RoutedEvent() const { return m_pEvent; }

    struct EventRouteItem
    {
        EventRouteItem() : pObject(NULL), pEventHandler(NULL), HandledEventsToo(false) { }

        Object* pObject;
        Delegate* pEventHandler;
        bool HandledEventsToo;
    };

private:

    ItemStructList<EventRouteItem> m_items;
    const RoutedEvent* m_pEvent;
};


}; // namespace AVUI
