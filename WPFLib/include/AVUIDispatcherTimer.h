#pragma once

#include <AVUIEventableObject.h>

namespace AVUI {

class DispatcherTimer : public EventableObject
{
    friend class Dispatcher;

public:
    DispatcherTimer(const TimeSpan& interval, EventHandler* pCallback = NULL);
    DispatcherTimer();


    void Start();
    void Stop();

    DECLARE_ELEMENT(DispatcherTimer, Object)
    IMPLEMENT_EVENT(Tick, EventArgs);

    TimeSpan get_DueTime() const { return m_dueTime; }

    void set_Interval(const TimeSpan newInterval) { m_interval = newInterval; }
    TimeSpan get_Interval() { return m_interval; }

    Object* get_Tag() { return m_pTag; }
    void set_Tag(Object* pTag) { m_pTag = pTag; }

    bool get_IsEnabled() const { return m_isEnabled; }
    void set_IsEnabled(bool isEnabled);

private:

    void Restart();
    void FireTick();
    
    TimeSpan m_interval;
    TimeSpan m_dueTime;
    bool m_isEnabled;
    TRefCountedPtr<Object> m_pTag;
};



}; // namespace AVUI
