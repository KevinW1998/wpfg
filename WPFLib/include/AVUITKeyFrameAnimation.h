#pragma once

#include <AVUITAnimationBase.h>
#include <AVUIClockGroup.h>
#include <AVUIAnimationClock.h>
#include <AVUIAnimatedTypeHelpers.h>
#include <AVUIKeyTime.h>

namespace AVUI {

template<class T, class KeyFrameType, class KeyFrameCollectionType>
class TValueKeyFrameAnimation : public TValueAnimationBase<T>
{

protected:
    TValueKeyFrameAnimation() : m_areKeyTimesValid(false) { }
    ~TValueKeyFrameAnimation() { m_keyFrames.InvalidateWeakReferences(); }

    virtual const bool& get_IsAdditive() = 0;
    virtual const bool& get_IsCumulative() = 0;

    KeyFrameCollectionType* get_KeyFrames() { return &m_keyFrames; }

    virtual Duration get_NaturalDurationCore(const Clock* pClock) { return Duration(get_LargestTimeSpanKeyTime()); }
    
    virtual T GetCurrentValueCore(const T& defaultOriginValue, const T& defaultDestinationValue, AnimationClock* pAnimationClock)
    {
        T currentValue;

        KeyFrameCollectionType* pKeyFrames = get_KeyFrames();
        if (pKeyFrames == NULL || pKeyFrames->get_Count() == 0)
        {
            return defaultDestinationValue;
        }
        if (!m_areKeyTimesValid)
        {
            ResolveKeyTimes();
        }
        if (m_sortedResolvedKeyFrames.get_Count() == 0)
        {
            return defaultDestinationValue;
        }

        TimeSpan currentTime = pAnimationClock->get_CurrentTime().get_Value();

        int cKeyFrames = m_sortedResolvedKeyFrames.get_Count();
        int idxLastKeyFrame = cKeyFrames - 1;
        int idxCurKeyFrame = 0;
        while ((idxCurKeyFrame < cKeyFrames) && (currentTime > m_sortedResolvedKeyFrames[idxCurKeyFrame].m_resolvedKeyTime))
        {
            idxCurKeyFrame++;
        }
        while ((idxCurKeyFrame < idxLastKeyFrame) && (currentTime == m_sortedResolvedKeyFrames[idxCurKeyFrame + 1].m_resolvedKeyTime))
        {
            idxCurKeyFrame++;
        }
        if (idxCurKeyFrame == cKeyFrames)
        {
            currentValue = GetResolvedKeyFrameValue(idxLastKeyFrame);
        }
        else if (currentTime == m_sortedResolvedKeyFrames[idxCurKeyFrame].m_resolvedKeyTime)
        {
            currentValue = GetResolvedKeyFrameValue(idxCurKeyFrame);
        }
        else
        {
            T fromValue;
            float progress = 0;
            if (idxCurKeyFrame == 0)
            {
                if (get_IsAdditive())
                {
                    fromValue = AnimatedTypeHelpers::GetZeroValue<T>();
                }
                else
                {
                    fromValue = defaultOriginValue;
                }
                progress = currentTime.get_TotalMilliseconds() / m_sortedResolvedKeyFrames[0].m_resolvedKeyTime.get_TotalMilliseconds();
            }
            else
            {
                int lastFrame = idxCurKeyFrame - 1;
                TimeSpan lastFrameTime = m_sortedResolvedKeyFrames[lastFrame].m_resolvedKeyTime;
                fromValue = GetResolvedKeyFrameValue(lastFrame);
                TimeSpan currentDelta = currentTime - lastFrameTime;
                TimeSpan overallDelta = m_sortedResolvedKeyFrames[idxCurKeyFrame].m_resolvedKeyTime - lastFrameTime;
                progress = currentDelta.get_TotalMilliseconds() / overallDelta.get_TotalMilliseconds();
            }
            currentValue = GetResolvedKeyFrame(idxCurKeyFrame)->InterpolateValue(fromValue, progress);
        }
        if (get_IsCumulative())
        {
            Nullable<int> nullable3 = pAnimationClock->get_CurrentIteration();
            Nullable<int> nullable2 = nullable3.get_HasValue() ? Nullable<int>(nullable3.get_ValueOrDefault() - 1) : Nullable<int>();
            float num5 = (float) nullable2.get_Value();
            if (num5 > 0)
            {
                currentValue = AnimatedTypeHelpers::Add(currentValue, AnimatedTypeHelpers::Scale(GetResolvedKeyFrameValue(idxLastKeyFrame), num5));
            }
        }
        if (get_IsAdditive())
        {
            return AnimatedTypeHelpers::Add(defaultOriginValue, currentValue);
        }
        return currentValue;
    }

