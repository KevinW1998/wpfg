#pragma once

namespace AVUI {

class Clock;

class PrefixSubtreeEnumerator
{
public:
    PrefixSubtreeEnumerator(Clock* pRoot, bool processRoot);
    bool MoveNext();
    void Reset();
    void SkipSubtree();
    Clock* get_Current() { return m_pCurrentClock; };
private:
    enum SubtreeFlags
    {
        Flags_Reset = 1,
        Flags_ProcessRoot = 2,
        Flags_SkipSubtree = 4
    };

    Clock* m_pRootClock;
    Clock* m_pCurrentClock;
    int m_subtreeFlags;

};

};
