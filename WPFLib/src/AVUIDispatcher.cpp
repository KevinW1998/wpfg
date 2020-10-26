#include <AVUICommon.h>
#include <AVUILayoutManager.h>
#include <AVUIInputManager.h>
#include <AVUITimeManager.h>
#include <AVUIClockGroup.h>
#include <AVUIDispatcher.h>
#include <AVUIDispatcherTimer.h>
#include <AVUIApplication.h>
#include <AVUIDataBindEngine.h>

namespace AVUI {

Dispatcher::Dispatcher()
{
    m_pDataBindEngine = new DataBindEngine();
    m_pLayoutManager = new LayoutManager();
    m_pTimeManager = new TimeManager();
    m_pInputManager = new InputManager();
}
Dispatcher::~Dispatcher()
{
    delete m_pLayoutManager;
    delete m_pTimeManager;
    delete m_pInputManager;
    delete m_pDataBindEngine;
}


Dispatcher* Dispatcher::get_CurrentDispatcher()
{
    return Application::get_Current()->get_Dispatcher();
}

void Dispatcher::Update()
{
    for(int idx = 0; idx < m_dispatcherOperations.get_Count(); idx++)
    {
        m_dispatcherOperations[idx].m_pCallback->Invoke(NULL, m_dispatcherOperations[idx].m_pArg);
    }
    m_dispatcherOperations.Clear();

    TimeSpan currentTime = m_pTimeManager->get_CurrentOverallTime();

    for(int idx = 0; idx < m_dispatcherTimers.get_Count(); idx++)
    {
        if(m_dispatcherTimers[idx]->get_DueTime() <= currentTime)
        {
            m_tickedTimers.AddItem(m_dispatcherTimers[idx]);
            m_dispatcherTimers.RemoveAt(idx);
            idx--;
        }
    }

    for(int idx = 0; idx < m_tickedTimers.get_Count(); idx++)
    {
        m_tickedTimers[idx]->FireTick();
    }
    m_tickedTimers.Clear();
}

void Dispatcher::AddTimer(DispatcherTimer* pTimer)
{
    m_dispatcherTimers.AddItem(pTimer);
}

void Dispatcher::RemoveTimer(DispatcherTimer* pTimer)
{
    m_dispatcherTimers.Remove(pTimer);
}

void Dispatcher::BeginInvoke(DispatcherPriority::Enum priority, DispatcherOperationCallback* pCallback, Object* pArg)
{
    m_dispatcherOperations.AddItem(DispatcherOperation(priority, pCallback, pArg));
}

}; // namespace AVUI