    KeyFrameType* GetResolvedKeyFrame(int idxKeyFrame)
    {
        return (*(get_KeyFrames()))[m_sortedResolvedKeyFrames[idxKeyFrame].m_originalKeyFrameIndex];
    }

    const T& GetResolvedKeyFrameValue(int idxKeyFrame)
    {
        return GetResolvedKeyFrame(idxKeyFrame)->get_Value();
    }

private:

    TimeSpan get_LargestTimeSpanKeyTime()
    {
        bool hasValidTime = false;
        TimeSpan largestTimeSpan = TimeSpan::get_Zero();

        KeyFrameCollectionType* pKeyFrames = get_KeyFrames();

        if (pKeyFrames != NULL)
        {
            int cKeys = pKeyFrames->get_Count();
            
            for (int idxKey = 0; idxKey < cKeys; idxKey++)
            {
                KeyTime keyTime = (*pKeyFrames)[idxKey]->get_KeyTime();
                if (keyTime.get_Type() == KeyTimeType::TimeSpan)
                {
                      hasValidTime = true;
                      if (keyTime.get_TimeSpan() > largestTimeSpan)
                      {
                            largestTimeSpan = keyTime.get_TimeSpan();
                      }
                }
            }
        }
        if (hasValidTime)
        {
              return largestTimeSpan;
        }
        return TimeSpan::FromSeconds(1);
    }

    struct ResolvedKeyFrameEntry
    {
        int m_originalKeyFrameIndex;
        TimeSpan m_resolvedKeyTime;
    };

    struct KeyTimeBlock
    { 
        int BeginIndex;
        int EndIndex;
    };

