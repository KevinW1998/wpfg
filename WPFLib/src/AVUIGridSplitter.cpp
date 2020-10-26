#include <AVUICommon.h>
#include <AVUIGridSplitter.h>
#include <AVUIGrid.h>
#include <AVUICursors.h>
#include <AVUICursor.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridSplitter)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)

    REGISTER_PROPERTY(ResizeDirection)
    REGISTER_PROPERTY(ResizeBehavior)
    REGISTER_PROPERTY(KeyboardIncrement)
    REGISTER_PROPERTY(DragIncrement)
REGISTER_ELEMENT_END()

namespace AVUI {


void GridSplitter::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(GridSplitter), get_DragStartedEvent(), RoutedEventHandler::Create(&GridSplitter::OnDragStarted), false);
    EventManager::RegisterClassHandler(GET_TYPE(GridSplitter), get_DragDeltaEvent(), DragDeltaEventHandler::Create(&GridSplitter::OnDragDelta), false);
    EventManager::RegisterClassHandler(GET_TYPE(GridSplitter), get_DragCompletedEvent(), RoutedEventHandler::Create(&GridSplitter::OnDragCompleted), false);

    static DependencyPropertyMetadata cursorMetadata(PropertyFlag::None, NULL, &GridSplitter::CoerceCursor);
    UIElement::get_CursorProperty().OverrideMetadata(GET_TYPE(GridSplitter), NULL, &cursorMetadata);
}

void GridSplitter::OnDragStarted(RoutedEventArgs* pEventArgs)
{
    InitializeData();
}

void GridSplitter::OnDragDelta(DragDeltaEventArgs* pEventArgs)
{
    if(m_pResizeData == NULL)
    {
        return;
    }

    float horizontalChange = pEventArgs->get_HorizontalChange();
    float verticalChange = pEventArgs->get_VerticalChange();
    float dragIncrement = get_DragIncrement();
    horizontalChange = Float::Round(horizontalChange / dragIncrement) * dragIncrement;
    verticalChange = Float::Round(verticalChange / dragIncrement) * dragIncrement;

    MoveSplitter(horizontalChange, verticalChange);
}

void GridSplitter::OnDragCompleted(RoutedEventArgs* pEventArgs)
{
    m_pResizeData = NULL;
}

void GridSplitter::InitializeData()
{
    Grid* pGrid = object_cast<Grid>(get_VisualParent());
    if (pGrid != NULL)
    {
        m_pResizeData = object_allocate<ResizeData>();
        m_pResizeData->pGrid = pGrid;
        m_pResizeData->ResizeDirection = GetEffectiveResizeDirection();
        m_pResizeData->ResizeBehavior = GetEffectiveResizeBehavior(m_pResizeData->ResizeDirection);
        m_pResizeData->SplitterLength = Float::Min(get_RenderSize().get_Width(), get_RenderSize().get_Height());

        if (!SetupDefinitionsToResize())
        {
            m_pResizeData = NULL;
        }
    }
}

GridResizeBehavior::Enum GridSplitter::GetEffectiveResizeBehavior(GridResizeDirection::Enum direction)
{
    GridResizeBehavior::Enum resizeBehavior = get_ResizeBehavior();
    if (resizeBehavior != GridResizeBehavior::BasedOnAlignment)
    {
        return resizeBehavior;
    }
    if (direction != GridResizeDirection::Columns)
    {
        switch (get_VerticalAlignment())
        {
        case VerticalAlignment::Top:
                return GridResizeBehavior::PreviousAndCurrent;

        case VerticalAlignment::Center:
                return GridResizeBehavior::PreviousAndNext;

        case VerticalAlignment::Bottom:
                return GridResizeBehavior::CurrentAndNext;
        }
    }
    else
    {
        switch (get_HorizontalAlignment())
        {
        case HorizontalAlignment::Left:
                return GridResizeBehavior::PreviousAndCurrent;

        case HorizontalAlignment::Right:
                return GridResizeBehavior::CurrentAndNext;
        }
        return GridResizeBehavior::PreviousAndNext;
    }

    return GridResizeBehavior::PreviousAndNext;
}

GridResizeDirection::Enum GridSplitter::GetEffectiveResizeDirection()
{
    GridResizeDirection::Enum resizeDirection = get_ResizeDirection();
    if (resizeDirection != GridResizeDirection::Auto)
    {
        return resizeDirection;
    }
    if (get_HorizontalAlignment() != HorizontalAlignment::Stretch)
    {
        return GridResizeDirection::Columns;
    }
    if ((get_VerticalAlignment() == VerticalAlignment::Stretch) && (get_RenderSize().get_Width() <= get_RenderSize().get_Height()))
    {
        return GridResizeDirection::Columns;
    }
    return GridResizeDirection::Rows;
}

