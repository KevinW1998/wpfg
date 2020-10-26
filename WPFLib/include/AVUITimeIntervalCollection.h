
#pragma once

#include <AVUIItemStructList.h>

namespace AVUI {

class TimeIntervalCollection
{
public:
    TimeIntervalCollection();

    void AddNullPoint() { m_containsNullPoint = true; }

    const TimeSpan& get_FirstNodeTime() const { return m_nodes[0].nodeTime; }
    TimeIntervalCollection SlipBeginningOfConnectedInterval(const TimeSpan& slipTime);
    static TimeIntervalCollection get_Empty() { return TimeIntervalCollection(); }

    static TimeIntervalCollection CreatePoint(const TimeSpan& timeSpan) { return TimeIntervalCollection(timeSpan); };
    static TimeIntervalCollection CreateNullPoint() { return TimeIntervalCollection(true); };
    static TimeIntervalCollection CreateOpenClosedInterval(const TimeSpan& from, const TimeSpan& to) { return TimeIntervalCollection(from, false, to, true); };
    static TimeIntervalCollection CreateClosedOpenInterval(const TimeSpan& from, const TimeSpan& to) { return TimeIntervalCollection(from, true, to, false); };
    static TimeIntervalCollection CreateInfiniteClosedInterval(const TimeSpan& from) { return TimeIntervalCollection(from, true); };

    bool Intersects(const TimeIntervalCollection& other) const;
    bool IntersectsInverseOf(const TimeIntervalCollection& other) const;
    bool IntersectsPeriodicCollection(const TimeSpan& beginTime, const Duration& period, float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) const;

    bool get_ContainsNullPoint() const { return InvertIfNecessary(m_containsNullPoint); }

    void Clear();

    void ProjectOntoPeriodicFunction(TimeIntervalCollection* pIntervalCollection, const TimeSpan& beginTime, const Nullable<TimeSpan>& endTime, const Duration& fillDuration, const Duration& period, 
                                     float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) const;

    void ProjectPostFillZone(TimeIntervalCollection* pProjection, const TimeSpan& beginTime, const TimeSpan& endTime, const Duration& period, float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) const;

private:
    bool get_IsEmptyOfRealPoints() const { return m_count == 0; };
    bool IntersectsHelper(const TimeIntervalCollection& other) const;
    void EnsureAllocatedCapacity(int capacity);
    void set_InvertedMode(bool isInverted) const { m_invertCollection = isInverted; }

    void MoveFirst() const { m_current = 0; };
    void MoveNext() const { m_current++; };

    const TimeSpan& get_CurrentNodeTime() const { return m_nodes[m_current].nodeTime; }
    void set_CurrentNodeTime(const TimeSpan& nodeTime) { m_nodes[m_current].nodeTime = nodeTime; }

    const TimeSpan& get_NextNodeTime() const { return m_nodes[m_current + 1].nodeTime; }
    void set_NextNodeTime(const TimeSpan& nodeTime);

    bool get_CurrentIsAtLastNode() const { return (m_current + 1) == m_count; }

    bool get_CurrentNodeIsInterval() const { return InvertIfNecessary(m_nodes[m_current].nodeIsInterval); }
    void set_CurrentNodeIsInterval(bool nodeIsInterval) { m_nodes[m_current].nodeIsInterval = nodeIsInterval; }

    bool get_CurrentNodeIsPoint() const { return InvertIfNecessary(m_nodes[m_current].nodeIsPoint); };
    void set_CurrentNodeIsPoint(bool nodeIsPoint) { m_nodes[m_current].nodeIsPoint = nodeIsPoint; }

    bool get_NextNodeIsInterval() const { return InvertIfNecessary(m_nodes[m_current + 1].nodeIsInterval); }
    bool get_NextNodeIsPoint() const { return InvertIfNecessary(m_nodes[m_current + 1].nodeIsPoint); };

    void ProjectionWarp(int periodInTicks, float accelRatio, float decelRatio);
    void ProjectionNormalize(TimeIntervalCollection* pProjection, const TimeSpan& beginTime, const Nullable<TimeSpan>& endTime, bool includeFillPeriod, float speedRatio) const;
    void ProjectionFold(TimeIntervalCollection* pProjection, const Nullable<TimeSpan>& activeDuration, int periodInTicks, bool isAutoReversed, bool includeMaxPoint);
    void ProjectionFoldPoint(TimeIntervalCollection* pProjection, const Nullable<TimeSpan>& activeDuration, int periodInTicks, bool isAutoReversed, bool includeMaxPoint);
    bool ProjectionFoldInterval(TimeIntervalCollection* pProjection, const Nullable<TimeSpan>& activeDuration, int periodInTicks, bool isAutoReversed, bool includeMaxPoint);
    void MergePoint(const TimeSpan& point);
    void MergeInterval(const TimeSpan& from, bool includeFrom, const TimeSpan& to, bool includeTo);
    int Locate(const TimeSpan& time);

    bool InvertIfNecessary(bool input) const { return m_invertCollection ? !input : input; }
    static void IntersectsHelperPrepareIndexers(const TimeIntervalCollection* pTic1, const TimeIntervalCollection* pTic2);
    static bool IntersectsHelperEqualCase(const TimeIntervalCollection* pTic1, const TimeIntervalCollection* pTic2, bool* pIntersectionFound);
    static bool IntersectsHelperUnequalCase(const TimeIntervalCollection* pTic1, const TimeIntervalCollection* pTic2, bool* pIntersectionFound);

    TimeIntervalCollection(bool containsNullPoint);
    TimeIntervalCollection(const TimeSpan& point);
    TimeIntervalCollection(const TimeSpan& point, bool includePoint);
    TimeIntervalCollection(const TimeSpan& from, bool includeFrom, const TimeSpan& to, bool includeTo);

    void Init(bool containsNullPoint); 
    void InitializePoint(const TimeSpan& point);

    bool m_containsNullPoint;
    int m_count;

    mutable int m_current;
    mutable bool m_invertCollection;

    struct NodeData
    {
        NodeData() : nodeIsInterval(false), nodeIsPoint(false) { }
        bool nodeIsInterval;
        bool nodeIsPoint;
        TimeSpan nodeTime;
    };
    ItemStructList<NodeData> m_nodes;
};


}; // namespace AVUI
