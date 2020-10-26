#pragma once

#include <AVUIClock.h>
#include <AVUITimeIntervalCollection.h>
#include <AVUIItemStructList.h>
#include <AVUIParallelTimeline.h>

namespace AVUI {

class TimeManager;

class ClockGroup : public Clock
{
public:
    DECLARE_ELEMENT(ClockGroup, Clock);

    ClockGroup(TimelineGroup* pTimelineGroup) : Clock(pTimelineGroup) { }

    void AddNullPointToCurrentIntervals();
    virtual void BuildClockSubTreeFromTimeline(Timeline* pTimeline, bool hasControllableRoot);
    virtual void ClearCurrentIntervalsToNull();
    virtual void ComputeCurrentFillInterval(const TimeIntervalCollection& parentIntervalCollection, const TimeSpan& beginTime, const TimeSpan& endTime, 
                                            const Duration& period, float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed);
    virtual void ComputeCurrentIntervals(const TimeIntervalCollection& parentIntervalCollection, const TimeSpan& beginTime, const Nullable<TimeSpan>& endTime, const Duration& fillDuration, const Duration& period, 
                                         float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed);

    void ComputeTreeState();
    void ComputeTreeStateRoot();
    void MakeRoot(TimeManager* pTimeManager);
    void RootActivate();
    void RootCleanChildren();
    void RootDisable();

    TimeIntervalCollection& get_CurrentIntervals() { return m_currentIntervals; }
    Clock* get_FirstChild() { if(m_children.get_Count() > 0) return m_children[0]; return NULL; }
    bool get_RootHasChildren() const { return m_rootChildren.get_Count() > 0; }
    ItemStructList<ClockPtr>& get_Children() { return m_children; }
    const ItemStructList<ClockPtr>& get_Children() const { return m_children; }
    ItemStructList<ClockPtr>& get_InternalRootChildren() { return m_rootChildren; }

    TimelineGroup* get_Timeline() { return (TimelineGroup*) Clock::get_Timeline(); }
private:
    void ClipNextTickByChildren();
    void ComputeTreeStatePostfix();
    bool IsEmptyClockTree(Clock* pClock);

    TimeIntervalCollection m_currentIntervals;
    ItemStructList<ClockPtr> m_rootChildren;
    ItemStructList<ClockPtr> m_children;
};

}; // namespace AVUI
