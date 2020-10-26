#pragma once

#include <AVUIGridViewRowPresenterBase.h>
#include <AVUIContentControl.h>

namespace AVUI {

class GridViewColumn;
class ItemsControl;

class GridViewRowPresenter : public GridViewRowPresenterBase
{
public:
    GridViewRowPresenter() : m_viewPortValid(false), m_isOnCurrentPageValid(true) { };
    ~GridViewRowPresenter() { };

    DECLARE_ELEMENT(GridViewRowPresenter, GridViewRowPresenterBase);

    static const AVUI::DependencyProperty& get_ContentProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, OnContentChanged));
        const static AVUI::DependencyProperty& property = ContentControl::get_ContentProperty().AddOwner(GET_TYPE(_thisType), NULL, &metadata);

        return property;
    }

    Object* get_Content() { return GetValue(get_ContentProperty()); }
    void set_Content(Object* pValue) { SetValue(get_ContentProperty(), pValue); }

protected:

    Size MeasureOverride(const Size& constraint);
    Size ArrangeOverride(const Size& constraint);

    void OnPreApplyTemplate();

    void OnColumnPropertyChanged(GridViewColumn* pColumn, const Property* pProperty);

private:

    static ItemsControl* FindItemsControlThroughTemplatedParent(GridViewRowPresenter* pPresenter);
    void RenewCell(int index, GridViewColumn* pColumn);

    void OnLayoutUpdated(Object* pSender, EventArgs* pArgs);

    bool get_IsOnCurrentPage() { return true; }

    TRefCountedPtr<UIElement> CreateCell(GridViewColumn* pColumn);

    static void OnContentChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e) { ((GridViewRowPresenter*)pDO)->set_NeedUpdateVisualTree(true); }

    bool m_viewPortValid : 1;
    bool m_isOnCurrentPageValid : 1;
};

}; // namespace AVUI
