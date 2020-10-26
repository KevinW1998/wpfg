#pragma once

#include <AVUIEventableObject.h>
#include <AVUITimeline.h>

namespace AVUI {

class TimeManager;
class TimeIntervalCollection;
class ClockGroup;

/* Animation types -- */
/* Bool, Byte, Char, Decimal, Double, Int16, Int32, Int64, Matrix, Object, Point3D, Quaternion, Rect, Rotation3D, Single, Size, String, Vector3d */

class Clock : public EventableObject
{
public:
    DECLARE_ELEMENT(Clock, Object);

    Clock(Timeline* pTimeline);

    virtual bool get_NeedsTicksWhenActive() const { return true; }
    Timeline* get_Timeline() { return m_pTimeline; }

    static TRefCountedPtr<Clock> AllocateClock(Timeline* pTimeline, bool hasControllableRoot);
    static TRefCountedPtr<Clock> BuildClockTreeFromTimeline(Timeline* pRootTimeline, bool hasControllableRoot);

    void AddToRoot();
 
    virtual void BuildClockSubTreeFromTimeline(Timeline* pTimeline, bool hasControllableRoot);

    virtual void AddNullPointToCurrentIntervals() { /* NO IMPLEMENTATION IN BASE */ };
    virtual void ClearCurrentIntervalsToNull() { /* NO IMPLEMENTATION IN BASE */ };
    virtual void ComputeCurrentFillInterval(const TimeIntervalCollection& parentIntervalCollection, const TimeSpan& beginTime, const TimeSpan& endTime, 
                                            const Duration& period, float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) { /* NO IMPLEMENTATION IN BASE */ };
    virtual void ComputeCurrentIntervals(const TimeIntervalCollection& parentIntervalCollection, const TimeSpan& beginTime, const Nullable<TimeSpan>& endTime, const Duration& fillDuration, const Duration& period, 
                                         float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) { /* NO IMPLEMENTATION IN BASE */ };

    bool get_IsInTimingTree() const;
    void set_Flag(ClockFlags::Enum flag, bool value) { m_flags = (value ? m_flags | (int) flag : m_flags & ~(int)flag); }
    bool get_Flag(ClockFlags::Enum flag) const { return (m_flags & (int) flag) != 0; }

    bool get_RootBeginPending() const { return get_Flag(ClockFlags::RootBeginPending); }
    void set_RootBeginPending(bool value) { return set_Flag(ClockFlags::RootBeginPending, value); }

    bool get_PauseStateChangeDuringTick() const { return get_Flag(ClockFlags::PauseStateChangeDuringTick); }
    void set_PauseStateChangeDuringTick(bool value) { return set_Flag(ClockFlags::PauseStateChangeDuringTick, value); }

    bool get_PendingInteractivePause() const { return get_Flag(ClockFlags::PendingInteractivePause); }
    void set_PendingInteractivePause(bool value) { return set_Flag(ClockFlags::PendingInteractivePause, value); }

    bool get_PendingInteractiveStop() const { return get_Flag(ClockFlags::PendingInteractiveStop); }
    void set_PendingInteractiveStop(bool value) { return set_Flag(ClockFlags::PendingInteractiveStop, value); }

    bool get_PendingInteractiveRemove() const { return get_Flag(ClockFlags::PendingInteractiveRemove); }
    void set_PendingInteractiveRemove(bool value) { return set_Flag(ClockFlags::PendingInteractiveRemove, value); }

    bool get_PendingInteractiveResume() const { return get_Flag(ClockFlags::PendingInteractiveResume); }
    void set_PendingInteractiveResume(bool value) { return set_Flag(ClockFlags::PendingInteractiveResume, value); }

    bool get_CurrentGlobalSpeedInvalidatedEventRaised() const { return get_Flag(ClockFlags::CurrentGlobalSpeedInvalidatedEventRaised); }
    void set_CurrentGlobalSpeedInvalidatedEventRaised(bool value) { return set_Flag(ClockFlags::CurrentGlobalSpeedInvalidatedEventRaised, value); }

