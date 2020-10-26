#include <AVUITimeIntervalCollection.h>

namespace AVUI {

class ClockGroup;
class Clock;

class TimeManager
{
public:
    ClockGroup* get_RootClock() { return m_rootClock; }

    void set_Dirty() { m_isDirty = true; }
    bool get_IsDirty() const { return m_isDirty; }

    const TimeSpan& get_InternalCurrentGlobalTime() { return m_globalTime; }

    TimeState::Enum get_State() const { return m_timeState; }
    void NotifyNewEarliestFutureActivity() { /*No implementation */ } 

    bool get_InternalIsStopped() const { return m_timeState == TimeState::Stopped; }

    void set_InternalCurrentIntervals(const TimeIntervalCollection& intervals) { m_currentTickInterval = intervals; }
    const TimeIntervalCollection& get_InternalCurrentIntervals() const { return m_currentTickInterval; }

    void AddToEventQueue(Clock* pClock) { m_eventQueue.AddItem(pClock); }

    TimeSpan get_LastTickDelta() { return m_globalTime - m_lastTickTime; }

    void Start();
    void Tick(float timeSinceLastFrame);
    TimeSpan get_CurrentGlobalTime();

    TimeSpan get_CurrentOverallTime();

    TimeManager();
private:

    void CleanupClocks();
    void RaiseEnqueuedEvents();

    TimeIntervalCollection m_currentTickInterval;
    TimeSpan m_globalTime;
    bool m_isDirty;
    bool m_isInTick;
    TimeSpan m_lastTickTime;
    TimeSpan m_startTime;
    TimeState::Enum m_lastTimeState;
    TimeState::Enum m_timeState;
    bool m_lockTickTime;
    bool m_needClockCleanup;
    bool m_nextTickTimeQueried;
    TimeSpan m_pauseTime;
    TRefCountedPtr<ClockGroup> m_rootClock;
    ItemStructList<TWeakPtr<Clock> > m_eventQueue;
    TimeSpan m_overallTime;
};

}; // namespace AVUI
