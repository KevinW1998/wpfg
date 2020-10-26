#pragma once

#include <AVUIGridViewRowPresenterBase.h>

namespace AVUI {

class DataTemplate;
class DataTemplateSelector;
class ScrollViewer;
class GridViewColumnHeader;
class ItemsControl;
class Separator;
class ScrollChangedEventArgs;
class GridViewColumn;

class GridViewHeaderRowPresenter : public GridViewRowPresenterBase
{
    friend class GridViewColumnHeader;
public:
    GridViewHeaderRowPresenter();
    ~GridViewHeaderRowPresenter();

    DECLARE_ELEMENT(GridViewHeaderRowPresenter, GridViewRowPresenterBase);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderContainerStyle, Style, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderTemplate, DataTemplate, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderContextMenu, Object, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ColumnHeaderToolTip, Object, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AllowsColumnReorder, bool, true, PropertyFlag::None);

protected:

    Size MeasureOverride(const Size& constraint);
    Size ArrangeOverride(const Size& constraint);

    void OnPreApplyTemplate();

    void OnColumnPropertyChanged(GridViewColumn* pColumn, const Property* pProperty);

private:
    void MakeParentItemsControlGotFocus();

    GridViewColumnHeader* FindHeaderByColumn(GridViewColumn* pColumn);
    void RenewEvents();
    void OnMasterScrollChanged(Object* pSender, ScrollChangedEventArgs* pArgs);
    void OnHeaderScrollChanged(Object* pSender, ScrollChangedEventArgs* pArgs);
    void OnColumnHeadersPresenterKeyDown(Object* pSender, KeyEventArgs* pArgs);
    void AddPaddingColumnHeader();
    void AddFloatingHeader(GridViewColumnHeader* pSrcHeader);
    void AddIndicator();
    void RemoveHeader(GridViewColumnHeader* pHeader, int index);
    void BuildHeaderLinks();
    int GetVisualIndex(int columnIndex);
    GridViewColumnHeader* CreateAndInsertHeader(GridViewColumn* pColumn, int index);
    void PrepareHeaderDrag(GridViewColumnHeader* pHeader, const Point& pos, const Point& relativePos, bool cancelInvoke);
    static ItemsControl* FindItemsControlThroughTemplatedParent(GridViewHeaderRowPresenter* pPresenter);
    static bool IsMousePositionValid(UIElement* pFloatingHeader, const Point& currentPos, float arrange);
    int FindIndexByPosition(const Point& startPos, bool findNearestColumn);
    Point FindPositionByIndex(int index);
    void FinishHeaderDrag(bool isCancel);
    void UpdateHeaderContent(GridViewColumnHeader* pHeader);
    void UpdateHeader(GridViewColumnHeader* pHeader);
    void UpdatePaddingHeader(GridViewColumnHeader* pHeader);
    void UpdateHeaderProperty(GridViewColumnHeader* pHeader, const DependencyProperty* pProperty);
    void UpdateHeaderProperty(GridViewColumnHeader* pHeader, const DependencyProperty* pTargetDP, const DependencyProperty* pColumnDP, const DependencyProperty* pGVDP);
    void GetMatchingDPs(const DependencyProperty* indexDP, const DependencyProperty** ppGVDP, const DependencyProperty** ppColumnDP, const DependencyProperty** ppHeaderDP);
    void HookupItemsControlKeyboardEvent(GridViewColumnHeader* pHeader);
    void UnhookItemsControlKeyboardEvent(GridViewColumnHeader* pHeader);
    void OnLayoutUpdated(Object* pSender, EventArgs* e);

    static const DependencyProperty* GetPresenterDP(int row, int column);
     

    Point m_currentPos;
    int m_desColumnIndex;
    TRefCountedPtr<GridViewColumnHeader> m_pDraggingSrcHeader;
    TRefCountedPtr<GridViewColumnHeader> m_pFloatingHeader;
    ItemStructList<TRefCountedPtr<GridViewColumnHeader> > m_headers;
    bool m_areHeadersValid;
    ItemStructList<Rect> m_headersPositionList;
    ScrollViewer* m_pHeaderSV;
    TRefCountedPtr<Separator> m_pIndicator;
    bool m_isColumnChangedOrCreated;
    bool m_isHeaderDragging;
    ItemsControl* m_pItemsControl;
    ScrollViewer* m_pMainSV;
    TRefCountedPtr<GridViewColumnHeader> m_pPaddingHeader;
    bool m_prepareDragging;
    Point m_relativeStartPos;
    int m_startColumnIndex;
    Point m_startPos;
};

}; // namespace AVUI

