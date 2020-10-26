#include <AVUICommon.h>
#include <AVUIEventTrigger.h>
#include <AVUITriggerCollection.h>
#include <AVUIUIElement.h>

REGISTER_ELEMENT_BEGIN(AVUI::EventTrigger)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Actions)
    REGISTER_PROPERTY(RoutedEvent)
    REGISTER_PROPERTY(SourceName)
REGISTER_ELEMENT_END()

namespace AVUI {

UncommonField<TriggerCollection> EventTrigger::TriggerCollectionField;

void EventTrigger::Seal()
{
    if(get_PropertyValues().get_Count() != 0)
    {
        LibraryCriticalError();
    }

    if(get_HasEnterActions() || get_HasExitActions())
    {
        LibraryCriticalError();
    }

    if(m_pRoutedEvent != NULL && m_actions.get_Count() > 0)
    {
//        m_actions.Seal(this);
    }
    TriggerBase::Seal();
}

void EventTrigger::ProcessTriggerCollection(UIElement* pTriggerHost)
{
    TriggerCollection* pTriggers = TriggerCollectionField.GetValue(pTriggerHost);
    if (pTriggers != NULL)
    {
        for (int i = 0; i < pTriggers->get_Count(); i++)
        {
            ProcessOneTrigger(pTriggerHost, (*pTriggers)[i]);
        }
    }
}

void EventTrigger::ProcessOneTrigger(UIElement* pTriggerHost, TriggerBase* pTrigger)
{
    EventTrigger* pEventTrigger = object_cast<EventTrigger>(pTrigger);
    if (pEventTrigger == NULL)
    {
        LibraryCriticalError();
    }

    UIElement* pSource = pTriggerHost;
    if(pEventTrigger->m_pSourceName != NULL)
    {
        Object* pTarget = pTriggerHost->FindName(*(pEventTrigger->m_pSourceName));

        if(pTarget == NULL || !pTarget->IsType(GET_TYPE(UIElement)))
        {
            LibraryCriticalError();
        }
        pSource = (UIElement*) pTarget;
    }

    pSource->AddEventHandler(pEventTrigger->get_RoutedEvent(), RoutedEventHandler::Create(pEventTrigger, &EventTrigger::Handler));
}

void EventTrigger::Handler(Object* pSender, RoutedEventArgs* pArgs)
{
    for(int i = 0; i < m_actions.get_Count(); i++)
    {
        m_actions[i]->Invoke((UIElement*)pSender, NULL);
    }
}


}; // namespace AVUI
