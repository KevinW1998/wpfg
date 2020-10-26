#pragma once

#include <AVUIFrameworkElement.h>
#include <AVUIUIElementCollection.h>

namespace AVUI {

class ItemContainerGenerator;
class ItemsChangedEventArgs;
struct GeneratorPosition;

class Panel : public FrameworkElement
{
    friend class VirtualizingPanel;
    friend class ItemsControl;
public:
    Panel(bool isVirtualizing = false);
    virtual ~Panel();

    virtual unsigned int get_VisualChildCount() { return get_Children()->get_Count(); }
    virtual Visual* get_VisualChild(unsigned int idx);

    DECLARE_ELEMENT(Panel, FrameworkElement);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Background, Brush, NULL, PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Children, UIElementCollection, true);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsItemsHost, bool, false, DependencyPropertyMetadata(PropertyFlag::None, Panel::OnIsItemsHostChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ZIndex, int, 0, DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, Panel::OnZIndexChanged));

    UIElementCollection* get_Children();

protected:
    virtual void OnRender(DrawingContext& drawingContext);
    virtual void OnChildrenChanged();
    void OnIsItemsHostChanged(bool oldIsItemsHost, bool newIsItemsHost);

    static void OnIsItemsHostChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnZIndexChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    static int GetZIndex(DependencyObject* pDO);
    void RecomputeZState();
    virtual void OnVisualChildrenChanged(DependencyObject* pChildAdded, DependencyObject* pChildRemoved);

    void set_IsZStateDirty(bool isDirty) { m_isZStateDirty = isDirty; }
    bool get_IsZStateDirty() { return m_isZStateDirty; }

    void set_IsZStateDiverse(bool isDiverse) { m_isZStateDiverse = isDiverse; }
    bool get_IsZStateDiverse() { return m_isZStateDiverse; }

    void InvalidateZOrder();

    class PanelChildrenCollection : public UIElementCollection
    {
    public:
        PanelChildrenCollection(Panel* pOwner) : m_pOwner(pOwner), UIElementCollection(pOwner) { }
    
    private:
        virtual void OnCollectionChanged() { if(m_pOwner != NULL) m_pOwner->OnChildrenChanged(); }
    
        Panel* m_pOwner;
    };

    virtual void OnClearChildrenInternal() { }
    virtual void OnItemsChangedInternal(Object* pSender, ItemsChangedEventArgs* pArgs);    

    virtual bool get_HasLogicalOrientation() { return false; }
    virtual Orientation::Enum get_LogicalOrientation() { return Orientation::Vertical; }

private:
    void OnItemsChangedCallback(Object* pSender, ItemsChangedEventArgs* pArgs);
    void addChildren(const GeneratorPosition& pos, int itemCount);
    void removeChildren(const GeneratorPosition& pos, int itemCount);
    void replaceChildren(const GeneratorPosition& pos, int itemCount);
    void moveChildren(const GeneratorPosition& fromPos, const GeneratorPosition& toPos, int itemCount);
    void reGenerateChildren();
    void VerifyBoundState();
    ItemContainerGenerator* getGenerator();

 

    bool get_IsVirtualizing()
    {
        return m_isVirtualizing;
    }

    bool m_isItemsHostCached : 1;
    bool m_isVirtualizing : 1;
    bool m_isZStateDirty : 1;
    bool m_isZStateDiverse : 1;
    int m_zConsonant;
    PanelChildrenCollection m_children;
    ItemStructList<int> m_zLut;

    TRefCountedPtr<ItemContainerGenerator> m_pItemContainerGenerator;
};

}; // namespace AVUI
