#include <AVUICommon.h>
#include <AVUIClock.h>
#include <AVUIClockGroup.h>
#include <AVUITimeManager.h>
#include <AVUIParallelTimeline.h>

namespace AVUI {

TimeManager::TimeManager()
{
    m_timeState = TimeState::Stopped;
    m_lastTimeState = TimeState::Stopped;
    m_globalTime = TimeSpan((long) (-1));
    m_lastTickTime = TimeSpan((long) (-1));
    m_nextTickTimeQueried = false;
    m_isInTick = false;
    m_lockTickTime = false;

    TRefCountedPtr<ParallelTimeline> rootTimeline(new ParallelTimeline(Nullable<TimeSpan>(TimeSpan((long) 0)), Duration::get_Forever()), false);

    m_rootClock = TRefCountedPtr<ClockGroup>(new ClockGroup(rootTimeline), false);
    m_rootClock->MakeRoot(this);
}


void TimeManager::Tick(float timeSinceLastFrame)
{
    m_overallTime = m_overallTime + TimeSpan::FromSeconds(timeSinceLastFrame);
    m_lastTickTime = m_globalTime;
    m_nextTickTimeQueried = false;
    m_isDirty = false;

    if (m_timeState == TimeState::Running)
    {
        m_globalTime = get_CurrentGlobalTime();
        m_isInTick = true;
    }

    if ((m_lastTimeState == TimeState::Stopped) && (m_timeState == TimeState::Stopped))
    {
        m_currentTickInterval = TimeIntervalCollection::CreateNullPoint();
    }
    else
    {
        m_currentTickInterval = TimeIntervalCollection::CreateOpenClosedInterval(m_lastTickTime, m_globalTime);
        if ((m_lastTimeState == TimeState::Stopped) || (m_timeState == TimeState::Stopped))
        {
            m_currentTickInterval.AddNullPoint();
        }
    }
    m_rootClock->ComputeTreeState();
    m_lastTimeState = m_timeState;
    RaiseEnqueuedEvents();

    m_isInTick = false;
    CleanupClocks();
}

void TimeManager::CleanupClocks()
{
    if (m_needClockCleanup)
    {
        m_needClockCleanup = false;
        m_rootClock->RootCleanChildren();
    }    
}

void TimeManager::RaiseEnqueuedEvents()
{
    for(int i = 0; i < m_eventQueue.get_Count(); i++)
    {
        Clock* pClock = m_eventQueue[i];

        if(pClock != NULL)
        {
            pClock->RaiseAccumulatedEvents();
        }
    }
    m_eventQueue.Clear();
}

TimeSpan TimeManager::get_CurrentGlobalTime()
{
    switch (m_timeState)
    {
        case TimeState::Stopped:
              return TimeSpan::get_Zero();
    
        case TimeState::Paused:
              return (m_pauseTime - m_startTime);
    
        case TimeState::Running:
              if (!m_isInTick && !m_lockTickTime)
              {
                    return TimeSpan::FromMilliseconds( get_CurrentOverallTime().get_Milliseconds() - m_startTime.get_Milliseconds());
              }
              return m_globalTime;
    }

    return TimeSpan::get_Zero();    
}

TimeSpan TimeManager::get_CurrentOverallTime()
{
    return m_overallTime;
}

void TimeManager::Start()
{
    if (m_timeState == TimeState::Stopped)
    {
        m_lastTickTime = TimeSpan((long) (-1));
        m_startTime = get_CurrentOverallTime();
        m_globalTime = TimeSpan::get_Zero();
        m_timeState = TimeState::Running;
        m_rootClock->RootActivate();
    }
}

 


}; // namespace AVUI