    void ResolveKeyTimes()
    {
        m_sortedResolvedKeyFrames.Clear();
        m_areKeyTimesValid = true;


        int cKeyFrames = 0;

        KeyFrameCollectionType* pKeyFrames = get_KeyFrames();
        if (pKeyFrames != NULL)
        {
            cKeyFrames = pKeyFrames->get_Count();
        }

        if (cKeyFrames == 0)
        {
            return;
        }

        m_sortedResolvedKeyFrames.AddEmptyItems(cKeyFrames);

        int idxCur = 0;
        while (idxCur < cKeyFrames)
        {
            m_sortedResolvedKeyFrames[idxCur].m_originalKeyFrameIndex = idxCur;
            idxCur++;
        }

        TimeSpan span3 = TimeSpan::get_Zero();

        Duration duration1 = Timeline::get_Duration();
        if (duration1.get_HasTimeSpan())
        {
            span3 = duration1.get_TimeSpan();
        }
        else
        {
            span3 = get_LargestTimeSpanKeyTime();
        }
        int num4 = cKeyFrames - 1;

        ItemStructList<KeyTimeBlock> blockList;

        idxCur = 0;
        while (idxCur < cKeyFrames)
        {
            KeyTime time2 = (*pKeyFrames)[idxCur]->get_KeyTime();
            switch (time2.get_Type())
            {
            case KeyTimeType::Uniform:
                {
                    if (idxCur != num4)
                    {
                        break;
                    }
                    m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = span3;
                    idxCur++;
                    continue;
                }
            case KeyTimeType::Percent:
                {
                    m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = TimeSpan::FromMilliseconds((int)(time2.get_Percent() * span3.get_TotalMilliseconds()));
                    idxCur++;
                    continue;
                }
            case KeyTimeType::TimeSpan:
                {
                    m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = time2.get_TimeSpan();
                    idxCur++;
                    continue;
                }
            default:
                {
                    continue;
                }
            }

            KeyTimeBlock block1;

            block1.BeginIndex = idxCur;
            while (++idxCur < num4)
            {
            }
            block1.EndIndex = idxCur;
            blockList.AddItem(block1);
        }
        for (int num3 = 0; num3 < (int)blockList.get_Count(); num3++)
        {
            KeyTimeBlock block2 = blockList[num3];
            TimeSpan span2 = TimeSpan::get_Zero();
            if (block2.BeginIndex > 0)
            {
                span2 = m_sortedResolvedKeyFrames[block2.BeginIndex - 1].m_resolvedKeyTime;
            }
            long num5 = (block2.EndIndex - block2.BeginIndex) + 1;
            TimeSpan span5 = m_sortedResolvedKeyFrames[block2.EndIndex].m_resolvedKeyTime - span2;
            TimeSpan span4 = TimeSpan::FromTicks(span5.get_Ticks() / num5);
            idxCur = block2.BeginIndex;
            TimeSpan span1 = span2 + span4;
            while (idxCur < block2.EndIndex)
            {
                m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = span1;
                span1 = span1 + span4;
                idxCur++;
            }
        }
//        Array.Sort<ResolvedKeyFrameEntry>(this._sortedResolvedKeyFrames);
    }     

    KeyFrameCollectionType m_keyFrames;
    ItemStructList<ResolvedKeyFrameEntry> m_sortedResolvedKeyFrames;
    bool m_areKeyTimesValid;
};




template<class T, class KeyFrameType, class KeyFrameCollectionType>
class TReferenceKeyFrameAnimation : public TReferenceAnimationBase<T>
{

protected:
    TReferenceKeyFrameAnimation() : m_areKeyTimesValid(false) { }
    ~TReferenceKeyFrameAnimation() { m_keyFrames.InvalidateWeakReferences(); }

    virtual const bool& get_IsAdditive() = 0;
    virtual const bool& get_IsCumulative() = 0;

    KeyFrameCollectionType* get_KeyFrames() { return &m_keyFrames; }

    virtual Duration get_NaturalDurationCore(const Clock* pClock) { return Duration(get_LargestTimeSpanKeyTime()); }

