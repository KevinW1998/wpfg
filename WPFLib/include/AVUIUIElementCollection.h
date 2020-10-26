#pragma once

#include <AVUIUIElement.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class UIElementCollection : public TObjectCollection<UIElement>
{
    friend class Panel;
public:
    DECLARE_ELEMENT(UIElementCollection, ObjectCollectionBase);
    UIElementCollection() : m_pOwner(NULL), m_isNotLogical(false) { }
    UIElementCollection(UIElement* pOwner) : m_pOwner(pOwner), m_isNotLogical(false) { }
    ~UIElementCollection() { Clear(); }

protected:
    void OnAdd(UIElement* pAdd);
    void OnRemove(UIElement* pRemove);

private:

    void set_IsNotLogical(bool isNotLogical) { m_isNotLogical = isNotLogical; }
    bool get_IsNotLogical() { return m_isNotLogical; }


    UIElement* m_pOwner;
    bool m_isNotLogical;
};


}; // namespace AVUI
