#include <AVUICommon.h>
#include <AVUIClockGroup.h>
#include <AVUIPrefixSubtreeEnumerator.h>

namespace AVUI {


PrefixSubtreeEnumerator::PrefixSubtreeEnumerator(Clock* pRoot, bool processRoot)
{
      m_pRootClock = pRoot;
      m_pCurrentClock = NULL;
      m_subtreeFlags = processRoot ? (Flags_ProcessRoot | Flags_Reset) : Flags_Reset;
}


void PrefixSubtreeEnumerator::Reset()
{
      m_pCurrentClock = NULL;
      m_subtreeFlags = (m_subtreeFlags & Flags_ProcessRoot) | Flags_Reset;
}

void PrefixSubtreeEnumerator::SkipSubtree()
{
      m_subtreeFlags |= Flags_SkipSubtree;
}

bool PrefixSubtreeEnumerator::MoveNext()
{
      if (m_subtreeFlags & Flags_Reset)
      {
            if (m_subtreeFlags & Flags_ProcessRoot)
            {
                  m_pCurrentClock = m_pRootClock;
            }
            else if (m_pRootClock != NULL)
            {
                  ClockGroup* pClockGroup = object_cast<ClockGroup>(m_pRootClock);
                  
                  if (pClockGroup != NULL)
                  {
                        m_pCurrentClock = pClockGroup->get_FirstChild();
                  }
                  else
                  {
                        m_pCurrentClock = NULL;
                  }
            }
            m_subtreeFlags &= ~Flags_Reset;
      }
      else if (m_pCurrentClock != NULL)
      {
            ClockGroup* pClockGroup = object_cast<ClockGroup>(m_pCurrentClock);
            Clock* pClock = (pClockGroup == NULL) ? NULL : pClockGroup->get_FirstChild();
            if (m_subtreeFlags & Flags_SkipSubtree || pClock == NULL)
            {
                  while (m_pCurrentClock != m_pRootClock && (pClock = m_pCurrentClock->get_NextSibling()) == NULL)
                  {
                        m_pCurrentClock = m_pCurrentClock->get_InternalParent();
                  }
                  if (m_pCurrentClock == m_pRootClock)
                  {
                        pClock = NULL;
                  }
                  m_subtreeFlags &= ~Flags_SkipSubtree;
            }
            m_pCurrentClock = pClock;
      }
      return (m_pCurrentClock != NULL);
}

 


 


 


 


}; // namespace AVUI