    virtual TRefCountedPtr<T> GetCurrentValueCore(T* pDefaultOriginValue, T* pDefaultDestinationValue, AnimationClock* pAnimationClock)
    {
        TRefCountedPtr<T> pCurrentValue;

        KeyFrameCollectionType* pKeyFrames = get_KeyFrames();
        if (pKeyFrames == NULL || pKeyFrames->get_Count() == 0)
        {
            return pDefaultDestinationValue;
        }
        if (!m_areKeyTimesValid)
        {
            ResolveKeyTimes();
        }
        if (m_sortedResolvedKeyFrames.get_Count() == 0)
        {
            return pDefaultDestinationValue;
        }

        TimeSpan currentTime = pAnimationClock->get_CurrentTime().get_Value();

        int cKeyFrames = m_sortedResolvedKeyFrames.get_Count();
        int idxLastKeyFrame = cKeyFrames - 1;
        int idxCurKeyFrame = 0;
        while ((idxCurKeyFrame < cKeyFrames) && (currentTime > m_sortedResolvedKeyFrames[idxCurKeyFrame].m_resolvedKeyTime))
        {
            idxCurKeyFrame++;
        }
        while ((idxCurKeyFrame < idxLastKeyFrame) && (currentTime == m_sortedResolvedKeyFrames[idxCurKeyFrame + 1].m_resolvedKeyTime))
        {
            idxCurKeyFrame++;
        }
        if (idxCurKeyFrame == cKeyFrames)
        {
            pCurrentValue = GetResolvedKeyFrameValue(idxLastKeyFrame);
        }
        else if (currentTime == m_sortedResolvedKeyFrames[idxCurKeyFrame].m_resolvedKeyTime)
        {
            pCurrentValue = GetResolvedKeyFrameValue(idxCurKeyFrame);
        }
        else
        {
            TRefCountedPtr<T> pFromValue;
            float progress = 0;
            if (idxCurKeyFrame == 0)
            {
                if (get_IsAdditive())
                {
                    pFromValue = AnimatedReferenceTypeHelpers::GetZeroValue(pDefaultOriginValue);
                }
                else
                {
                    pFromValue = pDefaultOriginValue;
                }
                progress = currentTime.get_TotalMilliseconds() / m_sortedResolvedKeyFrames[0].m_resolvedKeyTime.get_TotalMilliseconds();
            }
            else
            {
                int lastFrame = idxCurKeyFrame - 1;
                TimeSpan lastFrameTime = m_sortedResolvedKeyFrames[lastFrame].m_resolvedKeyTime;
                pFromValue = GetResolvedKeyFrameValue(lastFrame);
                TimeSpan currentDelta = currentTime - lastFrameTime;
                TimeSpan overallDelta = m_sortedResolvedKeyFrames[idxCurKeyFrame].m_resolvedKeyTime - lastFrameTime;
                progress = currentDelta.get_TotalMilliseconds() / overallDelta.get_TotalMilliseconds();
            }
            pCurrentValue = GetResolvedKeyFrame(idxCurKeyFrame)->InterpolateValue(pFromValue, progress);
        }
        if (get_IsCumulative())
        {
            Nullable<int> nullable3 = pAnimationClock->get_CurrentIteration();
            Nullable<int> nullable2 = nullable3.get_HasValue() ? Nullable<int>(nullable3.get_ValueOrDefault() - 1) : Nullable<int>();
            float num5 = (float) nullable2.get_Value();
            if (num5 > 0)
            {
                pCurrentValue = AnimatedReferenceTypeHelpers::Add<T>(pCurrentValue, AnimatedReferenceTypeHelpers::Scale<T>(GetResolvedKeyFrameValue(idxLastKeyFrame), num5));
            }
        }
        if (get_IsAdditive())
        {
            return AnimatedReferenceTypeHelpers::Add<T>(pDefaultOriginValue, pCurrentValue);
        }
        return pCurrentValue;
    }

    KeyFrameType* GetResolvedKeyFrame(int idxKeyFrame)
    {
        return (*(get_KeyFrames()))[m_sortedResolvedKeyFrames[idxKeyFrame].m_originalKeyFrameIndex];
    }

    T* GetResolvedKeyFrameValue(int idxKeyFrame)
    {
        return GetResolvedKeyFrame(idxKeyFrame)->get_Value();
    }

private:

    TimeSpan get_LargestTimeSpanKeyTime() 
    {
        bool hasValidTime = false;
        TimeSpan largestTimeSpan = TimeSpan::get_Zero();

        KeyFrameCollectionType* pKeyFrames = get_KeyFrames();

        if (pKeyFrames != NULL)
        {
            int cKeys = pKeyFrames->get_Count();
            
            for (int idxKey = 0; idxKey < cKeys; idxKey++)
            {
                KeyTime keyTime = (*pKeyFrames)[idxKey]->get_KeyTime();
                if (keyTime.get_Type() == KeyTimeType::TimeSpan)
                {
                      hasValidTime = true;
                      if (keyTime.get_TimeSpan() > largestTimeSpan)
                      {
                            largestTimeSpan = keyTime.get_TimeSpan();
                      }
                }
            }
        }
        if (hasValidTime)
        {
              return largestTimeSpan;
        }
        return TimeSpan::FromSeconds(1);
    }

