#include <AVUICommon.h>
#include <AVUIPanel.h>
#include <AVUIItemsControl.h>
#include <AVUIItemContainerGenerator.h>

REGISTER_ELEMENT_BEGIN(AVUI::Panel)

    REGISTER_PROPERTY(Background);
    REGISTER_PROPERTY(Children);
    REGISTER_PROPERTY(ZIndex);
    REGISTER_PROPERTY(IsItemsHost);
REGISTER_ELEMENT_END()

namespace AVUI {

Panel::Panel(bool isVirtualizing) : m_children(this), m_isZStateDirty(false), m_isZStateDiverse(false), m_zConsonant(0), m_isVirtualizing(isVirtualizing), m_isItemsHostCached(false)
{

}

Panel::~Panel()
{
    m_children.InvalidateWeakReferences();
}

void Panel::OnRender(DrawingContext& drawingContext)
{
    Brush* pBrush = get_Background();

    if(pBrush != NULL)
    {
        drawingContext.DrawRoundedRectangle(pBrush, NULL, Rect(0.0f, 0.0f, get_RenderSize().get_Width(), get_RenderSize().get_Height()), 0.0f, 0.0f);
    }
}

void Panel::OnChildrenChanged()
{
    InvalidateMeasure();
}

void Panel::OnZIndexChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    int oldValue = UnboxValue<int>(e.pOldValue);
    int newValue = UnboxValue<int>(e.pNewValue);

    UIElement* pElement = object_cast<UIElement>(pDO);
    if (pElement != NULL)
    {
        Panel* pPanel = object_cast<Panel>(pElement->get_VisualParent());
        if (pPanel != NULL)
        {
            if(!pPanel->get_IsZStateDirty())
            {
                pPanel->InvalidateZOrder();
            }
            pPanel->set_IsZStateDirty(true);
        }
    }

}

void Panel::InvalidateZOrder()
{
    // Invalidate hit test
}

Visual* Panel::get_VisualChild(unsigned int idx) 
{ 
    if(get_IsZStateDirty())
    {
        RecomputeZState();
    }

    unsigned int childIdx = idx;
    if(m_zLut.get_Count() > 0)
    {
        childIdx = m_zLut[idx];
    }

    return (*get_Children())[childIdx]; 
};

void Panel::RecomputeZState() 
{
    int capacity = m_children.get_Count();
    bool isZStateDiverse = false;
    bool hasZOrder = false;
    int defaultValue = 0;
    int currentZIndex = defaultValue;

    ItemStructList<int> list;
    if (capacity > 0)
    {
        if (m_children[0] != NULL)
        {
            currentZIndex = GetZIndex(m_children[0]);
        }

        if (capacity > 1)
        {
            list.AddItem(currentZIndex << 0x10);
            int lastZIndex = currentZIndex;
            int curIdx = 1;
            do
            {
                int newZIndex = (m_children[curIdx] != NULL) ? GetZIndex(m_children[curIdx]) : defaultValue;
                list.AddItem((newZIndex << 0x10) + curIdx);
                hasZOrder |= newZIndex < lastZIndex;
                lastZIndex = newZIndex;
                isZStateDiverse |= newZIndex != currentZIndex;
            }
            while (++curIdx < capacity);
        }
    }

    m_zLut.Clear();

    if (hasZOrder)
    {
        list.Sort();
        for (int i = 0; i < capacity; i++)
        {
            m_zLut.AddItem(list[i] & 0xffff);
        }
    }
    else
    {
        m_zLut.Clear();
    }

    set_IsZStateDiverse(isZStateDiverse);
    m_zConsonant = currentZIndex;
    set_IsZStateDirty(false);
}


void Panel::OnVisualChildrenChanged(DependencyObject* pChildAdded, DependencyObject* pChildRemoved)
{
    if (!get_IsZStateDirty())
    {
        if (get_IsZStateDiverse())
        {
            set_IsZStateDirty(true);
        }
        else if (pChildAdded != NULL)
        {
            int newZIndex = GetZIndex(pChildAdded);
            if (newZIndex != m_zConsonant)
            {
                set_IsZStateDirty(true);
            }
        }
    }
    Visual::OnVisualChildrenChanged(pChildAdded, pChildRemoved);
}


int Panel::GetZIndex(DependencyObject* pDO)
{
    return UnboxValue<int>(pDO->GetValue(get_ZIndexProperty()));
}

UIElementCollection* Panel::get_Children()
{
    VerifyBoundState();

    if (m_isItemsHostCached && m_pItemContainerGenerator == NULL)
    {
        m_children.set_IsNotLogical(true);
        reGenerateChildren();
    }
    return &m_children;
}

void Panel::VerifyBoundState()
{
    if (!m_isItemsHostCached && m_pItemContainerGenerator != NULL)
    {
        m_pItemContainerGenerator->remove_ItemsChanged(ItemsChangedEventHandler::Create(this, &Panel::OnItemsChangedCallback));
        m_pItemContainerGenerator->RemoveAll();
        m_pItemContainerGenerator = NULL;

        if(m_children.get_Count() != 0)
        {
            m_children.Clear();
            OnClearChildrenInternal();
        }
    }

    if (m_isItemsHostCached && m_pItemContainerGenerator == NULL && m_children.get_Count() != 0)
    {
        m_children.Clear();
        OnClearChildrenInternal();
    }
}

void Panel::OnItemsChangedCallback(Object* pSender, ItemsChangedEventArgs* pArgs)
{
    VerifyBoundState();
    if (get_IsItemsHost())
    {
        if (m_pItemContainerGenerator == NULL)
        {
            LibraryCriticalError();
        }
        OnItemsChangedInternal(pSender, pArgs);
        InvalidateMeasure();
    }
}

