#include <AVUICommon.h>
#include <AVUITimeline.h>
#include <AVUIClock.h>
#include <AVUIClockGroup.h>
#include <AVUIDispatcher.h>
#include <AVUITimeManager.h>
#include <AVUIPrefixSubtreeEnumerator.h>

REGISTER_ELEMENT_BEGIN(AVUI::Clock)

REGISTER_ELEMENT_END()

namespace AVUI {

Clock::Clock(Timeline* pTimeline)
{
    m_flags = 0;
    m_interactiveSpeedRatio = 1.0f;
    m_pTimeline = pTimeline;
    m_pTimeManager = NULL;
    m_pSyncData = NULL;
    m_pParent = NULL;
    set_Flag(ClockFlags::NeedsTicksWhenActive, true);
    set_AutoReverse(pTimeline->get_AutoReverse());
    m_accelerationRatio = pTimeline->get_AccelerationRatio();
    m_decelerationRatio = pTimeline->get_DecelerationRatio();
    m_duration = pTimeline->get_Duration();
    m_repeatBehavior = pTimeline->get_RepeatBehavior();
    m_fillBehavior = pTimeline->get_FillBehavior();
    m_speedRatio = pTimeline->get_SpeedRatio();

    m_beginTime = pTimeline->get_BeginTime();
    m_currentClockState = ClockState::Stopped;
    if(m_beginTime.get_HasValue())
    {
        m_nextTickNeededTime = Nullable<TimeSpan>(TimeSpan::get_Zero());
    }
}

ClockPtr Clock::BuildClockTreeFromTimeline(Timeline* pRootTimeline, bool hasControllableRoot)
{
      ClockPtr pClock = Clock::AllocateClock(pRootTimeline, hasControllableRoot);
      pClock->BuildClockSubTreeFromTimeline(pClock->get_Timeline(), hasControllableRoot);

      pClock->AddToRoot();

      return pClock;
}


ClockPtr Clock::AllocateClock(Timeline* pTimeline, bool hasControllableRoot)
{
      ClockPtr pClock = pTimeline->AllocateClock();

      pClock->set_Flag(ClockFlags::HasControllableRoot, hasControllableRoot);
                                           
      return pClock;
}

Duration Clock::get_NaturalDuration() const { return m_pTimeline->get_NaturalDuration(this); }

bool Clock::ComputeDuration(Duration* pDuration) const
{
    *pDuration = m_duration;

    if(*pDuration == Duration::get_Automatic())
    {
        *pDuration = get_NaturalDuration();
        if(*pDuration == Duration::get_Automatic())
        {
            *pDuration = Duration::get_Forever();
        }
    }

    return false;
}

void Clock::BuildClockSubTreeFromTimeline(Timeline* pTimeline, bool hasControllableRoot)
{
    set_Flag(ClockFlags::CanSlip, get_CanSlip());

    if(get_CanSlip() && m_pTimeline->get_BeginTime().get_HasValue())
    {
        Duration duration;
        ComputeDuration(&duration);

        if(!duration.get_HasTimeSpan() || duration.get_TimeSpan() > TimeSpan::get_Zero())
        {
            if(get_AutoReverse() || get_AccelerationRatio() > 0.0f || get_DecelerationRatio() > 0.0f)
            {
                // Can slip only on simple timelines.
                LibraryCriticalError();
            }
            m_pSyncData = SyncDataPtr(new SyncData(this), false);
        }
    }
}

void Clock::AddToRoot()
{
    TimeManager* m_pTimeManager = Dispatcher::get_CurrentDispatcher()->get_TimeManager();

    m_pParent = m_pTimeManager->get_RootClock();

    set_TimeManager(m_pParent->get_TimeManager());
    m_pParent->get_InternalRootChildren().AddItem(this);

    PrefixSubtreeEnumerator enumerator(this, true);
    while(enumerator.MoveNext())
    {
        Clock* pClock = enumerator.get_Current();
        pClock->m_depth = pClock->m_pParent->m_depth + 1;
        pClock->set_TimeManager(m_pTimeManager);
    }
    if(get_IsInTimingTree())
    {
        m_pTimeManager->set_Dirty();
    }

    TimeIntervalCollection collection = TimeIntervalCollection::CreatePoint(m_pTimeManager->get_InternalCurrentGlobalTime());
    collection.AddNullPoint();
    m_pTimeManager->set_InternalCurrentIntervals(collection);
    m_beginTime = Nullable<TimeSpan>();
    enumerator.Reset();
    while(enumerator.MoveNext())
    {
        Clock* pClock = enumerator.get_Current();
        pClock->ComputeLocalState();
        pClock->ClipNextTickByParent();
        pClock->set_NeedsPostfixTraversal(pClock->IsType(GET_TYPE(ClockGroup)));
    }
    m_pParent->ComputeTreeStateRoot();
    if(m_pTimeline->get_BeginTime().get_HasValue())
    {
        set_RootBeginPending(true);
    }
    NotifyNewEarliestFutureActivity();
}

TimeSpan Clock::MultiplyTimeSpan(const TimeSpan& timeSpan, float factor)
{
    return TimeSpan::FromTicks((long) ((factor * timeSpan.get_Ticks()) + 0.5));
}

bool Clock::ComputeSpeedRatio(float *pAppliedSpeedRatio) const
{
      if (m_interactiveSpeedRatio == 0.0f)
      {
            *pAppliedSpeedRatio = get_SpeedRatio();
      }
      else
      {
            *pAppliedSpeedRatio = get_SpeedRatio() * m_interactiveSpeedRatio;
      }
      return false;
}


Nullable<TimeSpan> Clock::ComputeEffectiveDuration(const Duration& duration, float appliedSpeedRatio) const
{
    RepeatBehavior repeatBehavior = get_RepeatBehavior();

    if(duration.get_HasTimeSpan() && duration.get_TimeSpan() == TimeSpan::get_Zero())
    {
        return Nullable<TimeSpan>(TimeSpan::get_Zero());
    }
    if(repeatBehavior.get_HasCount())
    {
        if(repeatBehavior.get_Count() == 0)
        {
            return Nullable<TimeSpan>(TimeSpan::get_Zero());
        }
        if(duration == Duration::get_Forever())
        {
            return Nullable<TimeSpan>();
        }
        float length = repeatBehavior.get_Count() / appliedSpeedRatio;
        if(get_AutoReverse())
        {
            length *= 2.0f;
        }
        return Nullable<TimeSpan>(Clock::MultiplyTimeSpan(duration.get_TimeSpan(), length));
    }

    if(repeatBehavior.get_HasDuration())
    {
        return Nullable<TimeSpan>(repeatBehavior.get_Duration());
    }
    return Nullable<TimeSpan>();
}


Clock::SyncData::SyncData(Clock* pClock)
{
    m_syncClockDuration = Duration::get_Automatic();
    m_pSyncClock = pClock;
    m_syncClockBeginTime = pClock->get_Timeline()->get_BeginTime().get_Value();
    m_pSyncClock->ComputeSpeedRatio(&m_syncClockSpeedRatio);

    Duration duration = get_SyncClockDuration();
    m_syncClockEffectiveDuration = m_pSyncClock->ComputeEffectiveDuration(duration, 1);
}

bool Clock::get_IsInTimingTree() const
{
    if(m_pTimeManager != NULL)
    {
        return m_pTimeManager->get_State() != TimeState::Stopped;
    }
    return false;
}

void Clock::ClipNextTickByParent()
{
    if (!get_IsTimeManager() && !m_pParent->get_IsTimeManager() && 
        (!get_InternalNextTickNeededTime().get_HasValue() || 
         (m_pParent->get_InternalNextTickNeededTime().get_HasValue() && 
          (m_pParent->get_InternalNextTickNeededTime().get_Value() < get_InternalNextTickNeededTime().get_Value())
         )
        )
       )
    {
        set_InternalNextTickNeededTime(m_pParent->get_InternalNextTickNeededTime());
    }
}

void Clock::ComputeLocalState()
{
      ClockState::Enum state = m_currentClockState;
      Nullable<TimeSpan> currentTime = m_currentTime;
      Nullable<float> currentGlobalSpeed = m_currentGlobalSpeed;
      Nullable<float> currentProgress = m_currentProgress;
      Nullable<int> currentIteration = m_currentIteration;

      set_PauseStateChangeDuringTick(false);

      ComputeLocalStateHelper(true);

      if (state != m_currentClockState)
      {
            RaiseCurrentStateInvalidated();
            RaiseCurrentGlobalSpeedInvalidated();
            RaiseCurrentTimeInvalidated();
      }

      Nullable<float> newGlobalSpeed = m_currentGlobalSpeed;
      if ((newGlobalSpeed.get_ValueOrDefault() != currentGlobalSpeed.get_ValueOrDefault()) || (newGlobalSpeed.get_HasValue() != currentGlobalSpeed.get_HasValue()))
      {
            RaiseCurrentGlobalSpeedInvalidated();
      }
      if (currentProgress.get_HasValue())
      {
            ComputeExpectedProgress(currentTime, currentProgress, currentIteration);
      }
}

 
void Clock::ComputeExpectedProgress(const Nullable<TimeSpan>& lastCurrentTime, const Nullable<float>& lastCurrentProgress, 
                                    const Nullable<int>& lastCurrentIteration)
{
    if(get_CurrentGlobalSpeedInvalidatedEventRaised() || get_CurrentStateInvalidatedEventRaised())
    {
        return;
    }

    if(!lastCurrentTime.get_HasValue() || !m_currentTime.get_HasValue() || lastCurrentProgress.get_Value() == 0.0f)
    {
        return;
    }

    if (lastCurrentIteration.get_ValueOrDefault() != m_currentIteration.get_ValueOrDefault() || lastCurrentIteration.get_HasValue() != m_currentIteration.get_HasValue())
    {
          DiscontinuousTimeMovement();
    }
    else
    {
          Duration duration;

          ComputeDuration(&duration);

          if (duration != Duration::get_Forever())
          {
                float ratio = get_AccelerationRatio() + get_DecelerationRatio();
                float modifier = 2 / (2 - ratio);
                float numTicks = (float) duration.get_TimeSpan().get_Ticks();
                float scale = ((m_currentTime.get_Value().get_Ticks() - lastCurrentTime.get_Value().get_Ticks()) / numTicks) * modifier;
                if ((scale > 1) || (Float::Abs((float) (m_currentProgress.get_Value() - lastCurrentProgress.get_Value() - scale)) > 0.001))
                {
                      DiscontinuousTimeMovement();
                }
          }
    }
}
 



void Clock::NotifyNewEarliestFutureActivity()
{

    PrefixSubtreeEnumerator enumerator(this, false);
    while (enumerator.MoveNext())
    {
          enumerator.get_Current()->set_InternalNextTickNeededTime(Nullable<TimeSpan>(TimeSpan::get_Zero()));
    }

    Clock* pClock = this;

    while(pClock != NULL)
    {
        Nullable<TimeSpan> timeSpan = pClock->get_InternalNextTickNeededTime();
        if(!timeSpan.get_HasValue() || timeSpan.get_ValueOrDefault() != TimeSpan::get_Zero())
        {
            pClock->set_InternalNextTickNeededTime(Nullable<TimeSpan>(TimeSpan::get_Zero()));
            if(pClock->get_IsTimeManager())
            {
                m_pTimeManager->NotifyNewEarliestFutureActivity();
                break;
            }
        }
        pClock = pClock->m_pParent;
    }

}

// Review this -- Does this even work?
Clock* Clock::get_NextSibling()
{
    if(m_pParent == NULL)
    {
        return NULL;
    }
    ItemStructList<ClockPtr>& children = m_pParent->get_Children();

    int index = children.IndexOf(this);
    if(index == (int) (children.get_Count() - 1))
    {
        return NULL;
    }

    return children[index + 1];
}

void Clock::ComputeLocalStateHelper(bool performTickOperations)
{
      float appliedSpeedRatio;
      Duration duration;
      Nullable<TimeSpan> expirationTime;
      TimeIntervalCollection parentIntervalCollection;
      Nullable<float> parentSpeed;
      Nullable<TimeSpan> parentTime;
      bool isInvalid = false;


      if (ComputeParentParameters(&parentTime, &parentSpeed, &parentIntervalCollection))
      {
            isInvalid = true;
      }
      if (ComputeSpeedRatio(&appliedSpeedRatio))
      {
            isInvalid = true;
      }
      if (m_pSyncData != NULL && m_pSyncData->get_IsInSyncPeriod() && m_pParent->get_CurrentState() != ClockState::Stopped)
      {
            ComputeSyncSlip(&parentIntervalCollection, parentTime.get_Value(), parentSpeed.get_Value(), appliedSpeedRatio);
      }
      if (ComputeDuration(&duration))
      {
            isInvalid = true;
      }
      bool isSeekPending = m_pendingSeekDestination.get_HasValue();
      if (performTickOperations && m_pParent->get_IsTimeManager() && ComputeInteractiveValues(&appliedSpeedRatio))
      {
            isInvalid = true;
      }
      if (m_pSyncData != NULL && !m_pSyncData->get_IsInSyncPeriod() && m_pParent->get_CurrentState() != ClockState::Stopped)
      {
            ComputeSyncEnter(&parentIntervalCollection, parentTime.get_Value(), appliedSpeedRatio, duration, isSeekPending);
      }
      if (ComputeExpirationTime(duration, appliedSpeedRatio, &expirationTime))
      {
            isInvalid = true;
      }
      if (performTickOperations)
      {
            ComputeEvents(expirationTime, duration, appliedSpeedRatio, parentIntervalCollection);
      }
      if (!isInvalid)
      {
            float localSpeed;
            TimeSpan localProgress;
            TimeSpan parentTimeValue = parentTime.get_Value();
            if (!ComputeNextTickNeededTime(expirationTime, parentTimeValue, parentSpeed.get_Value()) && !ComputeCurrentState(expirationTime, &parentTimeValue, parentSpeed.get_Value()) && !ComputeCurrentIteration(parentTimeValue, parentSpeed.get_Value(), duration, expirationTime, appliedSpeedRatio, &localProgress) && !ComputeCurrentTime(duration, localProgress, &localSpeed) && ComputeCurrentSpeed(localSpeed, appliedSpeedRatio))
            {
            }
      }
}

bool Clock::ComputeCurrentSpeed(float localSpeed, float appliedSpeedRatio)
{
      if (get_IsInteractivelyPaused())
      {
            m_currentGlobalSpeed = Nullable<float>(0.0f);
      }
      else
      {
            localSpeed *= appliedSpeedRatio;
            if (get_IsBackwardsProgressingGlobal())
            {
                  localSpeed = -localSpeed;
            }
            Nullable<float> parentGlobalSpeed = m_pParent->m_currentGlobalSpeed;
            m_currentGlobalSpeed = parentGlobalSpeed.get_HasValue() ? Nullable<float>(localSpeed * parentGlobalSpeed.get_ValueOrDefault()) : Nullable<float>();
      }
      return false;
}

 
bool Clock::ComputeCurrentTime(const Duration& duration, const TimeSpan& localProgress, float* pLocalSpeed)
{
      if (duration.get_HasTimeSpan())
      {
            float accelerationRatio = get_AccelerationRatio();
            float decelerationRatio = get_DecelerationRatio();
            float speedRatio = accelerationRatio + decelerationRatio;
            float ticks = (float) (duration.get_TimeSpan().get_Ticks());
            float percentCompleted = ((float) localProgress.get_Ticks()) / ticks;
            if (speedRatio == 0.0f)
            {
                  *pLocalSpeed = 1.0f;
                  m_currentTime = Nullable<TimeSpan>(localProgress);
            }
            else
            {
                  float speedRatioDivisor = 2.0f / (2.0f - speedRatio);
                  if (percentCompleted < accelerationRatio)
                  {
                        *pLocalSpeed = (speedRatioDivisor * percentCompleted) / accelerationRatio;
                        percentCompleted = ((speedRatioDivisor * percentCompleted) * percentCompleted) / (2.0f * accelerationRatio);
                        set_InternalNextTickNeededTime(Nullable<TimeSpan>(TimeSpan::get_Zero()));
                  }
                  else if (percentCompleted <= (1.0f - decelerationRatio))
                  {
                        *pLocalSpeed = speedRatioDivisor;
                        percentCompleted = speedRatioDivisor * (percentCompleted - (accelerationRatio / 2.0f));
                  }
                  else
                  {
                        float percentRemaining = 1.0f - percentCompleted;
                        *pLocalSpeed = (speedRatioDivisor * percentRemaining) / decelerationRatio;
                        percentCompleted = 1.0f - (((speedRatioDivisor * percentRemaining) * percentRemaining) / (2.0f * decelerationRatio));
                        set_InternalNextTickNeededTime(Nullable<TimeSpan>(TimeSpan::get_Zero()));
                  }
                  m_currentTime = Nullable<TimeSpan>(TimeSpan::FromTicks((int) ((percentCompleted * ticks) + 0.5f)));
            }
            m_currentProgress = Nullable<float>(percentCompleted);
      }
      else
      {
            m_currentTime = Nullable<TimeSpan>(localProgress);
            m_currentProgress = Nullable<float>(0.0f);
            *pLocalSpeed = 1.0f;
      }
      return (m_currentClockState != ClockState::Active);

}

void Clock::ComputeSyncSlip(TimeIntervalCollection* pParentIntervalCollection, const TimeSpan& currentParentTime, float currentParentSpeed, float appliedSpeedRatio)
{
      TimeSpan timeInto = Clock::MultiplyTimeSpan(currentParentTime - pParentIntervalCollection->get_FirstNodeTime(), appliedSpeedRatio);
      TimeSpan currentTime = m_pSyncData->get_SyncClock()->get_CurrentTimeCore();
      TimeSpan diffFromLastSync = currentTime - m_pSyncData->get_PreviousSyncClockTime();
      if (diffFromLastSync > TimeSpan::get_Zero())
      {
            Nullable<TimeSpan> effectiveDuration = m_pSyncData->get_SyncClockEffectiveDuration();
            if (effectiveDuration.get_HasValue() && (m_pSyncData->get_PreviousRepeatTime() + currentTime) >= effectiveDuration.get_Value())
            {
                  m_pSyncData->set_IsInSyncPeriod(false);
                  m_pSyncData->set_PreviousRepeatTime(TimeSpan::get_Zero());
            }
            Duration syncClockDuration = m_pSyncData->get_SyncClockDuration();
            if (syncClockDuration.get_HasTimeSpan() && (currentTime == syncClockDuration.get_TimeSpan()))
            {
                  m_pSyncData->set_PreviousSyncClockTime(TimeSpan::get_Zero());
                  m_pSyncData->set_PreviousRepeatTime(m_pSyncData->get_PreviousRepeatTime() + syncClockDuration.get_TimeSpan());
            }
            else
            {
                  m_pSyncData->set_PreviousSyncClockTime(currentTime);
            }
      }
      else
      {
            diffFromLastSync = TimeSpan::get_Zero();
      }
      if (m_pSyncData->get_SyncClock()->m_pParent == this)
      {
            diffFromLastSync = DivideTimeSpan(diffFromLastSync, m_pSyncData->get_SyncClockSpeedRatio());
      }
      TimeSpan timeDiff = timeInto - diffFromLastSync;
      Nullable<TimeSpan> nullable2 = m_beginTime;

      m_beginTime = m_beginTime.get_HasValue() ? Nullable<TimeSpan>(m_beginTime.get_ValueOrDefault() + timeDiff) : Nullable<TimeSpan>();
      TimeSpan slipTime = DivideTimeSpan(timeDiff, appliedSpeedRatio);
      *pParentIntervalCollection = pParentIntervalCollection->SlipBeginningOfConnectedInterval(slipTime);
}

TimeSpan Clock::DivideTimeSpan(const TimeSpan& timeSpan, float factor)
{
    return TimeSpan::FromTicks( (int) (((timeSpan.get_Ticks() * 1.0f) / factor) + 0.5));
}

 
bool Clock::ComputeParentParameters(Nullable<TimeSpan>* pParentTime, Nullable<float>* pParentSpeed, TimeIntervalCollection* pParentIntervalCollection)
{
    if (m_pParent->get_IsTimeManager())
    {
          if ((m_pTimeManager == NULL) || m_pTimeManager->get_InternalIsStopped())
          {
                ResetCachedStateToStopped();
                *pParentTime = Nullable<TimeSpan>();
                *pParentSpeed = Nullable<float>();
                set_InternalNextTickNeededTime(Nullable<TimeSpan>(TimeSpan::get_Zero()));
                *pParentIntervalCollection = TimeIntervalCollection::get_Empty();
                return true;
          }
          *pParentTime = Nullable<TimeSpan>(m_pTimeManager->get_InternalCurrentGlobalTime());
          *pParentSpeed = Nullable<float>(1.0f);
          *pParentIntervalCollection = m_pTimeManager->get_InternalCurrentIntervals();
          return false;
    }
    *pParentTime = m_pParent->m_currentTime;
    *pParentSpeed = m_pParent->m_currentGlobalSpeed;
    *pParentIntervalCollection = m_pParent->get_CurrentIntervals();
    if (m_pParent->m_currentClockState != ClockState::Stopped)
    {
          return false;
    }
    if (m_currentClockState != ClockState::Stopped)
    {
          RaiseCurrentStateInvalidated();
          RaiseCurrentGlobalSpeedInvalidated();
          RaiseCurrentTimeInvalidated();
    }

    ResetCachedStateToStopped();

    set_InternalNextTickNeededTime(Nullable<TimeSpan>());

    return true;
}

void Clock::ResetCachedStateToStopped()
{
    m_currentGlobalSpeed = Nullable<float>();
    m_currentIteration = Nullable<int>();
    set_IsBackwardsProgressingGlobal(false);
    m_currentProgress = Nullable<float>();
    m_currentTime = Nullable<TimeSpan>();
    m_currentClockState = ClockState::Stopped;

}

TimeSpan Clock::get_CurrentGlobalTime()
{
    if (m_pTimeManager == NULL)
    {
        return TimeSpan::get_Zero();
    }
    return m_pTimeManager->get_InternalCurrentGlobalTime();
}
 
bool Clock::ComputeCurrentIteration(const TimeSpan& parentTime, float parentSpeed, const Duration& duration, const Nullable<TimeSpan>& expirationTime, float appliedSpeedRatio, TimeSpan* pLocalProgress)
{
      TimeSpan span5 = Clock::MultiplyTimeSpan(parentTime - m_beginTime.get_Value(), appliedSpeedRatio);
      set_IsBackwardsProgressingGlobal(m_pParent->get_IsBackwardsProgressingGlobal());
      if (duration.get_HasTimeSpan())
      {
            if (duration.get_TimeSpan() == TimeSpan::get_Zero())
            {
                  float num1;
                  *pLocalProgress = TimeSpan::get_Zero();
                  m_currentTime = Nullable<TimeSpan>(TimeSpan::get_Zero());
                  const RepeatBehavior& repeatBehavior = get_RepeatBehavior();
                  if (repeatBehavior.get_HasCount())
                  {
                        float num3 = repeatBehavior.get_Count();
                        if (num3 <= 1)
                        {
                              num1 = num3;
                              m_currentIteration = Nullable<int>(1);
                        }
                        else
                        {
                              float num4 = (float) (int) num3;
                              if (num3 == num4)
                              {
                                    num1 = 1;
                                    m_currentIteration = Nullable<int>((int) num3);
                              }
                              else
                              {
                                    num1 = num3 - num4;
                                    m_currentIteration = Nullable<int>((int) (num3 + 1));
                              }
                        }
                  }
                  else
                  {
                        m_currentIteration = Nullable<int>(1);
                        num1 = 1;
                  }
                  if (get_AutoReverse())
                  {
                        if (num1 == 1.0f)
                        {
                              num1 = 0.0f;
                        }
                        else if (num1 < 0.5f)
                        {
                              num1 *= 2.0f;
                        }
                        else
                        {
                              num1 = 1.0f - ((num1 - 0.5f) * 2.0f);
                        }
                  }
                  m_currentProgress = Nullable<float>(num1);
                  return true;
            }

            int durationTicks = duration.get_TimeSpan().get_Ticks() + 1; // Addition of 1 prevents inappropriate rollover

            *pLocalProgress = TimeSpan::FromTicks(span5.get_Ticks() % durationTicks);
            int num2 = span5.get_Ticks() / durationTicks;
            if (*pLocalProgress == TimeSpan::get_Zero() && num2 > 0 && (m_currentClockState == ClockState::Filling || m_pParent->get_IsBackwardsProgressingGlobal()))
            {
                  *pLocalProgress = duration.get_TimeSpan();
                  num2--;
            }
            if (get_AutoReverse())
            {
                  if ((num2 & 1) == 1)
                  {
                        if (*pLocalProgress == TimeSpan::get_Zero())
                        {
                              set_InternalNextTickNeededTime(Nullable<TimeSpan>(TimeSpan::get_Zero()));
                        }
                        *pLocalProgress = duration.get_TimeSpan() - *pLocalProgress;
                        set_IsBackwardsProgressingGlobal(!get_IsBackwardsProgressingGlobal());
                        parentSpeed = -parentSpeed;
                  }
                  num2 /= 2;
            }
            m_currentIteration = Nullable<int>(1 + num2);
            if (((m_currentClockState == ClockState::Active) && (parentSpeed != 0)) && !get_NeedsTicksWhenActive())
            {
                  TimeSpan span1;
                  if (*pLocalProgress == TimeSpan::get_Zero())
                  {
                        span1 = DivideTimeSpan(duration.get_TimeSpan(), Float::Abs(parentSpeed));
                  }
                  else if (parentSpeed > 0)
                  {
                        TimeSpan span4 = Clock::MultiplyTimeSpan(duration.get_TimeSpan(), 1 - get_DecelerationRatio());
                        span1 = DivideTimeSpan(span4 - *pLocalProgress, parentSpeed);
                  }
                  else
                  {
                        TimeSpan span3 = Clock::MultiplyTimeSpan(duration.get_TimeSpan(), get_AccelerationRatio());
                        span1 = DivideTimeSpan(span3 - *pLocalProgress, parentSpeed);
                  }
                  TimeSpan span2 = get_CurrentGlobalTime() + span1;
                  if (!get_InternalNextTickNeededTime().get_HasValue() || (span2 < get_InternalNextTickNeededTime().get_Value()))
                  {
                        set_InternalNextTickNeededTime(Nullable<TimeSpan>(span2));
                  }
            }
      }
      else
      {
            *pLocalProgress = span5;
            m_currentIteration = Nullable<int>(1);
      }
      return false;
}

// Computes the current state of the animation, stopping it when expire time is over.
bool Clock::ComputeCurrentState(const Nullable<TimeSpan>& expirationTime, TimeSpan* pParentTime, float parentSpeed)
{
      FillBehavior::Enum fillBehavior = get_FillBehavior();
      TimeSpan span2 = *pParentTime;
      if (m_beginTime.get_HasValue() ? (span2 < m_beginTime.get_ValueOrDefault()) : false)
      {
            ResetCachedStateToStopped();
            return true;
      }
      if (expirationTime.get_HasValue())
      {
            TimeSpan span1 = *pParentTime;
            if (span1 >= expirationTime.get_ValueOrDefault())
            {
                  RaiseCompletedForRoot();

                  if (fillBehavior == FillBehavior::HoldEnd)
                  {
                        ResetCachedStateToFilling();
                        *pParentTime = expirationTime.get_Value();
                  }
                  else
                  {
                      ResetCachedStateToStopped();
                      return true;
                  }
            }
            else
            {
                m_currentClockState = ClockState::Active;
            }
      }
      else
      {
          m_currentClockState = ClockState::Active;
      }

      if (parentSpeed != 0 && m_currentClockState == ClockState::Active && get_NeedsTicksWhenActive())
      {
            set_InternalNextTickNeededTime(Nullable<TimeSpan>(TimeSpan::get_Zero()));
      }
      return false;
}

bool Clock::ComputeNextTickNeededTime(const Nullable<TimeSpan>& expirationTime, const TimeSpan& parentTime, float parentSpeed)
{
      if (parentSpeed == 0)
      {
            set_InternalNextTickNeededTime(get_IsInteractivelyPaused() ? Nullable<TimeSpan>(TimeSpan::get_Zero()) : Nullable<TimeSpan>());
      }
      else
      {
            float parentSpeedRatio = 1.0f / parentSpeed;
            Nullable<TimeSpan> nullable3 = Nullable<TimeSpan>();
            TimeSpan span3 = Clock::MultiplyTimeSpan(m_beginTime.get_Value() - parentTime, parentSpeedRatio);
            if (span3 >= TimeSpan::get_Zero())
            {
                  nullable3 = Nullable<TimeSpan>(span3);
            }
            if (expirationTime.get_HasValue())
            {
                  TimeSpan span2 = Clock::MultiplyTimeSpan(expirationTime.get_Value() - parentTime, parentSpeedRatio);
                  if ((span2 >= TimeSpan::get_Zero()) && (!nullable3.get_HasValue() || (span2 < nullable3.get_Value())))
                  {
                        nullable3 = Nullable<TimeSpan>(span2);
                  }
            }
            if (nullable3.get_HasValue())
            {
                  TimeSpan span4 = get_CurrentGlobalTime();
                  Nullable<TimeSpan> nullable5 = nullable3;
                  set_InternalNextTickNeededTime(nullable5.get_HasValue() ? Nullable<TimeSpan>(span4 + nullable5.get_ValueOrDefault()) : Nullable<TimeSpan>());
            }
            else
            {
                  set_InternalNextTickNeededTime(Nullable<TimeSpan>());
            }
      }
      return false;
}


void Clock::RaiseCompletedForRoot()
{
    if (m_pParent->get_IsTimeManager() && get_CurrentStateInvalidatedEventRaised())
    {
        PrefixSubtreeEnumerator enumerator(this, true);
        while (enumerator.MoveNext())
        {
              enumerator.get_Current()->RaiseCompleted();
        }
    }
}

void Clock::RaiseCompleted()
{
    set_CompletedEventRaised(true);

    if (!get_IsInEventQueue())
    {
        m_pTimeManager->AddToEventQueue(this);
        set_IsInEventQueue(true);
    }
}

void Clock::RaiseCurrentStateInvalidated()
{
    if (m_currentClockState == ClockState::Stopped)
    {
        Stopped();
    }
    set_CurrentStateInvalidatedEventRaised(true);
    if (!get_IsInEventQueue())
    {
        m_pTimeManager->AddToEventQueue(this);
        set_IsInEventQueue(true);
    }
}
void Clock::RaiseCurrentGlobalSpeedInvalidated()
{
    set_CurrentGlobalSpeedInvalidatedEventRaised(true);
    if (!get_IsInEventQueue())
    {
        m_pTimeManager->AddToEventQueue(this);
        set_IsInEventQueue(true);
    }
}
void Clock::RaiseCurrentTimeInvalidated()
{
    set_CurrentTimeInvalidatedEventRaised(true);
    if (!get_IsInEventQueue())
    {
        m_pTimeManager->AddToEventQueue(this);
        set_IsInEventQueue(true);
    }
}
void Clock::RaiseRemoveRequested()
{
    set_RemoveRequestedEventRaised(true);
    if (!get_IsInEventQueue())
    {
        m_pTimeManager->AddToEventQueue(this);
        set_IsInEventQueue(true);
    }
}

void Clock::RaiseRemoveRequestedForRoot()
{
    PrefixSubtreeEnumerator enumerator(this, true);
    while (enumerator.MoveNext())
    {
          enumerator.get_Current()->RaiseRemoveRequested();
    }
}

// Needs additional work for compliance
void Clock::RaiseAccumulatedEvents()
{
    if(get_CurrentTimeInvalidatedEventRaised())
    {
        invoke_CurrentTimeInvalidated(this, NULL);
    }

    if(get_CurrentStateInvalidatedEventRaised())
    {
        invoke_CurrentStateInvalidated(this, NULL);
    }

    set_CurrentTimeInvalidatedEventRaised(false);
    set_CurrentGlobalSpeedInvalidatedEventRaised(false);
    set_CurrentStateInvalidatedEventRaised(false);
    set_CompletedEventRaised(false);
    set_RemoveRequestedEventRaised(false);
    set_IsInEventQueue(false);

}

void Clock::ResetCachedStateToFilling()
{
    m_currentGlobalSpeed = Nullable<float>(0.0f);
    set_IsBackwardsProgressingGlobal(false);
    m_currentClockState = ClockState::Filling;
}


bool Clock::ComputeExpirationTime(const Duration& duration, float appliedSpeedRatio, Nullable<TimeSpan>* pExpirationTime) const
{
    if (!m_beginTime.get_HasValue())
    {
          *pExpirationTime = Nullable<TimeSpan>();
          return true;
    }
    Nullable<TimeSpan> nullable2 = ComputeEffectiveDuration(duration, appliedSpeedRatio);
    if (nullable2.get_HasValue())
    {
          Nullable<TimeSpan> nullable4 = m_beginTime;
          Nullable<TimeSpan> nullable3 = nullable2;
          *pExpirationTime = (nullable4.get_HasValue() & nullable3.get_HasValue()) ? Nullable<TimeSpan>(nullable4.get_ValueOrDefault() + nullable3.get_ValueOrDefault()) : Nullable<TimeSpan>();
    }
    else
    {
          *pExpirationTime = Nullable<TimeSpan>();
    }
    return false;
}


bool Clock::ComputeInteractiveValues(float* pAppliedSpeedRatio)
{
    bool isStopped = false;
    if (get_PendingInteractiveStop())
    {
          set_PendingInteractiveStop(false);
          set_IsInteractivelyStopped(true);
          m_beginTime = Nullable<TimeSpan>();
          PrefixSubtreeEnumerator enumerator = PrefixSubtreeEnumerator(this, true);
          while (enumerator.MoveNext())
          {
                Clock* pClock = enumerator.get_Current();
                if (pClock->m_currentClockState != ClockState::Stopped)
                {
                      pClock->ResetCachedStateToStopped();
                      pClock->RaiseCurrentStateInvalidated();
                      pClock->RaiseCurrentTimeInvalidated();
                      pClock->RaiseCurrentGlobalSpeedInvalidated();
                }
                else
                {
                      enumerator.SkipSubtree();
                }
          }
    }
    if (get_IsInteractivelyStopped())
    {
          ResetCachedStateToStopped();
          set_InternalNextTickNeededTime(Nullable<TimeSpan>());
          isStopped = true;
    }
    else
    {
          AdjustBeginTime(pAppliedSpeedRatio);
    }
    if (get_PendingInteractivePause())
    {
          set_PendingInteractivePause(false);
          RaiseCurrentGlobalSpeedInvalidated();
          PrefixSubtreeEnumerator enumerator = PrefixSubtreeEnumerator(this, true);
          while (enumerator.MoveNext())
          {
                enumerator.get_Current()->set_IsInteractivelyPaused(true);
                enumerator.get_Current()->set_PauseStateChangeDuringTick(true);
          }
    }
    if (get_PendingInteractiveResume())
    {
          set_PendingInteractiveResume(false);
          if (m_currentClockState != ClockState::Filling)
          {
                RaiseCurrentGlobalSpeedInvalidated();
          }
          PrefixSubtreeEnumerator enumerator = PrefixSubtreeEnumerator(this, true);
          while (enumerator.MoveNext())
          {
                enumerator.get_Current()->set_IsInteractivelyPaused(false);
                enumerator.get_Current()->set_PauseStateChangeDuringTick(true);
          }
    }
    return isStopped;

}
 
 
TimeSpan Clock::get_CurrentTimeCore()
{
    if (!m_currentTime.get_HasValue())
    {
        return TimeSpan::get_Zero();
    }
    return m_currentTime.get_Value();
}

void Clock::AdjustBeginTime(float* pAppliedSpeedRatio)
{
    if (m_pendingSeekDestination.get_HasValue())
      {
            m_beginTime = Nullable<TimeSpan>(get_CurrentGlobalTime() - DivideTimeSpan(m_pendingSeekDestination.get_Value(), *pAppliedSpeedRatio));
            m_pendingSeekDestination = Nullable<TimeSpan>();
            PrefixSubtreeEnumerator enumerator = PrefixSubtreeEnumerator(this, true);
            while (enumerator.MoveNext())
            {
                  enumerator.get_Current()->RaiseCurrentStateInvalidated();
                  enumerator.get_Current()->RaiseCurrentTimeInvalidated();
                  enumerator.get_Current()->RaiseCurrentGlobalSpeedInvalidated();
            }
      }
      else if (get_RootBeginPending())
      {
            TimeSpan span3 = get_CurrentGlobalTime();
            Nullable<TimeSpan> nullable4 = m_pTimeline->get_BeginTime();
            m_beginTime = nullable4.get_HasValue() ? Nullable<TimeSpan>(span3 + nullable4.get_ValueOrDefault()) : Nullable<TimeSpan>();
            set_RootBeginPending(false);
      }
      else if ((get_IsInteractivelyPaused() || (m_interactiveSpeedRatio == 0.0f)) && m_beginTime.get_HasValue())
      {
            Nullable<TimeSpan> nullable3 = m_beginTime;
            TimeSpan span2 = m_pTimeManager->get_LastTickDelta();
            m_beginTime = nullable3.get_HasValue() ? Nullable<TimeSpan>(nullable3.get_ValueOrDefault() + span2) : Nullable<TimeSpan>();
      }
      if (m_pendingSpeedRatio.get_HasValue())
      {
            float num1 = m_pendingSpeedRatio.get_Value() * get_SpeedRatio();
            if (num1 == 0.0f)
            {
                  num1 = 1.0f;
            }
            if (m_beginTime.get_HasValue())
            {
                  TimeSpan span1 = get_CurrentGlobalTime();
                  m_beginTime = Nullable<TimeSpan>(span1 - Clock::MultiplyTimeSpan(span1 - m_beginTime.get_Value(), *pAppliedSpeedRatio / num1));
                  RaiseCurrentGlobalSpeedInvalidated();
            }
            *pAppliedSpeedRatio = num1;
            m_interactiveSpeedRatio = m_pendingSpeedRatio.get_Value();
            m_pendingSpeedRatio = Nullable<float>();
      }
}

void Clock::ComputeEvents(const Nullable<TimeSpan>& expirationTime, const Duration& duration, float appliedSpeedRatio, const TimeIntervalCollection& parentIntervalCollection)
{
    TimeIntervalCollection collection1;
    Duration duration1 = TimeSpan::get_Zero();

    ClearCurrentIntervalsToNull();

    bool isPauseHandled = (get_IsInteractivelyPaused() && !get_PauseStateChangeDuringTick()) || (!get_IsInteractivelyPaused() && get_PauseStateChangeDuringTick());

    if(m_beginTime.get_HasValue() || !isPauseHandled)
    {
        Nullable<TimeSpan> nullable1;
        bool flag1 = true;

        nullable1 = expirationTime;
        if (nullable1.get_HasValue())
        {
            duration1 = Duration::get_Forever();
        }

        if (nullable1.get_HasValue())
        {
            Nullable<TimeSpan> nullable7 = nullable1;
            Nullable<TimeSpan> nullable6 = m_beginTime;
            if (!((nullable7.get_HasValue() && nullable6.get_HasValue()) ? (nullable7.get_ValueOrDefault() > nullable6.get_ValueOrDefault()) : false))
            {
                  Nullable<TimeSpan> nullable5 = nullable1;
                  Nullable<TimeSpan> nullable4 = m_beginTime;
                  if (((nullable5.get_HasValue() != nullable4.get_HasValue()) || (nullable5.get_HasValue() && (nullable5.get_ValueOrDefault() != nullable4.get_ValueOrDefault()))) || !flag1)
                  {
                      if (get_PendingInteractiveRemove())
                      {
                            RaiseRemoveRequestedForRoot();
                            RaiseCompletedForRoot();
                            set_PendingInteractiveRemove(false);
                      }
                      return;
                  }
            }
        }
        if (nullable1.get_HasValue())
        {
            Nullable<TimeSpan> nullable3 = nullable1;
            Nullable<TimeSpan> nullable2 = m_beginTime;
            if ((nullable3.get_HasValue() == nullable2.get_HasValue()) && (!nullable3.get_HasValue() || (nullable3.get_ValueOrDefault() == nullable2.get_ValueOrDefault())))
            {
                  collection1 = TimeIntervalCollection::get_Empty();
            }
            else
            {
                  collection1 = TimeIntervalCollection::CreateClosedOpenInterval(m_beginTime.get_Value(), nullable1.get_Value());
            }
        }
        else
        {
            collection1 = TimeIntervalCollection::CreateInfiniteClosedInterval(m_beginTime.get_Value());
        }
        if (parentIntervalCollection.Intersects(collection1))
        {
            ComputeIntervalsWithParentIntersection(duration, appliedSpeedRatio, parentIntervalCollection, collection1, nullable1, duration1);
        }
        else if ((duration1 != ((Duration) TimeSpan::get_Zero())) && (get_FillBehavior() == FillBehavior::HoldEnd))
        {
            ComputeIntervalsWithHoldEnd(duration, appliedSpeedRatio, parentIntervalCollection, nullable1);
        }
    }

    if (get_PendingInteractiveRemove())
    {
        RaiseRemoveRequestedForRoot();
        RaiseCompletedForRoot();
        set_PendingInteractiveRemove(false);
    }
}

void Clock::ComputeIntervalsWithParentIntersection(const Duration& duration, float appliedSpeedRatio, const TimeIntervalCollection& parentIntervalCollection, const TimeIntervalCollection& activePeriod, const Nullable<TimeSpan>& endOfActivePeriod, const Duration& postFillDuration)
{
    RaiseCurrentTimeInvalidated();
    if (parentIntervalCollection.IntersectsInverseOf(activePeriod))
    {
        RaiseCurrentStateInvalidated();
        RaiseCurrentGlobalSpeedInvalidated();
    }
    else if (parentIntervalCollection.IntersectsPeriodicCollection(m_beginTime.get_Value(), duration, appliedSpeedRatio, get_AccelerationRatio(), get_DecelerationRatio(), get_AutoReverse()))
    {
        RaiseCurrentGlobalSpeedInvalidated();
    }

    ComputeCurrentIntervals(parentIntervalCollection, m_beginTime.get_Value(), endOfActivePeriod, postFillDuration, duration, appliedSpeedRatio, get_AccelerationRatio(), get_DecelerationRatio(), get_AutoReverse());
}

void Clock::ComputeIntervalsWithHoldEnd(const Duration& duration, float appliedSpeedRatio, const TimeIntervalCollection& parentIntervalCollection, const Nullable<TimeSpan>& endOfActivePeriod)
{
    TimeIntervalCollection collection1;
    collection1 = TimeIntervalCollection::CreateInfiniteClosedInterval(endOfActivePeriod.get_Value());

    if (parentIntervalCollection.Intersects(collection1))
    {
        ComputeCurrentFillInterval(parentIntervalCollection, m_beginTime.get_Value(), endOfActivePeriod.get_Value(), duration, appliedSpeedRatio, get_AccelerationRatio(), get_DecelerationRatio(), get_AutoReverse());
        if (parentIntervalCollection.IntersectsInverseOf(collection1))
        {
              RaiseCurrentStateInvalidated();
              RaiseCurrentTimeInvalidated();
              RaiseCurrentGlobalSpeedInvalidated();
              AddNullPointToCurrentIntervals();
        }
    }
}

void Clock::ComputeSyncEnter(TimeIntervalCollection* pParentIntervalCollection, const TimeSpan& currentParentTime, float appliedSpeedRatio, const Duration& duration, bool hadPendingSeekDestination)
{
     if (m_beginTime.get_HasValue() && (currentParentTime >= m_beginTime.get_Value()))
      {
            TimeSpan span7 = currentParentTime;
            Nullable<TimeSpan> nullable12 = m_beginTime;
            Nullable<TimeSpan> nullable11 = nullable12.get_HasValue() ? Nullable<TimeSpan>(span7 - nullable12.get_ValueOrDefault()) : Nullable<TimeSpan>();
            TimeSpan span2 = Clock::MultiplyTimeSpan(nullable11.get_Value(), appliedSpeedRatio);
            if ((m_pSyncData->get_SyncClock() == this) || (span2 >= m_pSyncData->get_SyncClockBeginTime()))
            {
                  if (hadPendingSeekDestination)
                  {
                        Nullable<TimeSpan> nullable10;
                        ComputeExpirationTime(duration, appliedSpeedRatio, &nullable10);
                        if (!nullable10.get_HasValue() || (currentParentTime < nullable10.get_Value()))
                        {
                              TimeSpan span3 = (m_pSyncData->get_SyncClock() == this) ? span2 : Clock::MultiplyTimeSpan(span2 - m_pSyncData->get_SyncClockBeginTime(), m_pSyncData->get_SyncClockSpeedRatio());
                              Nullable<TimeSpan> nullable2 = m_pSyncData->get_SyncClockEffectiveDuration();
                              if ((m_pSyncData->get_SyncClock() != this) && nullable2.get_HasValue())
                              {
                                    TimeSpan span6 = span3;
                                    Nullable<TimeSpan> nullable6 = nullable2;
                                    if (!(nullable6.get_HasValue() ? (span6 < nullable6.get_ValueOrDefault()) : false))
                                    {
                                          return;
                                    }
                              }
                              Duration duration1 = m_pSyncData->get_SyncClockDuration();
                              if (duration1.get_HasTimeSpan())
                              {
                                    m_pSyncData->set_PreviousSyncClockTime(TimeSpan::FromTicks(span3.get_Ticks() % duration1.get_TimeSpan().get_Ticks()));
                                    m_pSyncData->set_PreviousRepeatTime(span3 - m_pSyncData->get_PreviousSyncClockTime());
                              }
                              else
                              {
                                    if (duration1 != Duration::get_Forever())
                                    {
                                        // Seek destination ambiguous due to slip.
                                        LibraryCriticalError();
                                    }
                                    m_pSyncData->set_PreviousSyncClockTime(span3);
                                    m_pSyncData->set_PreviousRepeatTime(TimeSpan::get_Zero());
                              }
                              m_pSyncData->set_IsInSyncPeriod(true);
                        }
                  }
                  else
                  {
                        Nullable<TimeSpan> nullable5 = (m_pSyncData->get_SyncClock() == this) ? Nullable<TimeSpan>(pParentIntervalCollection->get_FirstNodeTime()) : m_currentTime;
                        if (!nullable5.get_HasValue() || (nullable5.get_Value() < m_pSyncData->get_SyncClockBeginTime()))
                        {
                              TimeSpan span1 = span2;
                              if (m_pSyncData->get_SyncClock() != this)
                              {
                                    span1 = span1 - m_pSyncData->get_SyncClockBeginTime();
                              }
                              Nullable<TimeSpan> nullable4 = m_beginTime;
                              TimeSpan span5 = span1;
                              m_beginTime = nullable4.get_HasValue() ? Nullable<TimeSpan>(nullable4.get_ValueOrDefault() + span5) : Nullable<TimeSpan>();
                              TimeSpan span4 = DivideTimeSpan(span1, appliedSpeedRatio);
                              *pParentIntervalCollection = pParentIntervalCollection->SlipBeginningOfConnectedInterval(span4);
                              m_pSyncData->set_IsInSyncPeriod(true);
                              m_pSyncData->set_PreviousSyncClockTime(TimeSpan::get_Zero());
                              m_pSyncData->set_PreviousRepeatTime(TimeSpan::get_Zero());
                        }
                  }
            }
      }
}


Nullable<TimeSpan> Clock::get_EndOfActivePeriod() const
{
      Nullable<TimeSpan> nullable1;
      Duration duration1;
      float num1;

      ComputeSpeedRatio(&num1);
      ComputeDuration(&duration1);
      ComputeExpirationTime(duration1, num1, &nullable1);

      return nullable1;
}


}; // namespace AVUI