    struct ResolvedKeyFrameEntry
    {
        int m_originalKeyFrameIndex;
        TimeSpan m_resolvedKeyTime;
    };

    struct KeyTimeBlock
    { 
        int BeginIndex;
        int EndIndex;
    };

    void ResolveKeyTimes()
    {
        m_sortedResolvedKeyFrames.Clear();
        m_areKeyTimesValid = true;


        int cKeyFrames = 0;

        KeyFrameCollectionType* pKeyFrames = get_KeyFrames();
        if (pKeyFrames != NULL)
        {
            cKeyFrames = pKeyFrames->get_Count();
        }

        if (cKeyFrames == 0)
        {
            return;
        }

        m_sortedResolvedKeyFrames.AddEmptyItems(cKeyFrames);

        int idxCur = 0;
        while (idxCur < cKeyFrames)
        {
            m_sortedResolvedKeyFrames[idxCur].m_originalKeyFrameIndex = idxCur;
            idxCur++;
        }

        TimeSpan span3 = TimeSpan::get_Zero();

        Duration duration1 = Timeline::get_Duration();
        if (duration1.get_HasTimeSpan())
        {
            span3 = duration1.get_TimeSpan();
        }
        else
        {
            span3 = get_LargestTimeSpanKeyTime();
        }
        int num4 = cKeyFrames - 1;

        ItemStructList<KeyTimeBlock> blockList;

        idxCur = 0;
        while (idxCur < cKeyFrames)
        {
            KeyTime time2 = (*pKeyFrames)[idxCur]->get_KeyTime();
            switch (time2.get_Type())
            {
            case KeyTimeType::Uniform:
                {
                    if (idxCur != num4)
                    {
                        break;
                    }
                    m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = span3;
                    idxCur++;
                    continue;
                }
            case KeyTimeType::Percent:
                {
                    m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = TimeSpan::FromMilliseconds((int)(time2.get_Percent() * span3.get_TotalMilliseconds()));
                    idxCur++;
                    continue;
                }
            case KeyTimeType::TimeSpan:
                {
                    m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = time2.get_TimeSpan();
                    idxCur++;
                    continue;
                }
            default:
                {
                    continue;
                }
            }

            KeyTimeBlock block1;

            block1.BeginIndex = idxCur;
            while (++idxCur < num4)
            {
            }
            block1.EndIndex = idxCur;
            blockList.AddItem(block1);
        }
        for (int num3 = 0; num3 < (int)blockList.get_Count(); num3++)
        {
            KeyTimeBlock block2 = blockList[num3];
            TimeSpan span2 = TimeSpan::get_Zero();
            if (block2.BeginIndex > 0)
            {
                span2 = m_sortedResolvedKeyFrames[block2.BeginIndex - 1].m_resolvedKeyTime;
            }
            long num5 = (block2.EndIndex - block2.BeginIndex) + 1;
            TimeSpan span5 = m_sortedResolvedKeyFrames[block2.EndIndex].m_resolvedKeyTime - span2;
            TimeSpan span4 = TimeSpan::FromTicks(span5.get_Ticks() / num5);
            idxCur = block2.BeginIndex;
            TimeSpan span1 = span2 + span4;
            while (idxCur < block2.EndIndex)
            {
                m_sortedResolvedKeyFrames[idxCur].m_resolvedKeyTime = span1;
                span1 = span1 + span4;
                idxCur++;
            }
        }
//        Array.Sort<ResolvedKeyFrameEntry>(this._sortedResolvedKeyFrames);
    }     

    KeyFrameCollectionType m_keyFrames;
    ItemStructList<ResolvedKeyFrameEntry> m_sortedResolvedKeyFrames;
    bool m_areKeyTimesValid;
};




}; // namespace AVUI