void Panel::OnItemsChangedInternal(Object* pSender, ItemsChangedEventArgs* pArgs)
{
   switch (pArgs->get_Action())
   {
   case NotifyCollectionChangedAction::Add:
        addChildren(pArgs->get_Position(), pArgs->get_ItemCount());
        break;

   case NotifyCollectionChangedAction::Remove:
        removeChildren(pArgs->get_Position(), pArgs->get_ItemUICount());
        break;

   case NotifyCollectionChangedAction::Replace:
        replaceChildren(pArgs->get_Position(), pArgs->get_ItemCount());
        break;

   case NotifyCollectionChangedAction::Move:
        moveChildren(pArgs->get_OldPosition(), pArgs->get_Position(), pArgs->get_ItemUICount());
        break;

   case NotifyCollectionChangedAction::Reset:
        reGenerateChildren();
        break;
   }
}

void Panel::addChildren(const GeneratorPosition& pos, int itemCount)
{
    ItemContainerGenerator* pItemContainerGeneratorForPanel = getGenerator()->GetItemContainerGeneratorForPanel(this);

    TRefCountedPtr<Object> pSentinel = pItemContainerGeneratorForPanel->StartAt(pos, GeneratorDirection::Forward);

    for (int i = 0; i < itemCount; i++)
    {
        TRefCountedPtr<UIElement> pElement = object_cast<UIElement>(pItemContainerGeneratorForPanel->GenerateNext());
        if (pElement != NULL)
        {
            m_children.Insert((pos.Index + 1) + i, pElement);
            pItemContainerGeneratorForPanel->PrepareItemContainer(pElement);
        }
    }
}

void Panel::removeChildren(const GeneratorPosition& pos, int itemCount)
{
    m_children.RemoveRange(pos.Index, itemCount);
}

void Panel::replaceChildren(const GeneratorPosition& pos, int itemCount)
{
    ItemContainerGenerator* pItemContainerGeneratorForPanel = getGenerator()->GetItemContainerGeneratorForPanel(this);

    TRefCountedPtr<Object> pSentinel = pItemContainerGeneratorForPanel->StartAt(pos, GeneratorDirection::Forward);

    for (int i = 0; i < itemCount; i++)
    {
        TRefCountedPtr<UIElement> pItem = object_cast<UIElement>(pItemContainerGeneratorForPanel->GenerateNext());
        if (pItem != NULL)
        {
            m_children.SetItemAt(pos.Index + 1 + i, pItem);
            pItemContainerGeneratorForPanel->PrepareItemContainer(pItem);
        }
    }
}

void Panel::moveChildren(const GeneratorPosition& fromPos, const GeneratorPosition& toPos, int itemCount)
{
    if (fromPos.Index != toPos.Index || fromPos.Offset != toPos.Offset)
    {
        int num = getGenerator()->GetItemContainerGeneratorForPanel(this)->IndexFromGeneratorPosition(toPos);
        ItemStructList<TRefCountedPtr<UIElement> > elementArray;

        for (int i = 0; i < itemCount; i++)
        {
            elementArray.AddItem(m_children[fromPos.Index + 1]);
        }
        m_children.RemoveRange(fromPos.Index, itemCount);
        for (int j = 0; j < itemCount; j++)
        {
            m_children.Insert(num + j, elementArray[j]);
        }
    }
}



void Panel::reGenerateChildren()
{
    if (m_pItemContainerGenerator == NULL)
    {
        m_pItemContainerGenerator = getGenerator()->GetItemContainerGeneratorForPanel(this);
        m_pItemContainerGenerator->add_ItemsChanged(ItemsChangedEventHandler::Create(this, &Panel::OnItemsChangedCallback));
        m_pItemContainerGenerator->RemoveAll();
    }
    if (m_children.get_Count() > 0)
    {
        m_pItemContainerGenerator->RemoveAll();
        m_children.Clear();
        OnClearChildrenInternal();
    }

    if (!get_IsVirtualizing())
    {
        TRefCountedPtr<Object> pSentinel = m_pItemContainerGenerator->StartAt(GeneratorPosition(-1, 0), GeneratorDirection::Forward);
        TRefCountedPtr<UIElement> pElement;
        while ((pElement = object_cast<UIElement>(m_pItemContainerGenerator->GenerateNext())) != NULL)
        {
            m_children.Add(pElement);
            m_pItemContainerGenerator->PrepareItemContainer(pElement);
        }
    }
}

ItemContainerGenerator* Panel::getGenerator()
{
    ItemsControl* pItemsOwner = ItemsControl::GetItemsOwner(this);
    if (pItemsOwner == NULL)
    {
        LibraryCriticalError();
    }
    return pItemsOwner->get_ItemContainerGenerator();
}

void Panel::OnIsItemsHostChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((Panel*) pDO)->OnIsItemsHostChanged(UnboxValue<bool>(e.pOldValue), UnboxValue<bool>(e.pNewValue));
}

void Panel::OnIsItemsHostChanged(bool oldIsItemsHost, bool newIsItemsHost)
{
    m_isItemsHostCached = newIsItemsHost;

    ItemsControl* pItemsOwner = ItemsControl::GetItemsOwner(this);
    if (pItemsOwner != NULL)
    {
        ItemContainerGenerator* pItemContainerGenerator = pItemsOwner->get_ItemContainerGenerator();
        if (pItemContainerGenerator != NULL && pItemContainerGenerator == pItemContainerGenerator->GetItemContainerGeneratorForPanel(this))
        {
            pItemsOwner->set_ItemsHost(this);
        }
    }
    VerifyBoundState();    
}


}; // namespace AVUI