    bool get_CurrentStateInvalidatedEventRaised() const { return get_Flag(ClockFlags::CurrentStateInvalidatedEventRaised); }
    void set_CurrentStateInvalidatedEventRaised(bool value) { return set_Flag(ClockFlags::CurrentStateInvalidatedEventRaised, value); }

    bool get_CurrentTimeInvalidatedEventRaised() const { return get_Flag(ClockFlags::CurrentTimeInvalidatedEventRaised); }
    void set_CurrentTimeInvalidatedEventRaised(bool value) { return set_Flag(ClockFlags::CurrentTimeInvalidatedEventRaised, value); }

    bool get_RemoveRequestedEventRaised() const { return get_Flag(ClockFlags::RemoveRequestedEventRaised); }
    void set_RemoveRequestedEventRaised(bool value) { return set_Flag(ClockFlags::RemoveRequestedEventRaised, value); }

    bool get_NeedsPostfixTraversal() const { return get_Flag(ClockFlags::NeedsPostfixTraversal); }
    void set_NeedsPostfixTraversal(bool value) { return set_Flag(ClockFlags::NeedsPostfixTraversal, value); }

    bool get_IsInteractivelyPaused() const { return get_Flag(ClockFlags::IsInteractivelyPaused); }
    void set_IsInteractivelyPaused(bool value) { return set_Flag(ClockFlags::IsInteractivelyPaused, value); }

    bool get_IsInteractivelyStopped() const { return get_Flag(ClockFlags::IsInteractivelyStopped); }
    void set_IsInteractivelyStopped(bool value) { return set_Flag(ClockFlags::IsInteractivelyStopped, value); }

    bool get_IsInEventQueue() const { return get_Flag(ClockFlags::IsInEventQueue); }
    void set_IsInEventQueue(bool value) { return set_Flag(ClockFlags::IsInEventQueue, value); }

    bool get_CompletedEventRaised() const { return get_Flag(ClockFlags::CompletedEventRaised); }
    void set_CompletedEventRaised(bool value) { return set_Flag(ClockFlags::CompletedEventRaised, value); }

    bool get_IsBackwardsProgressingGlobal() const { return get_Flag(ClockFlags::IsBackwardsProgressingGlobal); }
    void set_IsBackwardsProgressingGlobal(bool value) { return set_Flag(ClockFlags::IsBackwardsProgressingGlobal, value); }

    bool get_IsTimeManager() const { return get_Flag(ClockFlags::IsTimeManager); }
    void set_IsTimeManager(bool value) { return set_Flag(ClockFlags::IsTimeManager, value); }

    bool get_AutoReverse() const { return get_Flag(ClockFlags::AutoReverse); }
    void set_AutoReverse(bool value) { return set_Flag(ClockFlags::AutoReverse, value); }

    float get_AccelerationRatio() const { return m_accelerationRatio; }
    float get_DecelerationRatio() const { return m_decelerationRatio; }

    const Duration& get_Duration() const { return m_duration; }
    const RepeatBehavior& get_RepeatBehavior() const { return m_repeatBehavior; }

    FillBehavior::Enum get_FillBehavior() const { return m_fillBehavior; }
    float get_SpeedRatio() const { return m_speedRatio; }

    void set_InternalCurrentIteration(const Nullable<int> iteration) { m_currentIteration = iteration; }


    Nullable<float> get_CurrentProgress() { return m_currentProgress; }
    Nullable<int> get_CurrentIteration() { return m_currentIteration; }

    void set_InternalCurrentProgress(const Nullable<float> progress) { m_currentProgress = progress; }
    void set_InternalCurrentGlobalSpeed(const Nullable<float> globalSpeed) { m_currentGlobalSpeed = globalSpeed; }
    void set_InternalCurrentClockState(ClockState::Enum clockState) { m_currentClockState = clockState; }
    
    const Nullable<TimeSpan>& get_InternalNextTickNeededTime() const { return m_nextTickNeededTime; }
    void set_InternalNextTickNeededTime(const Nullable<TimeSpan>& value) { m_nextTickNeededTime = value; }

