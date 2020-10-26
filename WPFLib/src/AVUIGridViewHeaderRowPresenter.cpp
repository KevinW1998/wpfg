#include <AVUICommon.h>
#include <AVUIGridViewHeaderRowPresenter.h>
#include <AVUIGridViewColumnCollection.h>
#include <AVUIScrollViewer.h>
#include <AVUIListView.h>
#include <AVUIGridView.h>
#include <AVUIGridViewColumnHeader.h>
#include <AVUISeparator.h>
#include <AVUIStyle.h>
#include <AVUILayoutManager.h>
#include <AVUIIScrollInfo.h>
#include <AVUIContentPresenter.h>
#include <AVUIListBoxItem.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridViewHeaderRowPresenter)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(ColumnHeaderContainerStyle)
    REGISTER_PROPERTY(ColumnHeaderTemplate)
    REGISTER_PROPERTY(ColumnHeaderTemplateSelector)
    REGISTER_PROPERTY(AllowsColumnReorder)
REGISTER_ELEMENT_END()


namespace AVUI
{

const DependencyProperty* GridViewHeaderRowPresenter::GetPresenterDP(int row, int column)
{
    static const DependencyProperty* s_DPList[3][5] =
    {
        { &get_ColumnHeaderContainerStyleProperty(), &get_ColumnHeaderTemplateProperty(), &get_ColumnHeaderTemplateSelectorProperty(), &get_ColumnHeaderContextMenuProperty(), &get_ColumnHeaderToolTipProperty() },
        { &GridViewColumn::get_HeaderContainerStyleProperty(), &GridViewColumn::get_HeaderTemplateProperty(), &GridViewColumn::get_HeaderTemplateSelectorProperty(), NULL, NULL },
        { &UIElement::get_StyleProperty(), &ContentControl::get_ContentTemplateProperty(), &ContentControl::get_ContentTemplateSelectorProperty(), &UIElement::get_ContextMenuProperty(), &UIElement::get_ToolTipProperty() }
    };

    return s_DPList[row][column];
}

GridViewHeaderRowPresenter::GridViewHeaderRowPresenter() : m_desColumnIndex(0), m_areHeadersValid(false), m_pHeaderSV(NULL), m_isColumnChangedOrCreated(false),
                                                           m_isHeaderDragging(false), m_pItemsControl(NULL), m_pMainSV(NULL), m_pPaddingHeader(NULL),
                                                           m_prepareDragging(false), m_startColumnIndex(0)
{

}

GridViewHeaderRowPresenter::~GridViewHeaderRowPresenter()
{
    UIElementCollection* pChildren = get_InternalChildren();

    int num = pChildren->get_Count();
    for (int i = 0; i < num; i++)
    {
        GridViewColumnHeader* pHeader = object_cast<GridViewColumnHeader>(pChildren->GetItemAt(i));
        if(pHeader != NULL)
        {
            UnhookItemsControlKeyboardEvent(pHeader);
        }
    }
}

void GridViewHeaderRowPresenter::OnPreApplyTemplate()
{
    GridViewRowPresenterBase::OnPreApplyTemplate();
    if (get_NeedUpdateVisualTree())
    {
        UIElementCollection* pChildren = get_InternalChildren();
        GridViewColumnCollection* pColumns = get_Columns();

        RenewEvents();
        if (pChildren->get_Count() == 0)
        {
            AddPaddingColumnHeader();
            AddIndicator();
            AddFloatingHeader(NULL);
        }
        else if (pChildren->get_Count() > 3)
        {
            int num = pChildren->get_Count() - 3;
            for (int i = 0; i < num; i++)
            {
                RemoveHeader(NULL, 1);
            }
        }
        UpdatePaddingHeader(m_pPaddingHeader);
        if (pColumns != NULL)
        {
            int num3 = 1;
            for (int j = pColumns->get_Count() - 1; j >= 0; j--)
            {
                GridViewColumn* pColumn = (*pColumns)[j];
                CreateAndInsertHeader(pColumn, num3++);
            }
        }
        BuildHeaderLinks();
        set_NeedUpdateVisualTree(false);
        m_isColumnChangedOrCreated = true;
    }    
}


void GridViewHeaderRowPresenter::RenewEvents()
{
    ScrollViewer* pOldScrollViewer = m_pHeaderSV;
    m_pHeaderSV = object_cast<ScrollViewer>(get_Parent());
    if (pOldScrollViewer != m_pHeaderSV)
    {
        if (pOldScrollViewer != NULL)
        {
            pOldScrollViewer->remove_ScrollChanged(ScrollChangedEventHandler::Create(this, &GridViewHeaderRowPresenter::OnHeaderScrollChanged));
        }
        if (m_pHeaderSV != NULL)
        {
            m_pHeaderSV->add_ScrollChanged(ScrollChangedEventHandler::Create(this, &GridViewHeaderRowPresenter::OnHeaderScrollChanged));
        }
    }

    pOldScrollViewer = m_pMainSV;
    m_pMainSV = object_cast<ScrollViewer>(get_TemplatedParent());
    if (pOldScrollViewer != m_pMainSV)
    {
        if (pOldScrollViewer != NULL)
        {
            pOldScrollViewer->remove_ScrollChanged(ScrollChangedEventHandler::Create(this, &GridViewHeaderRowPresenter::OnMasterScrollChanged));
        }
        if (m_pMainSV != NULL)
        {
            m_pMainSV->add_ScrollChanged(ScrollChangedEventHandler::Create(this, &GridViewHeaderRowPresenter::OnMasterScrollChanged));
        }
    }
    ItemsControl* pOldItemsControl = m_pItemsControl;
    m_pItemsControl = FindItemsControlThroughTemplatedParent(this);
    if (pOldItemsControl != m_pItemsControl)
    {
        if (pOldItemsControl != NULL)
        {
            pOldItemsControl->remove_KeyDown(KeyEventHandler::Create(this, &GridViewHeaderRowPresenter::OnColumnHeadersPresenterKeyDown));
        }
        if (m_pItemsControl != NULL)
        {
            m_pItemsControl->add_KeyDown(KeyEventHandler::Create(this, &GridViewHeaderRowPresenter::OnColumnHeadersPresenterKeyDown));
        }
    }

    ListView* pListView = object_cast<ListView>(m_pItemsControl);
    if (pListView != NULL && pListView->get_View() != NULL && pListView->get_View()->IsType(GET_TYPE(GridView)))
    {
        ((GridView*) (pListView->get_View()))->set_HeaderRowPresenter(this);
    }
}

void GridViewHeaderRowPresenter::AddPaddingColumnHeader()
{
    TRefCountedPtr<GridViewColumnHeader> pElement = object_allocate<GridViewColumnHeader>();
    pElement->set_IsInternalGenerated(true);
    pElement->set_Role(GridViewColumnHeaderRole::Padding);
    pElement->set_Content(NULL);
    pElement->set_ContentTemplate(NULL);
    pElement->set_ContentTemplateSelector(NULL);
    pElement->set_MinWidth(0);
    pElement->set_Padding(Thickness(0));
    pElement->set_Width(Float::get_NaN());
    pElement->set_HorizontalAlignment(HorizontalAlignment::Stretch);
    get_InternalChildren()->Add(pElement);

    m_pPaddingHeader = pElement;
}

Size GridViewHeaderRowPresenter::MeasureOverride(const Size& constraint)
{
    GridViewColumnCollection* pColumns = get_Columns();
    UIElementCollection* pChildren = get_InternalChildren();
    float num = 0;
    float width = 0;
    float height = constraint.get_Height();
    bool flag = false;
    if (pColumns != NULL)
    {
        for (int i = 0; i < pColumns->get_Count(); i++)
        {
            UIElement* pElement = (*pChildren)[GetVisualIndex(i)];
            if (pElement != NULL)
            {
                float num5 = Float::Max(0, constraint.get_Width() - width);
                GridViewColumn* pColumn = (*pColumns)[i];
                if (pColumn->get_State() == ColumnMeasureState::Init)
                {
                    if (!flag)
                    {
                        EnsureDesiredWidthList();

                        get_LayoutManager()->add_LayoutUpdated(EventHandler::Create(this, &GridViewHeaderRowPresenter::OnLayoutUpdated));

                        flag = true;
                    }
                    pElement->Measure(Size(num5, height));
                    m_desiredWidthList[pColumn->get_ActualIndex()] = pColumn->EnsureWidth(pElement->get_DesiredSize().get_Width());
                    width += pColumn->get_DesiredWidth();
                }
                else if (pColumn->get_State() == ColumnMeasureState::Headered || pColumn->get_State() == ColumnMeasureState::Data)
                {
                    num5 = Float::Min(num5, pColumn->get_DesiredWidth());
                    pElement->Measure(Size(num5, height));
                    width += pColumn->get_DesiredWidth();
                }
                else
                {
                    num5 = Float::Min(num5, pColumn->get_Width());
                    pElement->Measure(Size(num5, height));
                    width += pColumn->get_Width();
                }
                num = Float::Max(num, pElement->get_DesiredSize().get_Height());
            }
        }
    }
    m_pPaddingHeader->Measure(Size(0, height));
    num = Float::Max(num, m_pPaddingHeader->get_DesiredSize().get_Height());
    width += 2;
    if (m_isHeaderDragging)
    {
        m_pIndicator->Measure(constraint);
        m_pFloatingHeader->Measure(constraint);
    }
    return Size(width, num);
}


Size GridViewHeaderRowPresenter::ArrangeOverride(const Size& arrangeSize)
{
    Rect finalRect;
    GridViewColumnCollection* pColumns = get_Columns();
    UIElementCollection* pChildren = get_InternalChildren();
    float x = 0;
    float width = arrangeSize.get_Width();

    m_headersPositionList.Clear();
    if (pColumns != NULL)
    {
        for (int i = 0; i < pColumns->get_Count(); i++)
        {
            UIElement* pElement = (*pChildren)[GetVisualIndex(i)];
            if (pElement != NULL)
            {
                GridViewColumn* pColumn = (*pColumns)[i];
                float num4 = Float::Min(width, (pColumn->get_State() == ColumnMeasureState::SpecificWidth) ? pColumn->get_Width() : pColumn->get_DesiredWidth());
                finalRect = Rect(x, 0, num4, arrangeSize.get_Height());
                pElement->Arrange(finalRect);
                m_headersPositionList.AddItem(finalRect);
                width -= num4;
                x += num4;
            }
        }
        if (m_isColumnChangedOrCreated)
        {
            for (int j = 0; j < pColumns->get_Count(); j++)
            {
                ((GridViewColumnHeader*) (*pChildren)[GetVisualIndex(j)])->CheckWidthForPreviousHeaderGripper();
            }
            m_pPaddingHeader->CheckWidthForPreviousHeaderGripper();
            m_isColumnChangedOrCreated = false;
        }
    }
    finalRect = Rect(x, 0, Float::Max(width, 0), arrangeSize.get_Height());
    m_pPaddingHeader->Arrange(finalRect);
    m_headersPositionList.AddItem(finalRect);
    if (m_isHeaderDragging)
    {
        Rect rect2 = m_headersPositionList[m_startColumnIndex];
        m_pFloatingHeader->Arrange(Rect(Point(m_currentPos.get_X() - m_relativeStartPos.get_X(), 0), rect2.get_Size()));
        Point location = FindPositionByIndex(m_desColumnIndex);
        m_pIndicator->Arrange(Rect(location, Size(m_pIndicator->get_DesiredSize().get_Width(), arrangeSize.get_Height())));
    }
    return arrangeSize;
}

Point GridViewHeaderRowPresenter::FindPositionByIndex(int index)
{
    Rect rect = m_headersPositionList[index];
    return Point(rect.get_X(), 0);
}

void GridViewHeaderRowPresenter::AddIndicator()
{
    TRefCountedPtr<Separator> pElement = object_allocate<Separator>();
    pElement->set_Visibility(Visibility::Hidden);
    pElement->set_Margin(Thickness(0));
    pElement->set_Width(2.0f);

    // TODO -- Create a default indicator style, custom template here is not good.
//    FrameworkElementFactory factory = new FrameworkElementFactory(typeof(Border));
//    factory.SetValue(Border.BackgroundProperty, new SolidColorBrush(Color.FromUInt32(0xff000080)));
//    ControlTemplate template = new ControlTemplate(typeof(Separator));
//    template.VisualTree = factory;
//    template.Seal();
//    element.Template = template;

    get_InternalChildren()->Add(pElement);
    m_pIndicator = pElement;
}

void GridViewHeaderRowPresenter::AddFloatingHeader(GridViewColumnHeader* pSrcHeader)
{
    TRefCountedPtr<GridViewColumnHeader> pElement;
    const Type* pType = (pSrcHeader != NULL) ? &(pSrcHeader->get_Type()) : &GET_TYPE(GridViewColumnHeader);

    pElement = object_cast<GridViewColumnHeader>(pType->CreateInstance());

    pElement->set_IsInternalGenerated(true);
    pElement->set_Role(GridViewColumnHeaderRole::Floating);
    pElement->set_Visibility(Visibility::Hidden);

    ContentPresenter::BindToFontProperties(m_pItemsControl, pElement);

    get_InternalChildren()->Add(pElement);
    m_pFloatingHeader = pElement;
}

 
void GridViewHeaderRowPresenter::RemoveHeader(GridViewColumnHeader* pHeader, int index)
{
    TRefCountedPtr<GridViewColumnHeader> pHeaderRoot = pHeader;
    m_areHeadersValid = false;

    if (pHeader != NULL)
    {
        get_InternalChildren()->Remove(pHeader);
    }
    else
    {
        pHeader = (GridViewColumnHeader*) (*get_InternalChildren())[index];
        pHeaderRoot = pHeader;

        get_InternalChildren()->RemoveAt(index);
    }
    UnhookItemsControlKeyboardEvent(pHeader);
}


void GridViewHeaderRowPresenter::BuildHeaderLinks()
{
    GridViewColumnHeader* pHeader = NULL;

    GridViewColumnCollection* pColumns = get_Columns();
    UIElementCollection* pChildren = get_InternalChildren();

    if (pColumns != NULL)
    {
        for (int i = 0; i < pColumns->get_Count(); i++)
        {
            GridViewColumnHeader* pHeader2 = (GridViewColumnHeader*) (*pChildren)[GetVisualIndex(i)];
            pHeader2->set_PreviousVisualHeader(pHeader);
            pHeader = pHeader2;
        }
    }
    if (m_pPaddingHeader != NULL)
    {
        m_pPaddingHeader->set_PreviousVisualHeader(pHeader);
    }
}

int GridViewHeaderRowPresenter::GetVisualIndex(int columnIndex)
{
    return ((get_InternalChildren()->get_Count() - 3) - columnIndex);
}
 
 
GridViewColumnHeader* GridViewHeaderRowPresenter::CreateAndInsertHeader(GridViewColumn* pColumn, int index)
{
    Object* pChild = pColumn->get_Header();

    TRefCountedPtr<GridViewColumnHeader> pElement = object_cast<GridViewColumnHeader>(pChild);
    if (pChild != NULL)
    {
        DependencyObject* pDO = object_cast<DependencyObject>(pChild);
        if (pDO != NULL)
        {
            Visual* pReference = object_cast<Visual>(pDO);
            if (pReference != NULL)
            {
                Visual* pVisualParent = pReference->get_VisualParent();
                if (pVisualParent != NULL)
                {
                    if (pElement != NULL)
                    {
                        GridViewHeaderRowPresenter* pPresenter = object_cast<GridViewHeaderRowPresenter>(pVisualParent);
                        if (pPresenter != NULL)
                        {
                            pPresenter->get_InternalChildren()->Remove(pElement);
                        }
                    }
                    else
                    {
                        GridViewColumnHeader* pHeader = object_cast<GridViewColumnHeader>(pVisualParent);
                        if (pHeader != NULL)
                        {
                            pHeader->ClearValue(ContentControl::get_ContentProperty());
                        }
                    }
                }
            }

            // TODO -- Parity with logical tree?
            /*
            DependencyObject* pLogicalParent = LogicalTreeHelper.GetParent(current);
            if (parent != null)
            {
                LogicalTreeHelper.RemoveLogicalChild(parent, child);
            }
            */
        }
    }
    if (pElement == NULL)
    {
        pElement = object_allocate<GridViewColumnHeader>();
        pElement->set_IsInternalGenerated(true);

        ContentPresenter::BindToFontProperties(m_pItemsControl, pElement);
    }
    pElement->SetValue(GridViewColumnHeader::get_ColumnProperty(), pColumn);

    HookupItemsControlKeyboardEvent(pElement);
    get_InternalChildren()->Insert(index, pElement);
    UpdateHeader(pElement);
    m_areHeadersValid = false;
    return pElement;
}



void GridViewHeaderRowPresenter::PrepareHeaderDrag(GridViewColumnHeader* pHeader, const Point& pos, const Point& relativePos, bool cancelInvoke)
{
    if (pHeader->get_Role() == GridViewColumnHeaderRole::Normal)
    {
        m_prepareDragging = true;
        m_isHeaderDragging = false;
        m_pDraggingSrcHeader = pHeader;
        m_startPos = pos;
        m_relativeStartPos = relativePos;
        if (!cancelInvoke)
        {
            m_startColumnIndex = FindIndexByPosition(m_startPos, false);
        }
    }
}


int GridViewHeaderRowPresenter::FindIndexByPosition(const Point& startPos, bool findNearestColumn)
{
    int num = -1;
    if (startPos.get_X() < 0)
    {
        return 0;
    }
    for (int i = 0; i < m_headersPositionList.get_Count(); i++)
    {
        num++;
        Rect rect = m_headersPositionList[i];
        float x = rect.get_X();
        float num4 = x + rect.get_Width();
        if (Float::GreaterThanOrClose(startPos.get_X(), x) && Float::LessThanOrClose(startPos.get_X(), num4))
        {
            if (findNearestColumn)
            {
                float num5 = (x + num4) * 0.5f;
                if (Float::GreaterThanOrClose(startPos.get_X(), num5) && (i != (m_headersPositionList.get_Count() - 1)))
                {
                    num++;
                }
            }
            return num;
        }
    }
    return num;
}
 
ItemsControl* GridViewHeaderRowPresenter::FindItemsControlThroughTemplatedParent(GridViewHeaderRowPresenter* pPresenter)
{
    UIElement *pElement = object_cast<UIElement>(pPresenter->get_TemplatedParent());
    ItemsControl* pControl = NULL;
    while (pElement != NULL)
    {
        pControl = object_cast<ItemsControl>(pElement);
        if (pControl != NULL)
        {
            return pControl;
        }
        pElement = object_cast<UIElement>(pElement->get_TemplatedParent());
    }
    return pControl;
}

void GridViewHeaderRowPresenter::FinishHeaderDrag(bool isCancel)
{
    m_prepareDragging = false;
    m_isHeaderDragging = false;
    m_pDraggingSrcHeader->set_SuppressClickEvent(false);
    m_pFloatingHeader->set_Visibility(Visibility::Hidden);
    m_pIndicator->set_Visibility(Visibility::Hidden);

    GridViewColumnCollection* pColumns = get_Columns();

    if (pColumns != NULL)
    {
        pColumns->UnblockWrite();
    }
    if (!isCancel)
    {
        bool flag = IsMousePositionValid(m_pFloatingHeader, m_currentPos, 2);
        int newIndex = (m_startColumnIndex >= m_desColumnIndex) ? m_desColumnIndex : (m_desColumnIndex - 1);
        if (flag)
        {
            pColumns->Move(m_startColumnIndex, newIndex);
        }
    }
}

void GridViewHeaderRowPresenter::UpdateHeaderContent(GridViewColumnHeader* pHeader)
{
    if (pHeader != NULL && pHeader->get_IsInternalGenerated())
    {
        GridViewColumn* pColumn = pHeader->get_Column();
        if (pColumn != NULL)
        {
            if (pColumn->get_Header() == NULL)
            {
                pHeader->ClearValue(ContentControl::get_ContentProperty());
            }
            else
            {
                pHeader->set_Content(pColumn->get_Header());
            }
        }
    }
}

 


bool GridViewHeaderRowPresenter::IsMousePositionValid(UIElement* pFloatingHeader, const Point& currentPos, float arrange)
{
    if (Float::LessThanOrClose(-pFloatingHeader->get_Height() * arrange, currentPos.get_Y()))
    {
        return Float::LessThanOrClose(currentPos.get_Y(), pFloatingHeader->get_Height() * (arrange + 1));
    }
    return false;
}

void GridViewHeaderRowPresenter::OnHeaderScrollChanged(Object* pSender, ScrollChangedEventArgs* pArgs)
{
    if (m_pMainSV != NULL)
    {
        m_pMainSV->get_ScrollInfo()->set_HorizontalOffset(pArgs->get_HorizontalOffset());
        // TODO -- this is correct code, above is an (ahem) workaround
        //m_pMainSV->ScrollToHorizontalOffset(pArgs->get_HorizontalOffset());
    }
}

void GridViewHeaderRowPresenter::OnMasterScrollChanged(Object* pSender, ScrollChangedEventArgs* pArgs)
{
    if (m_pHeaderSV != NULL)
    {
//        m_pHeaderSV->ScrollToHorizontalOffset(pArgs->get_HorizontalOffset());
        // TODO -- this is correct code, above is an (ahem) workaround
        m_pHeaderSV->get_ScrollInfo()->set_HorizontalOffset(pArgs->get_HorizontalOffset());

    }
}

void GridViewHeaderRowPresenter::OnColumnHeadersPresenterKeyDown(Object* pSender, KeyEventArgs* pArgs)
{
    if (pArgs->get_Key() == Key::Escape && m_isHeaderDragging)
    {
        TRefCountedPtr<GridViewColumnHeader> pHeader = m_pDraggingSrcHeader;

        FinishHeaderDrag(true);
        PrepareHeaderDrag(pHeader, m_currentPos, m_relativeStartPos, true);
        InvalidateArrange();
    }
}

void GridViewHeaderRowPresenter::UpdateHeader(GridViewColumnHeader* pHeader)
{
    UpdateHeaderContent(pHeader);
    for (int i = 0; i < 5; i++)
    {
        UpdateHeaderProperty(pHeader, GetPresenterDP(2, i), GetPresenterDP(1, i), GetPresenterDP(0, i));
    }
}

void GridViewHeaderRowPresenter::UpdatePaddingHeader(GridViewColumnHeader* pHeader)
{
    UpdateHeaderProperty(pHeader, &get_ColumnHeaderContainerStyleProperty());
    UpdateHeaderProperty(pHeader, &get_ColumnHeaderContextMenuProperty());
    UpdateHeaderProperty(pHeader, &get_ColumnHeaderToolTipProperty());
}

 
void GridViewHeaderRowPresenter::UpdateHeaderProperty(GridViewColumnHeader* pHeader, const DependencyProperty* pProperty)
{
    const DependencyProperty* pGVDP;
    const DependencyProperty* pColumnDP;
    const DependencyProperty* pHeaderDP;

    GetMatchingDPs(pProperty, &pGVDP, &pColumnDP, &pHeaderDP);
    UpdateHeaderProperty(pHeader, pHeaderDP, pColumnDP, pGVDP);
}

 
void GridViewHeaderRowPresenter::UpdateHeaderProperty(GridViewColumnHeader* pHeader, const DependencyProperty* pTargetDP, 
                                                      const DependencyProperty* pColumnDP, 
                                                      const DependencyProperty* pGVDP)
{
    if (pGVDP == &get_ColumnHeaderContainerStyleProperty() && pHeader->get_Role() == GridViewColumnHeaderRole::Padding)
    {
        Style* pColumnHeaderContainerStyle = get_ColumnHeaderContainerStyle();
        if (pColumnHeaderContainerStyle != NULL && !pColumnHeaderContainerStyle->get_TargetType()->IsTypeOrBase(GET_TYPE(GridViewColumnHeader)))
        {
            pHeader->set_Style(NULL);
            return;
        }
    }
    GridViewColumn* pColumn = pHeader->get_Column();
    Object* pObj2 = NULL;
    if (pColumn != NULL && pColumnDP != NULL)
    {
        pObj2 = pColumn->GetValue(*pColumnDP);
    }
    if (pObj2 == NULL)
    {
        pObj2 = GetValue(*pGVDP);
    }
    pHeader->UpdateProperty(pTargetDP, pObj2);
}

 
void GridViewHeaderRowPresenter::GetMatchingDPs(const DependencyProperty* indexDP, const DependencyProperty** ppGVDP, const DependencyProperty** ppColumnDP, const DependencyProperty** ppHeaderDP)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (indexDP == GetPresenterDP(i, j))
            {
                *ppGVDP = GetPresenterDP(0, j);
                *ppColumnDP = GetPresenterDP(1, j);
                *ppHeaderDP = GetPresenterDP(2, j);
                return;
            }
        }
    }
    *ppGVDP = *ppColumnDP = *ppHeaderDP = NULL;
}

