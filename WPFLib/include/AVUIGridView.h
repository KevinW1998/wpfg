#pragma once

#include <AVUIViewBase.h>

namespace AVUI {

class GridViewColumnCollection;
class GridViewHeaderRowPresenter;
class Style;
class DataTemplate;
class DataTemplateSelector;

class GridView : public ViewBase
{
    friend class GridViewHeaderRowPresenter;
public:
    GridView() : m_pColumns(NULL), m_pHeaderRowPresenter(NULL) { };

    DECLARE_ELEMENT(GridView, ViewBase);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnCollection, GridViewColumnCollection, NULL, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderContainerStyle, Style, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderTemplate, DataTemplate, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AllowsColumnReorder, bool, true, PropertyFlag::None);


    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Columns, GridViewColumnCollection, true);
    // Context menu? tooltip?

    GridViewColumnCollection* get_Columns();

protected:

    virtual void ClearItem(ListViewItem* pItem);
    virtual void PrepareItem(ListViewItem* pItem);

private:

    void set_HeaderRowPresenter(GridViewHeaderRowPresenter* pHeaderPresenter) { m_pHeaderRowPresenter = pHeaderPresenter; }

    TRefCountedPtr<GridViewColumnCollection> m_pColumns;
    GridViewHeaderRowPresenter* m_pHeaderRowPresenter;
};

 


}; // namespace AVUI