    const ClockState::Enum get_CurrentState() { return m_currentClockState; }

    IMPLEMENT_EVENT(CurrentTimeInvalidated, EventArgs);
    IMPLEMENT_EVENT(CurrentStateInvalidated, EventArgs);

    ClockGroup* get_InternalParent() { return m_pParent; }
    void set_InternalParent(ClockGroup* pParent) { m_pParent = pParent; }
    Clock* get_NextSibling();

    Duration get_NaturalDuration() const;

    static TimeSpan MultiplyTimeSpan(const TimeSpan& timeSpan, float factor);
    void ComputeLocalState();
    void ClipNextTickByParent();

    void RaiseCompleted();
    void RaiseCompletedForRoot();
    void RaiseCurrentStateInvalidated();
    void RaiseCurrentGlobalSpeedInvalidated();
    void RaiseCurrentTimeInvalidated();
    void RaiseRemoveRequested();
    void RaiseRemoveRequestedForRoot();

    void RaiseAccumulatedEvents();

    void ResetCachedStateToStopped();

    class SyncData : public Object
    {
    public:
        SyncData(Clock* pClock);

        bool get_IsInSyncPeriod() const { return m_isInSyncPeriod; }
        void set_IsInSyncPeriod(bool isInSyncPeriod) { m_isInSyncPeriod = isInSyncPeriod; }

        const TimeSpan& get_PreviousRepeatTime() const { return m_previousRepeatTime; }
        void set_PreviousRepeatTime(const TimeSpan& previousRepeatTime) { m_previousRepeatTime = previousRepeatTime; }

        const TimeSpan& get_PreviousSyncClockTime() const { return m_previousSyncClockTime; }
        void set_PreviousSyncClockTime(const TimeSpan& previousSyncClockTime) { m_previousSyncClockTime = previousSyncClockTime; }

        Clock* get_SyncClock() { return m_pSyncClock; }

        const TimeSpan& get_SyncClockBeginTime() const { return m_syncClockBeginTime; }
        const Duration& get_SyncClockDuration() 
        {
            if(m_syncClockDuration == Duration::get_Automatic())
            {
                m_pSyncClock->ComputeDuration(&m_syncClockDuration);
            }
            return m_syncClockDuration;
        }
        const Nullable<TimeSpan>& get_SyncClockEffectiveDuration() const { return m_syncClockEffectiveDuration; }
        float get_SyncClockSpeedRatio() const { return m_syncClockSpeedRatio; }

    private:
        bool m_isInSyncPeriod;
        TimeSpan m_previousRepeatTime;
        TimeSpan m_previousSyncClockTime;
        TRefCountedPtr<Clock> m_pSyncClock;
        TimeSpan m_syncClockBeginTime;
        Duration m_syncClockDuration;
        Nullable<TimeSpan> m_syncClockEffectiveDuration;
        float m_syncClockSpeedRatio;
    };

    typedef TRefCountedPtr<Clock::SyncData> SyncDataPtr;

    int get_Depth() const { return m_depth; }

    Nullable<TimeSpan> get_EndOfActivePeriod() const;
    Nullable<TimeSpan> get_CurrentTime() const { return m_currentTime; }

    void set_SyncData(SyncData* pSyncData) { m_pSyncData = pSyncData; }
    SyncData* get_SyncData() { return m_pSyncData; }

    virtual bool get_CanSlip() { return false; }
    bool ComputeDuration(Duration* pDuration) const;


protected:
    virtual void DiscontinuousTimeMovement() { /* NO IMPLEMENTATION ON BASE */ } 
    virtual TimeSpan get_CurrentTimeCore();
    TimeSpan get_CurrentGlobalTime();
    virtual void Stopped() { /* NO IMPLEMENTATION ON BASE */ }
    TimeManager* get_TimeManager() { return m_pTimeManager; }

    void set_TimeManager(TimeManager* pTimeManager) { m_pTimeManager = pTimeManager; }
    void set_Depth(int newDepth) { m_depth = newDepth; }

