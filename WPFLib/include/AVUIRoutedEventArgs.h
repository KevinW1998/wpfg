#pragma once

namespace AVUI {

class RoutedEvent;

class RoutedEventArgs : public Object
{
public:
    DECLARE_ELEMENT(RoutedEventArgs, Object);

    RoutedEventArgs() : m_isHandled(false), m_isUserInitiated(false), m_isInvokingHandler(false), m_pOriginalSource(NULL), m_pSource(NULL)
    {

    }

    RoutedEventArgs(const RoutedEvent* pRoutedEvent, Object* pOriginalSource = NULL) : m_pRoutedEvent(pRoutedEvent), m_isHandled(false),
                                                                                       m_isUserInitiated(false), m_isInvokingHandler(false), m_pOriginalSource(pOriginalSource), m_pSource(pOriginalSource)
    {

    }

    void set_RoutedEvent(const RoutedEvent* pRoutedEvent)
    {
        if(get_IsUserInitiated() && get_InvokingHandler())
        {
            LibraryCriticalError();
        }
        m_pRoutedEvent = pRoutedEvent;
    }

    const RoutedEvent* get_RoutedEvent() const { return m_pRoutedEvent; }
    bool get_IsUserInitiated() const { return m_isUserInitiated; }
    bool get_IsHandled() const { return m_isHandled; }
    void set_IsHandled(bool isHandled) { m_isHandled = isHandled; }
    Object* get_Source() const { return m_pSource; }

    Object* get_OriginalSource() { return m_pOriginalSource; }

    void set_Source(Object* pSource);

    void OverrideRoutedEvent(const RoutedEvent* pRoutedEvent) { m_pRoutedEvent = pRoutedEvent; }
    void OverrideSource(Object* pSource) { m_pSource = pSource; }

protected:

    virtual void OnSetSource(Object* pSource) { };

private:
    bool get_InvokingHandler() const { return m_isInvokingHandler; }

    const RoutedEvent* m_pRoutedEvent;
    bool m_isUserInitiated;
    bool m_isInvokingHandler;
    bool m_isHandled;
    Object* m_pSource;
    Object* m_pOriginalSource;
};

}; 
