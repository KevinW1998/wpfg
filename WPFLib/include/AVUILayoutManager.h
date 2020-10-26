#pragma once

#include <AVUIEventableObject.h>
#include <AVUIUIElement.h>
#include <AVUIItemStructList.h>

namespace AVUI {

class LayoutManager : public EventableObject
{
    friend class Application;
    friend class WindowManager;
public:
    LayoutManager() : m_isInUpdateLayout(false) { }
    void AddToMeasureQueue(UIElement* pElement);
    void RemoveFromMeasureQueue(UIElement* pElement);
    void AddToArrangeQueue(UIElement* pElement);
    void RemoveFromArrangeQueue(UIElement* pElement);

    void Update();

    IMPLEMENT_EVENT(LayoutUpdated, EventArgs);

private:
    Rect get_ProperArrangeRect(UIElement* pElement);
    void FireLayoutUpdatedIfClean();

    void RemoveMeasureOrphans(UIElement* pElement);
    void RemoveArrangeOrphans(UIElement* pElement);
    bool CanRelyOnParentArrange(UIElement* pParent);
    bool CanRelyOnParentMeasure(UIElement* pParent);

    bool m_isInUpdateLayout;

    ItemStructList<UIElementPtr> m_measureQueue;
    ItemStructList<UIElementPtr> m_arrangeQueue;
};


}; // namespace AVUI
