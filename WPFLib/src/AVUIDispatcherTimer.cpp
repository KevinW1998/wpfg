#include <AVUICommon.h>
#include <AVUIDispatcher.h>
#include <AVUITimeManager.h>
#include <AVUIDispatcherTimer.h>

REGISTER_ELEMENT(AVUI::DispatcherTimer);

namespace AVUI {


DispatcherTimer::DispatcherTimer(const TimeSpan& interval, EventHandler* pCallback) : m_interval(interval), m_isEnabled(false)
{
    if(pCallback != NULL)
    {
        AddEventHandler(&get_TickEvent(), pCallback);
    }
}

DispatcherTimer::DispatcherTimer() : m_isEnabled(false)
{

}


void DispatcherTimer::Start()
{
    if(!m_isEnabled)
    {
        m_isEnabled = true;
        Restart();
    }
}

void DispatcherTimer::Stop()
{
    if(m_isEnabled)
    {
        m_isEnabled = false;
        Dispatcher::get_CurrentDispatcher()->RemoveTimer(this);
    }
}

void DispatcherTimer::FireTick()
{
    InvokeEventHandler(&get_TickEvent(), this, NULL);

    if(m_isEnabled)
    {
        Restart();
    }
}

void DispatcherTimer::Restart()
{
    m_dueTime = Dispatcher::get_CurrentDispatcher()->get_TimeManager()->get_CurrentOverallTime() + m_interval;
    Dispatcher::get_CurrentDispatcher()->AddTimer(this);
}

void DispatcherTimer::set_IsEnabled(bool isEnabled)
{
    if (!isEnabled && m_isEnabled)
    {
        Stop();
    }
    else if (isEnabled && !m_isEnabled)
    {
        Start();
    }
}


}; // namespace AVUI