bool GridSplitter::SetupDefinitionsToResize()
{
    int span = UnboxValue<int>(GetValue((m_pResizeData->ResizeDirection == GridResizeDirection::Columns) ? Grid::get_ColumnSpanProperty() : Grid::get_RowSpanProperty()));
    if (span != 1)
    {
        return false;
    }

    int index;
    int nextIndex;
    int rowcol = UnboxValue<int>(GetValue((m_pResizeData->ResizeDirection == GridResizeDirection::Columns) ? Grid::get_ColumnProperty() : Grid::get_RowProperty()));
    switch (m_pResizeData->ResizeBehavior)
    {
    case GridResizeBehavior::CurrentAndNext:
            index = rowcol;
            nextIndex = rowcol + 1;
            break;

    case GridResizeBehavior::PreviousAndCurrent:
            index = rowcol - 1;
            nextIndex = rowcol;
            break;

        default:
            index = rowcol - 1;
            nextIndex = rowcol + 1;
            break;
    }
    int cDefs = (m_pResizeData->ResizeDirection == GridResizeDirection::Columns) ? m_pResizeData->pGrid->get_ColumnDefinitions()->get_Count() : m_pResizeData->pGrid->get_RowDefinitions()->get_Count();
    if ((index >= 0) && (nextIndex < cDefs))
    {
        m_pResizeData->SplitterIndex = rowcol;
        m_pResizeData->Definition1Index = index;
        m_pResizeData->Definition1 = GetGridDefinition(m_pResizeData->pGrid, index, m_pResizeData->ResizeDirection);
        m_pResizeData->OriginalDefinition1Length = m_pResizeData->Definition1->get_UserSize();
        m_pResizeData->OriginalDefinition1ActualLength = GetActualLength(m_pResizeData->Definition1);
        m_pResizeData->Definition2Index = nextIndex;
        m_pResizeData->Definition2 = GetGridDefinition(m_pResizeData->pGrid, nextIndex, m_pResizeData->ResizeDirection);
        m_pResizeData->OriginalDefinition2Length = m_pResizeData->Definition2->get_UserSize();
        m_pResizeData->OriginalDefinition2ActualLength = GetActualLength(m_pResizeData->Definition2);
        bool isDef1Star = IsStar(m_pResizeData->Definition1);
        bool isDef2Star = IsStar(m_pResizeData->Definition2);
        if (isDef1Star && isDef2Star)
        {
            m_pResizeData->SplitBehavior = SplitBehavior::Split;
        }
        else
        {
            m_pResizeData->SplitBehavior = !isDef1Star ? SplitBehavior::Resize1 : SplitBehavior::Resize2;
        }
        return true;
    }
    return false;
}

bool GridSplitter::IsStar(DefinitionBase* pDefinition)
{
    return pDefinition->get_UserSize().get_IsStar();
}

float GridSplitter::GetActualLength(DefinitionBase* pDefinition)
{
    ColumnDefinition* pColDef = object_cast<ColumnDefinition>(pDefinition);
    if (pColDef != NULL)
    {
        return pColDef->get_ActualWidth();
    }
    return ((RowDefinition*) pDefinition)->get_ActualHeight();
}

DefinitionBase* GridSplitter::GetGridDefinition(Grid* pGrid, int index, GridResizeDirection::Enum direction)
{
    if (direction != GridResizeDirection::Columns)
    {
        RowDefinitionCollection* pRowDefs = pGrid->get_RowDefinitions();

        return (*pRowDefs)[index];
    }

    ColumnDefinitionCollection* pColDefs = pGrid->get_ColumnDefinitions();

    return (*pColDefs)[index];
}

void GridSplitter::MoveSplitter(float horizontalChange, float verticalChange)
{
    float delta = (m_pResizeData->ResizeDirection == GridResizeDirection::Columns) ? horizontalChange : verticalChange;
    DefinitionBase* pDef1 = m_pResizeData->Definition1;
    DefinitionBase* pDef2 = m_pResizeData->Definition2;
    if (pDef1 != NULL && pDef2 != NULL)
    {
        float def1Length = GetActualLength(pDef1);
        float def2Length = GetActualLength(pDef2);
        if ((m_pResizeData->SplitBehavior == SplitBehavior::Split) && !Float::AreClose(def1Length + def2Length, m_pResizeData->OriginalDefinition1ActualLength + m_pResizeData->OriginalDefinition2ActualLength))
        {
            CancelResize();
        }
        else
        {
            float minDelta;
            float maxDelta;

            GetDeltaConstraints(&minDelta, &maxDelta);

            delta = Float::Min(Float::Max(delta, minDelta), maxDelta);
            SetLengths(def1Length + delta, def2Length - delta);
        }
    }
}

