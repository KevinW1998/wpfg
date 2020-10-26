#include <AVUICommon.h>

#include <AVUIDependentList.h>
#include <AVUIDependencyObject.h>

namespace AVUI {

void DependentList::InvalidateDependents(DependencyObject* pSource, const DependencyPropertyChangedEventArgs& eventArgs)
{
    for(int i = 0; i < m_dependentStorage.get_Count(); i++)
    {
        Expression* pExpression = m_dependentStorage[i].pExpression;

        if(pExpression != NULL)
        {
            pExpression->OnPropertyInvalidation(pSource, &eventArgs);

            if(!pExpression->get_ForwardsInvalidations() && m_dependentStorage[i].pDependencyObject != NULL)
            {
                if(eventArgs.IsSubPropertyChange)
                {
                    m_dependentStorage[i].pDependencyObject->InvalidateSubProperty(*(m_dependentStorage[i].pDependencyProperty));
                }
                else
                {
                    m_dependentStorage[i].pDependencyObject->InvalidateProperty(*(m_dependentStorage[i].pDependencyProperty));
                }
            }
        }
    }
}

void DependentList::Add(DependencyObject* pDO,  const DependencyProperty& dp, Expression* pExpression)
{
    m_dependentStorage.AddItem(Dependent(pDO, pExpression, &dp));
}
void DependentList::Remove(DependencyObject* pDO, const DependencyProperty& dp, Expression* pExpression)
{
    for(int i = 0; i < m_dependentStorage.get_Count(); i++)
    {
        if(m_dependentStorage[i].pDependencyObject == pDO &&
           m_dependentStorage[i].pDependencyProperty == &dp &&
           m_dependentStorage[i].pExpression == pExpression)
        {
            m_dependentStorage.RemoveAt(i);
            i--;
        }
    }
}


}; // namespace AVUI
