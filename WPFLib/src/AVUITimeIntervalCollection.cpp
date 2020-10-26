#include <AVUICommon.h>
#include <AVUITimeIntervalCollection.h>

namespace AVUI {

TimeIntervalCollection::TimeIntervalCollection() { Init(false); }

void TimeIntervalCollection::Init(bool containsNullPoint)
{
    m_containsNullPoint = false;
    m_count = 0;
    m_current = 0;
    m_invertCollection = false;
}

void TimeIntervalCollection::InitializePoint(const TimeSpan& point)
{
    EnsureAllocatedCapacity(4);
    m_nodes[0].nodeTime = point;
    m_nodes[0].nodeIsPoint = true;
    m_nodes[0].nodeIsInterval = false;    
    m_count = 1;
}

void TimeIntervalCollection::EnsureAllocatedCapacity(int capacity)
{
    if(capacity > (int) m_nodes.get_Capacity())
    {
        m_nodes.AddEmptyItems(capacity - m_nodes.get_Capacity());
    }
}

TimeIntervalCollection::TimeIntervalCollection(bool containsNullPoint)
{
    Init(containsNullPoint);
}

TimeIntervalCollection::TimeIntervalCollection(const TimeSpan& point)
{
    Init(false);
    InitializePoint(point);
}

TimeIntervalCollection::TimeIntervalCollection(const TimeSpan& point, bool includePoint)
{
    Init(false);
    InitializePoint(point);

    m_nodes[0].nodeIsPoint = includePoint;
    m_nodes[0].nodeIsInterval = true;
}


TimeIntervalCollection::TimeIntervalCollection(const TimeSpan& from, bool includeFrom, const TimeSpan& to, bool includeTo)
{
    Init(false);

    EnsureAllocatedCapacity(4);

    m_nodes[0].nodeTime = from;
    if(from == to)
    {
        if (includeFrom || includeTo)
        {
              m_nodes[0].nodeIsPoint = true;
        }
        m_count = 1;
    }
    else
    {
        if (from < to)
        {
              m_nodes[0].nodeIsPoint = includeFrom;
              m_nodes[0].nodeIsInterval = true;
              m_nodes[1].nodeTime = to;
              m_nodes[1].nodeIsPoint = includeTo;
        }
        else
        {
            m_nodes[0].nodeTime = to;
            m_nodes[0].nodeIsPoint = includeTo;
            m_nodes[0].nodeIsInterval = true;

            m_nodes[1].nodeTime = from;
            m_nodes[1].nodeIsPoint = includeFrom;
        }
        m_count = 2;
    }
}

bool TimeIntervalCollection::Intersects(const TimeIntervalCollection& other) const
{
    if (get_ContainsNullPoint() && other.get_ContainsNullPoint())
    {
        return true;
    }
    if (!get_IsEmptyOfRealPoints() && !other.get_IsEmptyOfRealPoints())
    {
        return IntersectsHelper(other);
    }
    return false;

}
bool TimeIntervalCollection::IntersectsInverseOf(const TimeIntervalCollection& other) const
{
    if (get_ContainsNullPoint() && !other.get_ContainsNullPoint())
    {
        return true;
    }
    if (get_IsEmptyOfRealPoints())
    {
        return false;
    }
    if (get_IsEmptyOfRealPoints() || (m_nodes[0].nodeTime < other.m_nodes[0].nodeTime))
    {
        return true;
    }
    other.set_InvertedMode(true);
    bool intersects = IntersectsHelper(other);
    other.set_InvertedMode(false);
    return intersects;
}

bool TimeIntervalCollection::IntersectsPeriodicCollection(const TimeSpan& beginTime, const Duration& period, float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) const
{
     if (((!get_IsEmptyOfRealPoints() && (period != ((Duration) TimeSpan::get_Zero()))) && (((accelRatio != 0.0f) || (decelRatio != 0.0f)) || isAutoReversed)) && (period.get_HasTimeSpan() && (appliedSpeedRatio <= period.get_TimeSpan().get_Ticks())))
      {
            MoveFirst();
            int num7 = beginTime.get_Ticks();
            int num2 = (int) (((float) period.get_TimeSpan().get_Ticks()) / appliedSpeedRatio);
            int num6 = 2 * num2;
            int num4 = (int) (accelRatio * num2);
            int num3 = (int) ((1 - decelRatio) * num2);
            while (m_current < m_count)
            {
                  int num1;
                  bool flag1 = false;
                  if (isAutoReversed)
                  {
                        num1 = (get_CurrentNodeTime().get_Ticks() - num7) % num6;
                        if (num1 >= num2)
                        {
                              num1 = num6 - num1;
                              flag1 = true;
                        }
                  }
                  else
                  {
                        num1 = (get_CurrentNodeTime().get_Ticks() - num7) % num2;
                  }
                  if (((0 < num1) && (num1 < num4)) || (num3 < num1))
                  {
                        return true;
                  }
                  if (((num1 == 0) || (num1 == num3)) && get_CurrentNodeIsPoint())
                  {
                        return true;
                  }
                  if (get_CurrentNodeIsInterval())
                  {
                        int num5;
                        if (((num1 == 0) && (num4 > 0)) || ((num1 == num3) && (num3 < num2)))
                        {
                              return true;
                        }
                        if (flag1)
                        {
                              num5 = num1 - num4;
                        }
                        else
                        {
                              num5 = num3 - num1;
                        }
                        if (get_CurrentIsAtLastNode() || (!get_CurrentIsAtLastNode() && ((get_NextNodeTime().get_Ticks() - get_CurrentNodeTime().get_Ticks()) >= num5)))
                        {
                              return true;
                        }
                  }
                  MoveNext();
            }
      }
      return false;
}

bool TimeIntervalCollection::IntersectsHelper(const TimeIntervalCollection& other) const
{
    IntersectsHelperPrepareIndexers(this, &other);

    bool flag1 = false;
    while (((get_CurrentNodeTime() >= other.get_CurrentNodeTime()) || !IntersectsHelperUnequalCase(this, &other, &flag1)) && ((get_CurrentNodeTime() <= other.get_CurrentNodeTime()) || !IntersectsHelperUnequalCase(&other, this, &flag1)))
    {
        while (get_CurrentNodeTime() == other.get_CurrentNodeTime())
        {
              if (IntersectsHelperEqualCase(this, &other, &flag1))
              {
                    return flag1;
              }
        }
    }
    return flag1;
}

void TimeIntervalCollection::IntersectsHelperPrepareIndexers(const TimeIntervalCollection* pTic1, const TimeIntervalCollection* pTic2)
{
      pTic1->MoveFirst();
      pTic2->MoveFirst();
      if (pTic1->get_CurrentNodeTime() < pTic2->get_CurrentNodeTime())
      {
            while (!pTic1->get_CurrentIsAtLastNode() && (pTic1->get_NextNodeTime() <= pTic2->get_CurrentNodeTime()))
            {
                  pTic1->MoveNext();
            }
      }
      else if (pTic2->get_CurrentNodeTime() < pTic1->get_CurrentNodeTime())
      {
            while (!pTic2->get_CurrentIsAtLastNode() && (pTic2->get_NextNodeTime() <= pTic1->get_CurrentNodeTime()))
            {
                  pTic2->MoveNext();
            }
      }
}

bool TimeIntervalCollection::IntersectsHelperEqualCase(const TimeIntervalCollection* pTic1, const TimeIntervalCollection* pTic2, bool* pIntersectionFound)
{
      if ((pTic1->get_CurrentNodeIsPoint() && pTic2->get_CurrentNodeIsPoint()) || (pTic1->get_CurrentNodeIsInterval() && pTic2->get_CurrentNodeIsInterval()))
      {
            *pIntersectionFound = true;
            return true;
      }
      if (!pTic1->get_CurrentIsAtLastNode() && (pTic2->get_CurrentIsAtLastNode() || (pTic1->get_NextNodeTime() < pTic2->get_NextNodeTime())))
      {
            pTic1->MoveNext();
      }
      else if (!pTic2->get_CurrentIsAtLastNode() && (pTic1->get_CurrentIsAtLastNode() || (pTic2->get_NextNodeTime() < pTic1->get_NextNodeTime())))
      {
            pTic2->MoveNext();
      }
      else if (!pTic1->get_CurrentIsAtLastNode() && !pTic2->get_CurrentIsAtLastNode())
      {
            pTic1->MoveNext();
            pTic2->MoveNext();
      }
      else
      {
            *pIntersectionFound = false;
            return true;
      }
      return false;
}

 
bool TimeIntervalCollection::IntersectsHelperUnequalCase(const TimeIntervalCollection* pTic1, const TimeIntervalCollection* pTic2, bool* pIntersectionFound)
{
      if (pTic1->get_CurrentNodeIsInterval())
      {
            *pIntersectionFound = true;
            return true;
      }
      if (pTic1->get_CurrentIsAtLastNode())
      {
            *pIntersectionFound = false;
            return true;
      }

      bool doContinue = true;
      while (doContinue)
      {
            if (pTic2->get_CurrentIsAtLastNode() || (pTic2->get_NextNodeTime() > pTic1->get_NextNodeTime()))
            {
                  pTic1->MoveNext();
                  return false;
            }
            pTic2->MoveNext();
      }

      return false;
}

 

TimeIntervalCollection TimeIntervalCollection::SlipBeginningOfConnectedInterval(const TimeSpan& slipTime)
{
      if (slipTime == TimeSpan::get_Zero())
      {
            return (*this);
      }
      TimeSpan span1 = m_nodes[1].nodeTime - m_nodes[0].nodeTime;
      if (slipTime < span1)
      {
            return TimeIntervalCollection(m_nodes[0].nodeTime + slipTime, m_nodes[0].nodeIsPoint, m_nodes[1].nodeTime, m_nodes[1].nodeIsPoint);
      }
      return TimeIntervalCollection(m_nodes[1].nodeTime);
}

void TimeIntervalCollection::Clear()
{
    m_nodes.Clear();
    m_nodes.AddEmptyItems(4);
    m_containsNullPoint = false;
    m_count = 0;
    m_current = 0;
    m_invertCollection = false;
    
}

void TimeIntervalCollection::ProjectOntoPeriodicFunction(TimeIntervalCollection* pProjection, const TimeSpan& beginTime, const Nullable<TimeSpan>& endTime, const Duration& fillDuration, const Duration& period, 
                                                         float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) const
{
    bool flag3 = (m_containsNullPoint || (m_nodes[0].nodeTime < beginTime)) || ((endTime.get_HasValue() && fillDuration.get_HasTimeSpan()) && ((m_nodes[m_count - 1].nodeTime > (endTime.get_Value() + fillDuration.get_TimeSpan())) || 
                 (((m_nodes[m_count - 1].nodeTime == (endTime.get_Value() + fillDuration.get_TimeSpan())) && 
                   m_nodes[m_count - 1].nodeIsPoint) && ((endTime.get_Value() > beginTime) || (fillDuration.get_TimeSpan() > TimeSpan::get_Zero())))));

    if (endTime.get_HasValue() && (beginTime == endTime))
    {
        pProjection->InitializePoint(TimeSpan::get_Zero());
    }
    else
    {
        bool includeFillPeriod = !fillDuration.get_HasTimeSpan() || (fillDuration.get_TimeSpan() > TimeSpan::get_Zero());
        if (period.get_HasTimeSpan())
        {
            bool flag2;
            Nullable<TimeSpan> nullable;
            TimeIntervalCollection intervals;
            ProjectionNormalize(&intervals, beginTime, endTime, includeFillPeriod, appliedSpeedRatio);
            int ticks = period.get_TimeSpan().get_Ticks();
            if (endTime.get_HasValue())
            {
                nullable = Nullable<TimeSpan>(endTime.get_Value() - beginTime);
                flag2 = includeFillPeriod && ((nullable.get_Value().get_Ticks() % ticks) == 0L);
            }
            else
            {
                flag2 = false;
            }
            pProjection->EnsureAllocatedCapacity(4);
            intervals.ProjectionFold(pProjection, nullable, ticks, isAutoReversed, flag2);
            if ((accelRatio + decelRatio) > 0.0f)
            {
                pProjection->ProjectionWarp(ticks, accelRatio, decelRatio);
            }
        }
        else
        {
            ProjectionNormalize(pProjection, beginTime, endTime, includeFillPeriod, appliedSpeedRatio);
        }
    }
    pProjection->m_containsNullPoint = flag3;
}

void TimeIntervalCollection::ProjectionWarp(int periodInTicks, float accelRatio, float decelRatio)
{
    float ticks;
    float num2 = (float)periodInTicks;
    float num4 = 1.0f / num2;
    float num3 = 1.0f / ((2.0f - accelRatio) - decelRatio);
    TimeSpan span2 = TimeSpan::FromTicks((int) (num2 * accelRatio));
    TimeSpan span = TimeSpan::FromTicks(periodInTicks - ((int) (num2 * decelRatio)));

    MoveFirst();

    while ((m_current < m_count) && (get_CurrentNodeTime() < span2))
    {
        ticks = (float) (m_nodes[m_current].nodeTime.get_Ticks());
        m_nodes[m_current].nodeTime = TimeSpan::FromTicks((int) ((((num3 * num4) * ticks) * ticks) / accelRatio));
        MoveNext();
    }
    while ((m_current < m_count) && (get_CurrentNodeTime() <= span))
    {
        ticks = (float) (m_nodes[m_current].nodeTime.get_Ticks());
        m_nodes[m_current].nodeTime = TimeSpan::FromTicks((int) (num3 * ((2.0f * ticks) - (accelRatio * num2))));
        MoveNext();
    }
    while (m_current < m_count)
    {
        ticks = (float) (periodInTicks - m_nodes[m_current].nodeTime.get_Ticks());
        m_nodes[m_current].nodeTime = TimeSpan::FromTicks(periodInTicks - ((int) ((((num3 * num4) * ticks) * ticks) / decelRatio)));
        MoveNext();
    }
}
 
void TimeIntervalCollection::ProjectionNormalize(TimeIntervalCollection* pProjection, const TimeSpan& beginTime, const Nullable<TimeSpan>& endTime, bool includeFillPeriod, float speedRatio) const
{
    TimeSpan span3;
    pProjection->EnsureAllocatedCapacity(m_nodes.get_Count());
    MoveFirst();
    pProjection->MoveFirst();
    while (!get_CurrentIsAtLastNode() && (get_NextNodeTime() <= beginTime))
    {
        MoveNext();
    }
    if (get_CurrentNodeTime() < beginTime)
    {
        if (get_CurrentNodeIsInterval())
        {
            pProjection->m_count++;
            pProjection->set_CurrentNodeTime(TimeSpan::get_Zero());
            pProjection->set_CurrentNodeIsPoint(true);
            pProjection->set_CurrentNodeIsInterval(true);
            pProjection->MoveNext();
        }
        MoveNext();
    }

    while(true)
    {
        if (m_current < m_count)
        {
            if (!endTime.get_HasValue())
            {
                span3 = get_CurrentNodeTime() - beginTime;
                float ticks = (float)span3.get_Ticks();
                pProjection->m_count++;
                pProjection->set_CurrentNodeTime(TimeSpan::FromTicks((int) (speedRatio * ticks)));
                pProjection->set_CurrentNodeIsPoint(get_CurrentNodeIsPoint());
                pProjection->set_CurrentNodeIsInterval(get_CurrentNodeIsInterval());
                pProjection->MoveNext();
                MoveNext();
                continue;
            }
            TimeSpan currentNodeTime = get_CurrentNodeTime();
            Nullable<TimeSpan> nullable = endTime;
            if (nullable.get_HasValue() ? (currentNodeTime < nullable.get_ValueOrDefault()) : false)
            {
                span3 = get_CurrentNodeTime() - beginTime;
                float ticks = (float)span3.get_Ticks();
                pProjection->m_count++;
                pProjection->set_CurrentNodeTime(TimeSpan::FromTicks((int) (speedRatio * ticks)));
                pProjection->set_CurrentNodeIsPoint(get_CurrentNodeIsPoint());
                pProjection->set_CurrentNodeIsInterval(get_CurrentNodeIsInterval());
                pProjection->MoveNext();
                MoveNext();
                continue;
            }
        }
        break;
    }
    if ((m_current < m_count) && (m_nodes[m_current - 1].nodeIsInterval || (((get_CurrentNodeTime() == endTime.get_Value()) && get_CurrentNodeIsPoint()) && includeFillPeriod)))
    {
        TimeSpan span2 = endTime.get_Value() - beginTime;
        float num = (float)span2.get_Ticks();
        pProjection->m_count++;
        pProjection->set_CurrentNodeTime(TimeSpan::FromTicks((int) (speedRatio * num)));
        pProjection->set_CurrentNodeIsPoint(includeFillPeriod && ((get_CurrentNodeTime() > endTime.get_Value()) || get_CurrentNodeIsPoint()));
        pProjection->set_CurrentNodeIsInterval(false);
    }
}

void TimeIntervalCollection::ProjectionFold(TimeIntervalCollection* pProjection, const Nullable<TimeSpan>& activeDuration, int periodInTicks, bool isAutoReversed, bool includeMaxPoint)
{
    MoveFirst();
    bool flag = false;
    do
    {
        if (get_CurrentNodeIsInterval())
        {
            flag = ProjectionFoldInterval(pProjection, activeDuration, periodInTicks, isAutoReversed, includeMaxPoint);
            m_current += get_NextNodeIsInterval() ? 1 : 2;
        }
        else
        {
            ProjectionFoldPoint(pProjection, activeDuration, periodInTicks, isAutoReversed, includeMaxPoint);
            m_current++;
        }
    }
    while (!flag && (m_current < m_count));
}

void TimeIntervalCollection::ProjectionFoldPoint(TimeIntervalCollection* pProjection, const Nullable<TimeSpan>& activeDuration, int periodInTicks, bool isAutoReversed, bool includeMaxPoint)
{
    int num;
    if (isAutoReversed)
    {
        int num2 = periodInTicks << 1;
        num = get_CurrentNodeTime().get_Ticks() % num2;
        if (num > periodInTicks)
        {
            num = num2 - num;
        }
    }
    else if ((includeMaxPoint && activeDuration.get_HasValue()) && (get_CurrentNodeTime() == activeDuration))
    {
        num = periodInTicks;
    }
    else
    {
        if(periodInTicks > 0)
        {
            num = get_CurrentNodeTime().get_Ticks() % periodInTicks;
        }
    }
    pProjection->MergePoint(TimeSpan::FromTicks(num));
}

void TimeIntervalCollection::MergePoint(const TimeSpan& point)
{
    int index = Locate(point);
    if ((index >= 0) && (m_nodes[index].nodeTime == point))
    {
        if (!m_nodes[index].nodeIsPoint)
        {
            if (((index == 0) || !m_nodes[index - 1].nodeIsInterval) || !m_nodes[index].nodeIsInterval)
            {
                m_nodes[index].nodeIsPoint = true;
            }
            else
            {
                for (int i = index; (i + 1) < m_count; i++)
                {
                    m_nodes[i] = m_nodes[i + 1];
                }
                m_count--;
            }
        }
    }
    else if ((index == -1) || !m_nodes[index].nodeIsInterval)
    {
        EnsureAllocatedCapacity(m_count + 1);
        for (int j = m_count - 1; j > index; j--)
        {
            m_nodes[j + 1] = m_nodes[j];
        }

        m_nodes[index + 1].nodeTime = point;
        m_nodes[index + 1].nodeIsPoint = true;
        m_nodes[index + 1].nodeIsInterval = false;
        m_count++;
    }
}

int TimeIntervalCollection::Locate(const TimeSpan& time)
{
    if ((m_count == 0) || (time < m_nodes[0].nodeTime))
    {
        return -1;
    }
    int num2 = 0;
    int index = m_count - 1;
    while ((num2 + 1) < index)
    {
        int num3 = (num2 + index) >> 1;
        if (time < m_nodes[num3].nodeTime)
        {
            index = num3;
        }
        else
        {
            num2 = num3;
        }
    }
    if (time < m_nodes[index].nodeTime)
    {
        return num2;
    }
    return index;
}

 
bool TimeIntervalCollection::ProjectionFoldInterval(TimeIntervalCollection* pProjection, const Nullable<TimeSpan>& activeDuration, int periodInTicks, bool isAutoReversed, bool includeMaxPoint)
{
    int num;
    int num2;
    TimeSpan span4 = get_NextNodeTime() - get_CurrentNodeTime();
    int ticks = span4.get_Ticks();
    if (isAutoReversed)
    {
        bool flag3;
        int num7 = periodInTicks << 1;
        num = get_CurrentNodeTime().get_Ticks() % num7;
        if (num < periodInTicks)
        {
            flag3 = false;
            num2 = periodInTicks - num;
        }
        else
        {
            flag3 = true;
            num = num7 - num;
            num2 = num;
        }
        long num4 = ticks - num2;
        if (num4 > 0L)
        {
            if (num2 >= num4)
            {
                bool currentNodeIsPoint = get_CurrentNodeIsPoint();
                if (num2 == num4)
                {
                    currentNodeIsPoint = currentNodeIsPoint || get_NextNodeIsPoint();
                }
                if (flag3)
                {
                    pProjection->MergeInterval(TimeSpan::get_Zero(), true, TimeSpan::FromTicks(num), currentNodeIsPoint);
                    return (currentNodeIsPoint && (num == periodInTicks));
                }
                pProjection->MergeInterval(TimeSpan::FromTicks(num), currentNodeIsPoint, TimeSpan::FromTicks(periodInTicks), true);
                return (currentNodeIsPoint && (num == 0L));
            }
            if (flag3)
            {
                long num6 = (num4 < periodInTicks) ? num4 : periodInTicks;
                pProjection->MergeInterval(TimeSpan::get_Zero(), true, TimeSpan::FromTicks(num6), get_NextNodeIsPoint());
                return (get_NextNodeIsPoint() && (num6 == periodInTicks));
            }
            long num5 = (num4 < periodInTicks) ? (periodInTicks - num4) : 0L;
            pProjection->MergeInterval(TimeSpan::FromTicks(num5), get_NextNodeIsPoint(), TimeSpan::FromTicks(periodInTicks), true);
            return (get_NextNodeIsPoint() && (num5 == 0L));
        }
        if (flag3)
        {
            pProjection->MergeInterval(TimeSpan::FromTicks(num - ticks), get_NextNodeIsPoint(), TimeSpan::FromTicks(num), get_CurrentNodeIsPoint());
        }
        else
        {
            pProjection->MergeInterval(TimeSpan::FromTicks(num), get_CurrentNodeIsPoint(), TimeSpan::FromTicks(num + ticks), get_NextNodeIsPoint());
        }
        return false;
    }
    num = get_CurrentNodeTime().get_Ticks() % periodInTicks;
    num2 = periodInTicks - num;
    if (ticks > periodInTicks)
    {
        pProjection->m_nodes[0].nodeTime = TimeSpan::get_Zero();
        pProjection->m_nodes[0].nodeIsPoint = true;
        pProjection->m_nodes[0].nodeIsInterval = true;
        pProjection->m_nodes[1].nodeTime = TimeSpan::FromTicks(periodInTicks);
        pProjection->m_nodes[1].nodeIsPoint = includeMaxPoint;
        pProjection->m_nodes[1].nodeIsInterval = false;
        m_count = 2;
        return true;
    }
    if (ticks >= num2)
    {
        pProjection->MergeInterval(TimeSpan::FromTicks(num), get_CurrentNodeIsPoint(), TimeSpan::FromTicks(periodInTicks), false);
        if (ticks > num2)
        {
            pProjection->MergeInterval(TimeSpan::get_Zero(), true, TimeSpan::FromTicks(ticks - num2), get_NextNodeIsPoint());
        }
        else if (get_NextNodeIsPoint())
        {
            if ((includeMaxPoint && activeDuration.get_HasValue()) && (get_NextNodeTime() == activeDuration))
            {
                pProjection->MergePoint(TimeSpan::FromTicks(periodInTicks));
            }
            else
            {
                pProjection->MergePoint(TimeSpan::get_Zero());
            }
        }
        return false;
    }
    pProjection->MergeInterval(TimeSpan::FromTicks(num), get_CurrentNodeIsPoint(), TimeSpan::FromTicks(num + ticks), get_NextNodeIsPoint());

    return false;
}

void TimeIntervalCollection::MergeInterval(const TimeSpan& from, bool includeFrom, const TimeSpan& to, bool includeTo)
{
    if (get_IsEmptyOfRealPoints())
    {
        m_nodes[0].nodeTime = from;
        m_nodes[0].nodeIsPoint = includeFrom;
        m_nodes[0].nodeIsInterval = true;
        m_nodes[1].nodeTime = to;
        m_nodes[1].nodeIsPoint = includeTo;
        m_nodes[1].nodeIsInterval = false;
        m_count = 2;
    }
    else
    {
        int index = Locate(from);
        int num4 = Locate(to);
        bool flag2 = false;
        bool flag = false;
        int num = index - num4;
        int num3 = index + 1;
        int num7 = num4;
        if ((index == -1) || (m_nodes[index].nodeTime < from))
        {
            if ((index == -1) || !m_nodes[index].nodeIsInterval)
            {
                flag2 = true;
                num++;
            }
        }
        else if (((index > 0) && m_nodes[index - 1].nodeIsInterval) && (includeFrom || m_nodes[index].nodeIsPoint))
        {
            num--;
            num3--;
        }
        else
        {
            m_nodes[index].nodeIsPoint = includeFrom || m_nodes[index].nodeIsPoint;
        }
        if ((num4 == -1) || (m_nodes[num4].nodeTime < to))
        {
            if ((num4 == -1) || !m_nodes[num4].nodeIsInterval)
            {
                flag = true;
                num++;
            }
        }
        else if (!m_nodes[num4].nodeIsInterval || (!includeTo && !m_nodes[num4].nodeIsPoint))
        {
            num++;
            num7--;
            m_nodes[num4].nodeIsPoint = includeTo || m_nodes[num4].nodeIsPoint;
        }
        if (num > 0)
        {
            EnsureAllocatedCapacity(m_count + num);    
            for (int i = m_count - 1; i > num7; i--)
            {
                m_nodes[i + num] = m_nodes[i];
            }
        }
        else if (num < 0)
        {
            for (int j = num7 + 1; j < m_count; j++)
            {
                m_nodes[j + num] = m_nodes[j];
            }
        }
        m_count += num;
        if (flag2)
        {
            m_nodes[num3].nodeTime = from;
            m_nodes[num3].nodeIsPoint = includeFrom;
            m_nodes[num3].nodeIsInterval = true;
            num3++;
        }
        if (flag)
        {
            m_nodes[num3].nodeTime = to;
            m_nodes[num3].nodeIsPoint = includeTo;
            m_nodes[num3].nodeIsInterval = false;
        }
    }
}

 


void TimeIntervalCollection::ProjectPostFillZone(TimeIntervalCollection* pProjection, const TimeSpan& beginTime, const TimeSpan& endTime, const Duration& period, float appliedSpeedRatio, float accelRatio, float decelRatio, bool isAutoReversed) const
{
    int num;
    if (beginTime == endTime)
    {
        num = 0L;
    }
    else
    {
        TimeSpan span2 = endTime - beginTime;
        num = (int) (appliedSpeedRatio * span2.get_Ticks());
        if (period.get_HasTimeSpan())
        {
            int ticks = period.get_TimeSpan().get_Ticks();
            if (isAutoReversed)
            {
                int num7 = ticks << 1;
                num = num % num7;
                if (num > ticks)
                {
                    num = num7 - num;
                }
            }
            else
            {
                num = num % ticks;
                if (num == 0L)
                {
                    num = ticks;
                }
            }
            if ((accelRatio + decelRatio) > 0.0f)
            {
                float num3;
                float num5 = (float)ticks;
                float num6 = 1.0f / num5;
                float num4 = 1.0f / ((2.0f - accelRatio) - decelRatio);
                int num9 = (int) (num5 * accelRatio);
                int num8 = ticks - ((int) (num5 * decelRatio));
                if (num < num9)
                {
                    num3 = (float)num;
                    num = (int) ((((num4 * num6) * num3) * num3) / accelRatio);
                }
                else if (num <= num8)
                {
                    num3 = (float)num;
                    num = (int) (num4 * ((2.0f * num3) - accelRatio));
                }
                else
                {
                    num3 = (float)(ticks - num);
                    num = ticks - ((int) ((((num4 * num6) * num3) * num3) / decelRatio));
                }
            }
        }
    }
    pProjection->InitializePoint(TimeSpan::FromTicks(num));
}

 

 
 



}; // namespace AVUI
