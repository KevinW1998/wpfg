#pragma once

#include <AVUIObject.h>
#include <AVUIEvent.h>

namespace AVUI
{

class Delegate;
class EventRegistration;
class EventRoute;

class EventableObject : public Object
{
public:
    EventableObject() { }


protected:

    void AddEventHandler(const void* pvKey, Delegate* pEventHandler);
    void RemoveEventHandler(const void* pvKey, Delegate* pEventHandler);
    void InvokeEventHandler(const void* pvKey, Object* pSender, Object* pArgs);
    void AddInstanceHandlersToRoute(EventRoute* pRoute);

private:

    struct EventRegistration
    {
        EventRegistration() { }

        const void* Key;
        EventHandlerList eventHandlerList;
    };

    TRefCountedPtr<ItemStructList<EventRegistration> > m_pList;

};



}
