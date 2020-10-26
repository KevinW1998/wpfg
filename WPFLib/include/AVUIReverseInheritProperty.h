#pragma once

#include <AVUIObject.h>

namespace AVUI {

class DeferredElementTreeState;
class UIElement;

class ReverseInheritProperty : public Object
{
public:
    ReverseInheritProperty(const DependencyProperty* pProperty, unsigned int flagCache, unsigned int flagChanged) : m_pProperty(pProperty), m_flagCache(flagCache), m_flagChanged(flagChanged)
    {

    }


    virtual void FireNotifications(UIElement* pUIE, bool oldValue) = 0;

    void OnOriginValueChanged(DependencyObject* pOldOrigin, DependencyObject* pNewOrigin, DeferredElementTreeState** ppOldTreeState);

private:

    void FirePropertyChangeInAncestry(DependencyObject* pElement, bool oldValue, DeferredElementTreeState* pTreeState);
    bool IsFlagSet(DependencyObject* pObject, unsigned int flag);
    void SetCacheFlagInAncestry(DependencyObject* pElement, bool newValue, DeferredElementTreeState* pTreeState);
    void SetFlag(DependencyObject* pDO, unsigned int flag, bool value);
  
    const DependencyProperty* m_pProperty;
    unsigned int m_flagCache;
    unsigned int m_flagChanged;

};

}; // namespace AVUI
