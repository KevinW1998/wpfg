#include <AVUICommon.h>
#include <AVUIEvent.h>

namespace AVUI {

void EventManager::RegisterClassHandler(const Type& type, const RoutedEvent& routedEvent, Delegate* pEventHandler, bool handledEventsToo)
{
    ItemStructMap<ItemStructList<ClassEventHandler> >& classEventHandlers = *(get_ClassEventHandlers());
    int entryIndex = classEventHandlers.EnsureEntry(type.get_TypeID());

    classEventHandlers[entryIndex].AddItem(ClassEventHandler(routedEvent, pEventHandler, handledEventsToo));
}

void EventManager::CleanupStaticAllocations()
{
    ItemStructMap<ItemStructList<ClassEventHandler> >& classEventHandlers = *(get_ClassEventHandlers());

    classEventHandlers.EraseData();
}


void EventManager::AddListenersToRoute(EventRoute* pRoute, Object* pObject, const Type& type, const RoutedEvent* pEvent)
{
    ItemStructMap<ItemStructList<ClassEventHandler> >& classEventHandlers = *(get_ClassEventHandlers());
    const Type* pTypeCur = &type;

    while(pTypeCur != NULL)
    {
        int entryIndex = classEventHandlers.Search(pTypeCur->get_TypeID());
        if(entryIndex >= 0)
        {
            ItemStructList<ClassEventHandler>& eventHandlerList = classEventHandlers[entryIndex];

            for(int i = 0; i < eventHandlerList.get_Count(); i++)
            {
                if(eventHandlerList[i].pEvent == pEvent)
                {
                    pRoute->Add(pObject, eventHandlerList[i].pEventHandler, eventHandlerList[i].HandledEventsToo);
                }
            }
        }
        pTypeCur = pTypeCur->get_BaseType();
    }
}


}; // namespace AVUI