void GridViewHeaderRowPresenter::HookupItemsControlKeyboardEvent(GridViewColumnHeader* pHeader)
{
    if (m_pItemsControl != NULL)
    {
        m_pItemsControl->add_KeyDown(KeyEventHandler::Create(pHeader, &GridViewColumnHeader::OnColumnHeaderKeyDown));
    }
}

void GridViewHeaderRowPresenter::UnhookItemsControlKeyboardEvent(GridViewColumnHeader* pHeader)
{
    if (m_pItemsControl != NULL)
    {
        m_pItemsControl->remove_KeyDown(KeyEventHandler::Create(pHeader, &GridViewColumnHeader::OnColumnHeaderKeyDown));
    }
}


void GridViewHeaderRowPresenter::OnLayoutUpdated(Object* pSender, EventArgs* e)
{
    bool flag = false;
    GridViewColumnCollection* pColumns = get_Columns();

    for(int idx = 0; idx < pColumns->get_Count(); idx++)
    {
        GridViewColumn* pColumn = (*pColumns)[idx];

        if (pColumn->get_State() == ColumnMeasureState::SpecificWidth)
        {
            continue;
        }
        if (pColumn->get_State() == ColumnMeasureState::Init)
        {
            pColumn->set_State(ColumnMeasureState::Headered);
        }
        if (pColumn->get_ActualIndex() >= m_desiredWidthList.get_Count())
        {
            flag = true;
            break;
        }
        if (!Float::AreClose(pColumn->get_DesiredWidth(), m_desiredWidthList[pColumn->get_ActualIndex()]))
        {
            m_desiredWidthList[pColumn->get_ActualIndex()] = pColumn->get_DesiredWidth();
            flag = true;
        }
    }
    if (flag)
    {
        InvalidateMeasure();
    }

    get_LayoutManager()->remove_LayoutUpdated(EventHandler::Create(this, &GridViewHeaderRowPresenter::OnLayoutUpdated));
}
 

