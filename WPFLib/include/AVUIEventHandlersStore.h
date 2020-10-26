#pragma once

namespace AVUI {

class EventRoute;

class EventHandlersStore
{
public:

    void AddRoutedEventHandler(const RoutedEvent* pRoutedEvent, Delegate* pEventHandler);
    void AddHandlersToRoute(Object* pSource, EventRoute* pRoute);

private:

    struct EventHandlerRecord
    {
        EventHandlerRecord() : pRoutedEvent(NULL) { }
        EventHandlerRecord(const RoutedEvent* pRoutedEventIn, Delegate* pHandlerIn) : pRoutedEvent(pRoutedEventIn), pHandler(pHandlerIn) { }

        const RoutedEvent* pRoutedEvent;
        TRefCountedPtr<Delegate> pHandler;
    };

    ItemStructList<EventHandlerRecord> m_handlerRecords;
};

}; // namespace AVUI
