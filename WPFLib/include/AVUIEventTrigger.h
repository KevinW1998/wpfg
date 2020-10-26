#pragma once

#include <AVUITriggerBase.h>

namespace AVUI {
class TriggerCollection;
class EventTrigger : public TriggerBase
{
    friend class UIElement;
public:
    EventTrigger() : m_pRoutedEvent(NULL) { }

    DECLARE_ELEMENT(EventTrigger, TriggerBase);

    virtual void Seal();

    const RoutedEvent* get_RoutedEvent() { return m_pRoutedEvent; }
    void set_RoutedEvent(const RoutedEvent* pRE) { m_pRoutedEvent = pRE; }

    String* get_SourceName() { return m_pSourceName; }
    void set_SourceName(String* pSourceName) { m_pSourceName = pSourceName; }

    TriggerActionCollection* get_Actions() { return &m_actions; }

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Actions, TriggerActionCollection, true);
    IMPLEMENT_REFERENCE_PROPERTY(RoutedEvent, RoutedEvent, false);
    IMPLEMENT_REFERENCE_PROPERTY(SourceName, String, false);


private:

    void Handler(Object* pSender, RoutedEventArgs* pArgs);

    static void ProcessTriggerCollection(UIElement* pUIElement);
    static void ProcessOneTrigger(UIElement* pTriggerHost, TriggerBase* pTrigger);

    static UncommonField<TriggerCollection>  TriggerCollectionField;  

    const RoutedEvent* m_pRoutedEvent;
    StringPtr m_pSourceName;
    TriggerActionCollection m_actions;
};




}; // namespace AVUI
