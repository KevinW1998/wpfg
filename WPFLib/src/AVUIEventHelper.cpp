#include <AVUICommon.h>
#include <AVUIEvent.h>


namespace AVUI {

void EventHelper::AddEventHandler(EventHandlerList* pEventHandlerList, Delegate* pNewHandler)
{
    pEventHandlerList->AddItem(pNewHandler);
}

void EventHelper::RemoveEventHandler(EventHandlerList* pEventHandlerList, Delegate* pRemoveHandler)
{
    if(pEventHandlerList == NULL)
    {
        return;
    }

    if(pRemoveHandler == NULL)
    {
        LibraryCriticalError();
    }

    for(int idx = 0; idx < pEventHandlerList->get_Count(); idx++)
    {
        if((*pEventHandlerList)[idx]->IsEqual(pRemoveHandler))
        {
            pEventHandlerList->RemoveAt(idx);
            break;
        }
    }
}

void EventHelper::InvokeEventHandler(EventHandlerList* pEventHandlerList, Object* pSender, Object* pArgs)
{
    EventHandlerList invokeList;

    if(pEventHandlerList->get_Count() == 0)
    {
        return;
    }

    for(int idx = 0; idx < pEventHandlerList->get_Count(); idx++)
    {
        invokeList.AddItem((*pEventHandlerList)[idx]);
    }


    for(int idx = 0; idx < invokeList.get_Count(); idx++)
    {
        invokeList[idx]->Invoke(pSender, pArgs);
    }
}


}; // namespace AVUI
