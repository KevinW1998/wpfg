#include <AVUICommon.h>
#include <AVUIClockGroup.h>
#include <AVUITimeManager.h>
#include <AVUIParallelTimeline.h>
#include <AVUITimelineCollection.h>
#include <AVUIPrefixSubtreeEnumerator.h>

REGISTER_ELEMENT_BEGIN(AVUI::ClockGroup)

REGISTER_ELEMENT_END()

namespace AVUI {

void ClockGroup::AddNullPointToCurrentIntervals()
{
    m_currentIntervals.AddNullPoint();
}

void ClockGroup::ClearCurrentIntervalsToNull()
{
    m_currentIntervals.Clear();
    m_currentIntervals.AddNullPoint();
}

void ClockGroup::RootCleanChildren()
{
    for(int index = 0; index < (int)m_rootChildren.get_Count(); index++)
    {
        Clock* pClock = m_rootChildren[index];

        if(pClock == NULL)
        {
            m_rootChildren.RemoveAt(index);
            index--;
        }
    }
}

bool ClockGroup::IsEmptyClockTree(Clock* pClock)
{
    PrefixSubtreeEnumerator enumerator(pClock, true);
    while(enumerator.MoveNext())
    {
        if(enumerator.get_Current()->get_RefCount() > 1)
        {
            return false;
        }
    }
    return true;
}

void ClockGroup::ComputeTreeState()
{
    for(int index = 0; index < (int)m_rootChildren.get_Count(); index++)
    {
        Clock* pClock = m_rootChildren[index];

        if(IsEmptyClockTree(pClock))
        {
            m_rootChildren.RemoveAt(index);
            index--;
            continue;
        }

        PrefixSubtreeEnumerator enumerator(pClock, true);
        while(enumerator.MoveNext())
        {
            TimeSpan span1 = get_CurrentGlobalTime();
            Nullable<TimeSpan> nextTickTime = enumerator.get_Current()->get_InternalNextTickNeededTime();
            if (nextTickTime.get_HasValue() ? (span1 >= nextTickTime.get_ValueOrDefault()) : false)
            {
                  enumerator.get_Current()->ComputeLocalState();
                  enumerator.get_Current()->ClipNextTickByParent();
                  enumerator.get_Current()->set_NeedsPostfixTraversal(enumerator.get_Current()->IsType(GET_TYPE(ClockGroup)));
            }
            else
            {
                  enumerator.SkipSubtree();
            }
        }
    }

    ComputeTreeStateRoot();
}


void ClockGroup::ComputeTreeStateRoot()
{
    Nullable<TimeSpan> nullable2 = get_InternalNextTickNeededTime();
    set_InternalNextTickNeededTime(Nullable<TimeSpan>());

    for(int index = 0; index < (int)m_rootChildren.get_Count(); index++)
    {
        Clock* pClock = m_rootChildren[index];

        if(pClock == NULL)
        {
            m_rootChildren.RemoveAt(index);
            index--;
            continue;
        }

        if(pClock->get_NeedsPostfixTraversal())
        {
            ((ClockGroup*) pClock)->ComputeTreeStatePostfix();
        }

        if(get_InternalNextTickNeededTime().get_HasValue())
        {
            if(!pClock->get_InternalNextTickNeededTime().get_HasValue())
            {
                continue;
            }
            Nullable<TimeSpan> nullable7 = pClock->get_InternalNextTickNeededTime();
            Nullable<TimeSpan> nullable6 = get_InternalNextTickNeededTime();
            if (!((nullable7.get_HasValue() & nullable6.get_HasValue()) ? (nullable7.get_ValueOrDefault() < nullable6.get_ValueOrDefault()) : false))
            {
                  continue;
            }
        }
        set_InternalNextTickNeededTime(pClock->get_InternalNextTickNeededTime());

    }

    if(get_InternalNextTickNeededTime().get_HasValue())
    {
          if (nullable2.get_HasValue())
          {
                Nullable<TimeSpan> nullable4 = nullable2;
                Nullable<TimeSpan> nullable3 = get_InternalNextTickNeededTime();
                if (!((nullable4.get_HasValue() & nullable3.get_HasValue()) ? (nullable4.get_ValueOrDefault() > nullable3.get_ValueOrDefault()) : false))
                {
                      return;
                }
          }
          get_TimeManager()->NotifyNewEarliestFutureActivity();
    }
}

void ClockGroup::ComputeTreeStatePostfix()
{
    for (int num1 = 0; num1 < m_children.get_Count(); num1++)
    {
          if (m_children[num1]->get_NeedsPostfixTraversal())
          {
                ((ClockGroup*) (Clock*)m_children[num1])->ComputeTreeStatePostfix();
          }
    }
    ClipNextTickByChildren();

    set_NeedsPostfixTraversal(false);
}


void ClockGroup::ClipNextTickByChildren()
{
      for (int num1 = 0; num1 < m_children.get_Count(); num1++)
      {
            if (get_InternalNextTickNeededTime().get_HasValue())
            {
                  if (!m_children[num1]->get_InternalNextTickNeededTime().get_HasValue())
                  {
                        continue;
                  }
                  Nullable<TimeSpan> nullable2 = m_children[num1]->get_InternalNextTickNeededTime();
                  Nullable<TimeSpan> nextTickTime = get_InternalNextTickNeededTime();
                  if (!((nullable2.get_HasValue() & nextTickTime.get_HasValue()) ? (nullable2.get_ValueOrDefault() < nextTickTime.get_ValueOrDefault()) : false))
                  {
                        continue;
                  }
            }
            set_InternalNextTickNeededTime(m_children[num1]->get_InternalNextTickNeededTime());
      }
}

 
void ClockGroup::MakeRoot(TimeManager* pTimeManager)
{
      set_IsTimeManager(true);
      set_TimeManager(pTimeManager);
      set_Depth(0);

      set_InternalCurrentIteration(Nullable<int>(1));
      set_InternalCurrentProgress(Nullable<float>(0));
      set_InternalCurrentGlobalSpeed(Nullable<float>(1));
      set_InternalCurrentClockState(ClockState::Active);
}

void ClockGroup::RootActivate()
{
    TimeIntervalCollection collection = TimeIntervalCollection::CreatePoint(get_TimeManager()->get_InternalCurrentGlobalTime());
    collection.AddNullPoint();

    get_TimeManager()->set_InternalCurrentIntervals(collection);

    ComputeTreeState();
}

void ClockGroup::BuildClockSubTreeFromTimeline(Timeline* pTimeline, bool hasControllableRoot)
{
    TimelineGroup* pGroup = (TimelineGroup*) pTimeline;
    TimelineCollection* pChildren = pGroup->get_Children();

    if(pChildren != NULL && pChildren->get_Count() > 0)
    {
        for (int num1 = 0; num1 < pChildren->get_Count(); num1++)
        {
            Timeline* pTimeline = (*pChildren)[num1];
            ClockPtr pClock = Clock::AllocateClock(pTimeline, hasControllableRoot);

            pClock->BuildClockSubTreeFromTimeline(pTimeline, hasControllableRoot);
            m_children.AddItem((Clock*)pClock);
            pClock->set_InternalParent(this);
        }


        ParallelTimeline* pParallelTimeline = object_cast<ParallelTimeline>(get_Timeline());

        if(pParallelTimeline != NULL && pParallelTimeline->get_SlipBehavior() == SlipBehavior::Slip)
        {
            if(pParallelTimeline->get_RepeatBehavior().get_HasDuration()  ||
               !pParallelTimeline->get_RepeatBehavior().get_HasCount() ||
               pParallelTimeline->get_RepeatBehavior().get_Count() != 1 ||
               pParallelTimeline->get_AutoReverse() ||
               pParallelTimeline->get_AccelerationRatio() > 0 ||
               pParallelTimeline->get_DecelerationRatio() > 0)
            {
                // Only slip simple time lines.
                LibraryCriticalError();
            }

            for (int num2 = 0; num2 < m_children.get_Count(); num2++)
            {
                  Clock* pClock = m_children[num2];
                  if (pClock->get_CanSlip())
                  {
                        Duration duration1;
                        pClock->ComputeDuration(&duration1);
                        if (!duration1.get_HasTimeSpan() || duration1.get_TimeSpan() > TimeSpan::get_Zero())
                        {
                              if (!pClock->get_Timeline()->get_BeginTime().get_HasValue())
                              {
                                    return;
                              }

                              set_SyncData(SyncDataPtr(new SyncData(pClock), false));
                              pClock->set_SyncData(NULL);
                        }
                        return;
                  }
            }
        }
    }

}


void ClockGroup::ComputeCurrentFillInterval(const TimeIntervalCollection& parentIntervalCollection, const TimeSpan& beginTime, const TimeSpan& endTime, 
                                            const Duration& period, float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed)
{
    m_currentIntervals.Clear();
    parentIntervalCollection.ProjectPostFillZone(&m_currentIntervals, beginTime, endTime, period, appliedSpeedRatio, accelRatio, decelRatio, isAutoReversed);
}

void ClockGroup::ComputeCurrentIntervals(const TimeIntervalCollection& parentIntervalCollection, const TimeSpan& beginTime, const Nullable<TimeSpan>& endTime, const Duration& fillDuration, const Duration& period, 
                                         float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed)
{
   m_currentIntervals.Clear();

   parentIntervalCollection.ProjectOntoPeriodicFunction(&m_currentIntervals, beginTime, endTime, fillDuration, period, appliedSpeedRatio, accelRatio, decelRatio, isAutoReversed);
}


 


}; // namespace AVUI