    SyncDataPtr m_pSyncData;

private:
    void ResetCachedStateToFilling();

    Nullable<TimeSpan> ComputeEffectiveDuration(const Duration& duration, float appliedSpeedRatio) const;
    void ComputeExpectedProgress(const Nullable<TimeSpan>& lastCurrentTime, const Nullable<float>& lastCurrentProgress, 
                                 const Nullable<int>& lastCurrentIteration);
    bool ComputeSpeedRatio(float *pAppliedSpeedRatio) const;
    void NotifyNewEarliestFutureActivity();
    void ComputeLocalStateHelper(bool performTickOperations);    
    TimeSpan DivideTimeSpan(const TimeSpan& timeSpan, float factor);

    bool ComputeCurrentIteration(const TimeSpan& parentTime, float parentSpeed, const Duration& duration, const Nullable<TimeSpan>& expirationTime, float appliedSpeedRatio, TimeSpan* pLocalProgress);
    bool ComputeCurrentSpeed(float localSpeed, float appliedSpeedRatio);
    bool ComputeCurrentState(const Nullable<TimeSpan>& expirationTime, TimeSpan* pParentTime, float parentSpeed);
    void ComputeEvents(const Nullable<TimeSpan>& expirationTime, const Duration& duration, float appliedSpeedRatio, const TimeIntervalCollection& parentIntervalCollection);
    bool ComputeExpirationTime(const Duration& duration, float appliedSpeedRatio, Nullable<TimeSpan>* pExpirationTime) const;
    bool ComputeInteractiveValues(float* pAppliedSpeedRatio);
    void ComputeIntervalsWithParentIntersection(const Duration& duration, float appliedSpeedRatio, const TimeIntervalCollection& parentIntervalCollection, const TimeIntervalCollection& activePeriod, const Nullable<TimeSpan>& endOfActivePeriod, const Duration& postFillDuration);
    void ComputeIntervalsWithHoldEnd(const Duration& duration, float appliedSpeedRatio, const TimeIntervalCollection& parentIntervalCollection, const Nullable<TimeSpan>& endOfActivePeriod);
    
 

    bool ComputeNextTickNeededTime(const Nullable<TimeSpan>& expirationTime, const TimeSpan& parentTime, float parentSpeed);
    bool ComputeParentParameters(Nullable<TimeSpan>* pParentTime, Nullable<float>* pParentSpeed, TimeIntervalCollection* pParentIntervalCollection);
    void ComputeSyncEnter(TimeIntervalCollection* pParentIntervalCollection, const TimeSpan& currentParentTime, float appliedSpeedRatio, const Duration& duration, bool hadPendingSeekDestination);
    void ComputeSyncSlip(TimeIntervalCollection* pParentIntervalCollection, const TimeSpan& currentParentTime, float currentParentSpeed, float appliedSpeedRatio);
    bool ComputeCurrentTime(const Duration& duration, const TimeSpan& localProgress, float* pLocalSpeed);
    void AdjustBeginTime(float* pAppliedSpeedRatio);

    Nullable<TimeSpan> m_beginTime;
    ClockState::Enum m_currentClockState;
    Nullable<float> m_currentGlobalSpeed;
    Nullable<int> m_currentIteration;
    Nullable<float> m_currentProgress;
    Nullable<TimeSpan> m_currentTime;
    int m_depth;

    unsigned int m_flags;
    float m_interactiveSpeedRatio;
    Nullable<TimeSpan> m_nextTickNeededTime;
    Nullable<TimeSpan> m_pendingSeekDestination;
    Nullable<float> m_pendingSpeedRatio;
    TRefCountedPtr<Timeline> m_pTimeline;
    TimeManager* m_pTimeManager;
    ClockGroup* m_pParent;
    float m_accelerationRatio;
    float m_decelerationRatio;
    Duration m_duration;
    RepeatBehavior m_repeatBehavior;
    FillBehavior::Enum m_fillBehavior;
    float m_speedRatio;
};

typedef TRefCountedPtr<Clock> ClockPtr;
typedef TWeakPtr<Clock> ClockWeakPtr;

};
