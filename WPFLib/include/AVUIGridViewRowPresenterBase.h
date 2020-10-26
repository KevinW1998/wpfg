
#pragma once

#include <AVUIFrameworkElement.h>
#include <AVUIUIElementCollection.h>

namespace AVUI {

class GridViewColumnCollection;
class GridViewColumn;
class GridViewColumnCollectionChangedEventArgs;
class CollectionChangedEventArgs;

class GridViewRowPresenterBase : public FrameworkElement
{
public:
    GridViewRowPresenterBase() : m_children(this), m_needUpdateVisualTree(true) { };
    ~GridViewRowPresenterBase();

    DECLARE_ELEMENT(GridViewRowPresenterBase, FrameworkElement);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Columns, GridViewColumnCollection, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, &GridViewRowPresenterBase::ColumnsPropertyChanged));

protected:
    bool get_NeedUpdateVisualTree() { return m_needUpdateVisualTree; }
    void set_NeedUpdateVisualTree(bool needUpdateVisualTree) { m_needUpdateVisualTree = needUpdateVisualTree; }

    unsigned int get_VisualChildCount() { return m_children.get_Count(); }
    Visual* get_VisualChild(unsigned int idx) { return m_children[idx]; }

    void EnsureDesiredWidthList();

    UIElementCollection* get_InternalChildren() { return &m_children; }
    virtual void OnColumnPropertyChanged(GridViewColumn* pColumn, const Property* pProperty) = 0;
    virtual void OnColumnCollectionChanged(GridViewColumnCollectionChangedEventArgs* pArgs);

    ItemStructList<float> m_desiredWidthList;

private:

    bool get_IsPresenterVisualReady() { return get_IsInitialized() && !get_NeedUpdateVisualTree(); }
    void ColumnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArg);
    static void ColumnsPropertyChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    DependencyObject* GetStableAncestor();

    UIElementCollection m_children;

    bool m_needUpdateVisualTree : 1;

};

}; // namespace AVUI