void GridSplitter::SetLengths(float definition1Pixels, float definition2Pixels)
{
    if (m_pResizeData->SplitBehavior == SplitBehavior::Split)
    {
        ICollection* pCollection = m_pResizeData->pGrid->get_ColumnDefinitions();

        if(m_pResizeData->ResizeDirection == GridResizeDirection::Rows)
        {
            pCollection = m_pResizeData->pGrid->get_RowDefinitions();
        }

        for(int idx = 0; idx < pCollection->get_Count(); idx++)
        {
            DefinitionBase* pBase = (DefinitionBase*) (Object*) (pCollection->GetItemAt(idx));

            if (idx == m_pResizeData->Definition1Index)
            {
                SetDefinitionLength(pBase, GridLength(definition1Pixels, GridUnitType::Star));
            }
            else if (idx == m_pResizeData->Definition2Index)
            {
                SetDefinitionLength(pBase, GridLength(definition2Pixels, GridUnitType::Star));
            }
            else if (IsStar(pBase))
            {
                SetDefinitionLength(pBase, GridLength(GetActualLength(pBase), GridUnitType::Star));
            }
        }
    }
    else if (m_pResizeData->SplitBehavior == SplitBehavior::Resize1)
    {
        SetDefinitionLength(m_pResizeData->Definition1, GridLength(definition1Pixels));
    }
    else
    {
        SetDefinitionLength(m_pResizeData->Definition2, GridLength(definition2Pixels));
    }
}

void GridSplitter::SetDefinitionLength(DefinitionBase* pDefinition, const GridLength& length)
{
    if(pDefinition->IsType(GET_TYPE(ColumnDefinition)))
    {
        ((ColumnDefinition*)pDefinition)->set_Width(length);
    }
    else
    {
        ((RowDefinition*)pDefinition)->set_Height(length);
    }
}

void GridSplitter::CancelResize()
{
    SetDefinitionLength(m_pResizeData->Definition1, m_pResizeData->OriginalDefinition1Length);
    SetDefinitionLength(m_pResizeData->Definition2, m_pResizeData->OriginalDefinition2Length);

    m_pResizeData = NULL;
}


void GridSplitter::GetDeltaConstraints(float* pMinDelta, float* pMaxDelta)
{
    float actualLength = GetActualLength(m_pResizeData->Definition1);
    float userMinSizeValueCache = m_pResizeData->Definition1->get_UserMinSize();
    float userMaxSizeValueCache = m_pResizeData->Definition1->get_UserMaxSize();
    float num4 = GetActualLength(m_pResizeData->Definition2);
    float num5 = m_pResizeData->Definition2->get_UserMinSize();
    float num6 = m_pResizeData->Definition2->get_UserMaxSize();
    if (m_pResizeData->SplitterIndex == m_pResizeData->Definition1Index)
    {
        userMinSizeValueCache = Float::Max(userMinSizeValueCache, m_pResizeData->SplitterLength);
    }
    else if (m_pResizeData->SplitterIndex == m_pResizeData->Definition2Index)
    {
        num5 = Float::Max(num5, m_pResizeData->SplitterLength);
    }
    if (m_pResizeData->SplitBehavior == SplitBehavior::Split)
    {
        *pMinDelta = -Float::Min((float) (actualLength - userMinSizeValueCache), (float) (num6 - num4));
        *pMaxDelta = Float::Min((float) (userMaxSizeValueCache - actualLength), (float) (num4 - num5));
    }
    else if (m_pResizeData->SplitBehavior == SplitBehavior::Resize1)
    {
        *pMinDelta = userMinSizeValueCache - actualLength;
        *pMaxDelta = userMaxSizeValueCache - actualLength;
    }
    else
    {
        *pMinDelta = num4 - num6;
        *pMaxDelta = num4 - num5;
    }
}

TRefCountedPtr<Object> GridSplitter::CoerceCursor(DependencyObject* pDO, Object* pValue)
{
    bool flag;
    GridSplitter* pSplitter = (GridSplitter*) pDO;
    BaseValueSource::Enum valueSource = pSplitter->GetValueSource(UIElement::get_CursorProperty(), NULL, &flag);
    if (pValue == NULL && valueSource == BaseValueSource::Default)
    {
        switch (pSplitter->GetEffectiveResizeDirection())
        {
        case GridResizeDirection::Columns:
                return Cursors::get_SizeWE();

        case GridResizeDirection::Rows:
                return Cursors::get_SizeNS();
        }
    }
    return pValue;
}

void GridSplitter::UpdateCursor(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((DependencyObject*)pDO)->CoerceValue(UIElement::get_CursorProperty());
}

void GridSplitter::OnRenderSizeChanged(SizeChangedInfo* pSizeInfo)
{
    UIElement::OnRenderSizeChanged(pSizeInfo);
    CoerceValue(UIElement::get_CursorProperty());
}

}; // namespace AVUI