GridViewColumnHeader* GridViewHeaderRowPresenter::FindHeaderByColumn(GridViewColumn* pColumn)
{
    GridViewColumnCollection* pColumns = get_Columns();
    UIElementCollection* pInternalChildren = get_InternalChildren();
    if (pColumns != NULL && pInternalChildren->get_Count() > pColumns->get_Count())
    {
        int index = pColumns->IndexOf(pColumn);
        if (index != -1)
        {
            int visualIndex = GetVisualIndex(index);
            GridViewColumnHeader* pHeader = object_cast<GridViewColumnHeader>(pInternalChildren->GetItemAt(visualIndex));
            if (pHeader->get_Column() == pColumn)
            {
                return pHeader;
            }
            for (int i = 1; i < pInternalChildren->get_Count(); i++)
            {
                pHeader = object_cast<GridViewColumnHeader>(pInternalChildren->GetItemAt(i));
                if (pHeader != NULL && pHeader->get_Column() == pColumn)
                {
                    return pHeader;
                }
            }
        }
    }
    return NULL;
}


void GridViewHeaderRowPresenter::OnColumnPropertyChanged(GridViewColumn* pColumn, const Property* pProperty)
{
    if (pColumn->get_ActualIndex() >= 0)
    {
        GridViewColumnHeader* pElement = FindHeaderByColumn(pColumn);
        if (pElement != NULL)
        {
            if (pProperty == &GridViewColumn::get_WidthProperty() || pProperty == &GridViewColumn::get_ActualWidthProperty())
            {
                InvalidateMeasure();
            }
            else if (&GridViewColumn::get_HeaderProperty() == pProperty)
            {
                if (!pElement->get_IsInternalGenerated() || pColumn->get_Header()->IsType(GET_TYPE(GridViewColumnHeader)))
                {
                    int index = get_InternalChildren()->IndexOf(pElement);
                    RemoveHeader(pElement, -1);
                    CreateAndInsertHeader(pColumn, index);
                    BuildHeaderLinks();
                }
                else
                {
                    UpdateHeaderContent(pElement);
                }
            }
            else
            {
                if(pProperty->IsType(GET_TYPE(DependencyProperty)))
                {
                    UpdateHeaderProperty(pElement, (DependencyProperty*) pProperty);
                }
            }
        }
    }
}
 
 
void GridViewHeaderRowPresenter::MakeParentItemsControlGotFocus()
{
    if (m_pItemsControl != NULL && !m_pItemsControl->get_IsKeyboardFocusWithin())
    {
        ListBox* pListBox = object_cast<ListBox>(m_pItemsControl);
        if (pListBox != NULL && pListBox->get_LastActionItem() != NULL){
            pListBox->get_LastActionItem()->Focus();
        }
        else
        {
            m_pItemsControl->Focus();
        }
    }
}

};


