#include <AVUICommon.h>
#include <AVUIGrid.h>

REGISTER_ELEMENT_BEGIN(AVUI::ColumnDefinition)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Width)
    REGISTER_PROPERTY(MinWidth)
    REGISTER_PROPERTY(MaxWidth)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::RowDefinition)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Height)
    REGISTER_PROPERTY(MinHeight)
    REGISTER_PROPERTY(MaxHeight)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::ColumnDefinitionCollection)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Grid)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(ShowGridLines);
    REGISTER_PROPERTY(Column);
    REGISTER_PROPERTY(Row);
    REGISTER_PROPERTY(ColumnSpan);
    REGISTER_PROPERTY(RowSpan);
    REGISTER_PROPERTY(ColumnDefinitions);
    REGISTER_PROPERTY(RowDefinitions);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::RowDefinitionCollection)

REGISTER_ELEMENT_END()

namespace AVUI {

void ColumnDefinition::OnPropertyChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e) { if(((ColumnDefinition*)pDO)->m_pOwner != NULL) { ((ColumnDefinition*)pDO)->m_pOwner->PrivateColumnDefinitionsChanged(); } }
void RowDefinition::OnPropertyChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e) { if(((RowDefinition*)pDO)->m_pOwner != NULL) { ((RowDefinition*)pDO)->m_pOwner->PrivateRowDefinitionsChanged(); } }
float RowDefinition::get_ActualHeight()
{
    float finalRowDefinitionHeight = 0.0f;
    if (m_pGrid != NULL)
    {
        RowDefinitionCollection* pDefs = m_pGrid->get_RowDefinitions();
        int index = -1;
        for(int idx = 0; idx < pDefs->get_Count(); idx++)
        {
            if((*pDefs)[idx] == this)
            {
                index = (int) idx;
            }
        }

        if(index != -1)
        {
            finalRowDefinitionHeight = m_pGrid->GetFinalRowDefinitionHeight(index);
        }
    }
    return finalRowDefinitionHeight;
}


void RowDefinitionCollection::OnCollectionChanged() { if(m_pOwner != NULL) m_pOwner->PrivateRowDefinitionsChanged(); }
void ColumnDefinitionCollection::OnCollectionChanged() { if(m_pOwner != NULL) m_pOwner->PrivateColumnDefinitionsChanged(); }

float ColumnDefinition::get_ActualWidth()
{
    float finalColDefinitionWidth = 0.0f;
    if (m_pGrid != NULL)
    {
        ColumnDefinitionCollection* pDefs = m_pGrid->get_ColumnDefinitions();
        int index = -1;
        for(int idx = 0; idx < pDefs->get_Count(); idx++)
        {
            if((*pDefs)[idx] == this)
            {
                index = (int) idx;
            }
        }

        if(index != -1)
        {
            finalColDefinitionWidth = m_pGrid->GetFinalColumnDefinitionWidth(index);
        }
    }
    return finalColDefinitionWidth;
}



Grid::Grid() : m_rowDefinitions(this), m_columnDefinitions(this), m_sizeToContentU(false), m_sizeToContentV(false), m_listenToNotifications(false), 
               m_measureOverrideInProgress(false), m_cellsStructureDirty(true), m_cellGroup1(0), m_cellGroup2(0), m_cellGroup3(0), m_cellGroup4(0),
               m_hasStarCellsU(false), m_hasStarCellsV(false), m_hasGroup3CellsInAutoRows(false), m_arrangeOverrideInProgress(false)
{
}

Grid::~Grid()
{
    m_rowDefinitions.InvalidateWeakReferences();
    m_columnDefinitions.InvalidateWeakReferences();
}


Size Grid::MeasureOverride(const Size& constraintSize)
{
    Size measureSize;

    m_listenToNotifications = true;
    m_measureOverrideInProgress = true;

    if(m_rowDefinitions.get_Count() == 0 && m_columnDefinitions.get_Count() == 0)
    {
        UIElementCollection& children = *(get_Children());
        int cElts = children.get_Count();
        float desiredWidth = 0.0f;
        float desiredHeight = 0.0f;

        for(int i = 0; i < cElts; i++)
        {
            UIElement* pUIElement = children[i];
            if (pUIElement != NULL)
            {
                pUIElement->Measure(constraintSize);

                desiredWidth = Float::Max(desiredWidth, pUIElement->get_DesiredSize().get_Width());
                desiredHeight = Float::Max(desiredHeight, pUIElement->get_DesiredSize().get_Height());
            }
        }

        return Size(desiredWidth, desiredHeight);
    }


    m_tempDefinitions.EnsureIndex(Int::Max(m_rowDefinitions.get_Count(), m_columnDefinitions.get_Count()));

    bool isWidthInfinite = Float::IsPositiveInfinity(constraintSize.get_Width());
    bool isHeightInfinite = Float::IsPositiveInfinity(constraintSize.get_Height());
  
    ValidateDefinitionsLayout(&m_columnDefinitions, isWidthInfinite);
    ValidateDefinitionsLayout(&m_rowDefinitions, isHeightInfinite);

    if(!m_cellsStructureDirty)
    {
        if(m_sizeToContentU != isWidthInfinite || m_sizeToContentV != isHeightInfinite)
        {
            m_cellsStructureDirty;
        }
    }

    ValidateCells();
    MeasureCellsGroup(m_cellGroup1, constraintSize, false, false);

    if (!m_hasGroup3CellsInAutoRows)
    {
        if (m_hasStarCellsV)
        {
            ResolveStar(&m_rowDefinitions, constraintSize.get_Height());
        }

        MeasureCellsGroup(m_cellGroup2, constraintSize, false, false);
        if (m_hasStarCellsU)
        {
            ResolveStar(&m_columnDefinitions, constraintSize.get_Width());
        }
        MeasureCellsGroup(m_cellGroup3, constraintSize, false, false);
    }
    else if (m_cellGroup2 > (int)m_cellCache.get_Count())
    {
        if (m_hasStarCellsU)
        {
            ResolveStar(&m_columnDefinitions, constraintSize.get_Width());
        }
        MeasureCellsGroup(m_cellGroup3, constraintSize, false, false);
        if (m_hasStarCellsV)
        {
            ResolveStar(&m_rowDefinitions, constraintSize.get_Height());
        }
    }
    else
    {
        MeasureCellsGroup(m_cellGroup2, constraintSize, false, true);
        if (m_hasStarCellsU)
        {
            ResolveStar(&m_columnDefinitions, constraintSize.get_Width());
        }
        MeasureCellsGroup(m_cellGroup3, constraintSize, false, false);
        if (m_hasStarCellsV)
        {
            ResolveStar(&m_rowDefinitions, constraintSize.get_Height());
        }

        MeasureCellsGroup(m_cellGroup2, constraintSize, true, false);
    }

    MeasureCellsGroup(m_cellGroup4, constraintSize, false, false);

    measureSize = Size(CalculateDesiredSize(&m_columnDefinitions), CalculateDesiredSize(&m_rowDefinitions));

    m_measureOverrideInProgress = false;

    return measureSize;
}

Size Grid::ArrangeOverride(const Size& arrangeSize)
{
    if(m_rowDefinitions.get_Count() == 0 && m_columnDefinitions.get_Count() == 0)
    {
        Rect arrangeRect(arrangeSize);
        UIElementCollection& children = *(get_Children());
        int cElts = children.get_Count();

        for(int i = 0; i < cElts; i++)
        {
            UIElement* pElement = children[i];
            if (pElement != NULL)
            {
                pElement->Arrange(arrangeRect);
            }
        }
        return arrangeSize;
    }

    m_tempDefinitions.EnsureIndex(Int::Max(m_rowDefinitions.get_Count(), m_columnDefinitions.get_Count()));

    m_arrangeOverrideInProgress = true;

    set_FinalSize(&m_columnDefinitions, arrangeSize.get_Width());
    set_FinalSize(&m_rowDefinitions, arrangeSize.get_Height());

    UIElementCollection& children = *(get_Children());
    for (int index = 0; index < m_cellCache.get_Count(); index++)
    {
        UIElement* pChild = children[index];
        if (pChild != NULL)
        {
            CellCache& cellCache = m_cellCache[index];
            int columnIndex = cellCache.ColumnIndex;
            int rowIndex = cellCache.RowIndex;
            int columnSpan = cellCache.ColumnSpan;
            int rowSpan = cellCache.RowSpan;
            Rect arrangeRect((columnIndex == 0) ? 0 : m_columnDefinitions[columnIndex]->get_FinalOffset(), (rowIndex == 0) ? 0 : m_rowDefinitions[rowIndex]->get_FinalOffset(), get_FinalSizeForRange(&m_columnDefinitions, columnIndex, columnSpan), get_FinalSizeForRange(&m_rowDefinitions, rowIndex, rowSpan));
            pChild->Arrange(arrangeRect);
        }
    }

    set_Valid();
    m_arrangeOverrideInProgress = false;

    return arrangeSize;
}

void Grid::set_Valid()
{
    m_tempDefinitions.Clear();
}


void Grid::ValidateDefinitionsLayout(ICollection* pCollection, bool treatStarAsAuto)
{
    for (int index = 0; index < pCollection->get_Count(); index++)
    {
        DefinitionBase* pDefinitionBase = object_cast<DefinitionBase>(pCollection->GetItemAt(index));

          pDefinitionBase->OnBeforeLayout(this);

          float userMinSize = pDefinitionBase->get_UserMinSize();
          float userMaxSize = pDefinitionBase->get_UserMaxSize();
          float calculatedMaxSize = 0;

          switch (pDefinitionBase->get_UserSize().get_GridUnitType())
          {
          case GridUnitType::Auto:
              pDefinitionBase->set_SizeType(LayoutTimeSizeType::Auto);
              calculatedMaxSize = Float::get_PositiveInfinity();
              break;

          case GridUnitType::Pixel:
              pDefinitionBase->set_SizeType(LayoutTimeSizeType::Pixel);
              calculatedMaxSize = pDefinitionBase->get_UserSize().get_Value();
              userMinSize = Float::Max(userMinSize, Float::Min(calculatedMaxSize, userMaxSize));
              break;

          case GridUnitType::Star:
              if (!treatStarAsAuto)
              {
                  pDefinitionBase->set_SizeType(LayoutTimeSizeType::Star);
                  calculatedMaxSize = Float::get_PositiveInfinity();
                  break;
              }
              pDefinitionBase->set_SizeType(LayoutTimeSizeType::Auto);
              calculatedMaxSize = Float::get_PositiveInfinity();
              break;

          default:
              break;
          }

          pDefinitionBase->UpdateMinSize(userMinSize);
          pDefinitionBase->set_MeasureSize(Float::Max(userMinSize, Float::Min(calculatedMaxSize, userMaxSize)));
    }
}

void Grid::ResolveStar(ICollection* pCollection, float referenceSize)
{
    float remainingSize = referenceSize;
    float starTotal = 0;
    int starDefinitionsCount = 0;

    for (int index = 0; index < pCollection->get_Count(); index++)
    {
        DefinitionBase* pDefinition = (DefinitionBase*) (Object*)pCollection->GetItemAt(index);
        switch (pDefinition->get_SizeType())
        {
        case LayoutTimeSizeType::Pixel:
            remainingSize -= pDefinition->get_MeasureSize();
            break;

        case LayoutTimeSizeType::Auto:
            remainingSize -= pDefinition->get_MinSize();
            break;

        case LayoutTimeSizeType::Star:
        {
            m_tempDefinitions[starDefinitionsCount++] = pDefinition;

            starTotal += pDefinition->get_UserSize().get_Value();
            float maxSize = pDefinition->get_UserMaxSize();
            pDefinition->set_SizeCache(Float::IsZero(pDefinition->get_UserSize().get_Value()) ? 0 : (Float::Max(pDefinition->get_MinSize(), maxSize) / pDefinition->get_UserSize().get_Value()));
            break;
        }

        default:
            break;
        }
    }
    if (starDefinitionsCount > 0)
    {
        // Array.Sort(baseArray1, 0, starDefinitionsCount, Grid.s_starDistributionOrderComparer);

        float perStar = Float::IsZero(starTotal) ? 0 : (remainingSize / starTotal);
        for (int index = 0; index < starDefinitionsCount; index++)
        {
            DefinitionBase* pDefinition = m_tempDefinitions[index];

            float userMaxSize = pDefinition->get_UserMaxSize();
            float calculatedSize = pDefinition->get_UserSize().get_Value() * perStar;
            pDefinition->set_MeasureSize(Float::Max(pDefinition->get_MinSize(), Float::Min(calculatedSize, userMaxSize)));

            if (calculatedSize > userMaxSize)
            {
                starTotal -= pDefinition->get_UserSize().get_Value();
                remainingSize -= pDefinition->get_MeasureSize();
                perStar = Float::IsZero(starTotal) ? 0 : (remainingSize / starTotal);
            }
        }
    }
}

 
void Grid::MeasureCellsGroup(int cellsHead, const Size& referenceSize, bool ignoreDesiredSizeU, bool forceInfinityV)
{
    if (cellsHead < (int)m_cellCache.get_Count())
    {
        UIElementCollection& children = *(get_Children());
        m_tempSpans.Clear();

        do
        {
            MeasureCell(cellsHead, forceInfinityV);
            if (!ignoreDesiredSizeU)
            {
                if (m_cellCache[cellsHead].ColumnSpan == 1)
                {
                    m_columnDefinitions[m_cellCache[cellsHead].ColumnIndex]->UpdateMinSize(children[cellsHead]->get_DesiredSize().get_Width());
                }
                else
                {
                    RegisterSpan(m_cellCache[cellsHead].ColumnIndex, m_cellCache[cellsHead].ColumnSpan, true, children[cellsHead]->get_DesiredSize().get_Width());
                }
            }
            if (!forceInfinityV)
            {
                if (m_cellCache[cellsHead].RowSpan == 1)
                {
                    m_rowDefinitions[m_cellCache[cellsHead].RowIndex]->UpdateMinSize(children[cellsHead]->get_DesiredSize().get_Height());
                }
                else
                {
                    RegisterSpan(m_cellCache[cellsHead].RowIndex, m_cellCache[cellsHead].RowSpan, false, children[cellsHead]->get_DesiredSize().get_Height());
                }
            }
            cellsHead = m_cellCache[cellsHead].Next;
        }
        while (cellsHead < (int)m_cellCache.get_Count());

        for(int spanIndex = 0; spanIndex < m_tempSpans.get_Count(); spanIndex++)
        {
            SpanKey& spanKey = m_tempSpans[spanIndex];

            if(spanKey.U)
            {
                EnsureMinSizeInDefinitionRange(&m_columnDefinitions, spanKey.Start, spanKey.Count, spanKey.Value, referenceSize.get_Width());
            }
            else
            {
                EnsureMinSizeInDefinitionRange(&m_rowDefinitions, spanKey.Start, spanKey.Count, spanKey.Value, referenceSize.get_Height());
            }
        }
    }
}

float Grid::CalculateDesiredSize(ICollection* pCollection)
{
    float desiredSize = 0;

    for (int index = 0; index < pCollection->get_Count(); index++)
    {
        desiredSize += ((DefinitionBase*)(Object*)(pCollection->GetItemAt(index)))->get_MinSize();
    }
    return desiredSize;
}

 
void Grid::RegisterSpan(int start, int count, bool u, float value)
{
    for(int index = 0; index < m_tempSpans.get_Count(); index++)
    {
        SpanKey& span = m_tempSpans[index];

        if(span.Start == start && span.Count == count && span.U == u)
        {
            span.Value = value;
            return;
        }
    }

    SpanKey spanKey;

    spanKey.Start = start;
    spanKey.Count = count;
    spanKey.U = u;
    spanKey.Value = value;

    m_tempSpans.AddItem(spanKey);
}

void Grid::ValidateCells()
{
    if(m_cellsStructureDirty)
    {
        ValidateCellsCore();
        m_cellsStructureDirty = false;
    }
}

void Grid::ValidateCellsCore()
{
    UIElementCollection& children = *(get_Children());

    m_cellCache.Clear();
    m_cellCache.AddEmptyItems(children.get_Count());

    m_cellGroup1 = 0x7fffffff;
    m_cellGroup2 = 0x7fffffff;
    m_cellGroup3 = 0x7fffffff;
    m_cellGroup4 = 0x7fffffff;

    bool hasStarCellsU = false;
    bool hasStarCellsV = false;
    bool hasGroup3CellsInAutoRows = false;
    for (int cellCacheIndex = (int) m_cellCache.get_Count() - 1; cellCacheIndex >= 0; cellCacheIndex--)
    {
        UIElement* pChild = children[cellCacheIndex];
        if (pChild != NULL)
        {
            CellCache cache;

            cache.ColumnIndex = Int::Min(get_Column(pChild), m_columnDefinitions.get_Count() - 1);
            cache.RowIndex = Int::Min(get_Row(pChild), m_rowDefinitions.get_Count() - 1);
            cache.ColumnSpan = Int::Min(get_ColumnSpan(pChild), m_columnDefinitions.get_Count() - cache.ColumnIndex);
            cache.RowSpan = Int::Min(get_RowSpan(pChild), m_rowDefinitions.get_Count() - cache.RowIndex);
            cache.SizeTypeU = get_LengthTypeForRange(&m_columnDefinitions, cache.ColumnIndex, cache.ColumnSpan);
            cache.SizeTypeV = get_LengthTypeForRange(&m_rowDefinitions, cache.RowIndex, cache.RowSpan);
            hasStarCellsU |= cache.get_IsStarU();
            hasStarCellsV |= cache.get_IsStarV();

            if (!cache.get_IsStarV())
            {
                if (!cache.get_IsStarU())
                {
                    cache.Next = m_cellGroup1;
                    m_cellGroup1 = cellCacheIndex;
                }
                else
                {
                    cache.Next = m_cellGroup3;
                    m_cellGroup3 = cellCacheIndex;
                    hasGroup3CellsInAutoRows |= cache.get_IsAutoV();
                }
            }
            else if (cache.get_IsAutoU() && !cache.get_IsStarU())
            {
                cache.Next = m_cellGroup2;
                m_cellGroup2 = cellCacheIndex;
            }
            else
            {
                cache.Next = m_cellGroup4;
                m_cellGroup4 = cellCacheIndex;
            }
            m_cellCache[cellCacheIndex] = cache;
        }
    }
    m_hasStarCellsU = hasStarCellsU;
    m_hasStarCellsV = hasStarCellsV;
    m_hasGroup3CellsInAutoRows = hasGroup3CellsInAutoRows;
}
 
float Grid::get_FinalSizeForRange(ICollection* pCollection, unsigned int start, unsigned int count)
{
    float finalSize = 0;
    int currentIndex = (start + count) - 1;
    do
    {
        DefinitionBase* pDefinition = (DefinitionBase*) (Object*)pCollection->GetItemAt(currentIndex);
        finalSize += pDefinition->get_SizeCache();
    }
    while (--currentIndex >= (int)start);
    return finalSize;
}

LayoutTimeSizeType::Enum Grid::get_LengthTypeForRange(ICollection* pCollection, int start, int count)
{
      LayoutTimeSizeType::Enum type = LayoutTimeSizeType::None;

      int currentIndex = (start + count) - 1;
      do
      {
          DefinitionBase* pDefinition = (DefinitionBase*) (Object*)pCollection->GetItemAt(currentIndex);
          type = (LayoutTimeSizeType::Enum) (type | pDefinition->get_SizeType());
      }
      while (--currentIndex >= (int)start);

      return type;
}

 
void Grid::MeasureCell(int cell, bool forceInfinityV)
{
      float measureWidth;
      float measureHeight;
           
      if (m_cellCache[cell].get_IsAutoU() && !m_cellCache[cell].get_IsStarU())
      {
            measureWidth = Float::get_PositiveInfinity();
      }
      else
      {
            measureWidth = get_MeasureSizeForRange(&m_columnDefinitions, m_cellCache[cell].ColumnIndex, m_cellCache[cell].ColumnSpan);
      }
      if (forceInfinityV)
      {
            measureHeight = Float::get_PositiveInfinity();
      }
      else if (m_cellCache[cell].get_IsAutoV() && !m_cellCache[cell].get_IsStarV())
      {
            measureHeight = Float::get_PositiveInfinity();
      }
      else
      {
            measureHeight = get_MeasureSizeForRange(&m_rowDefinitions, m_cellCache[cell].RowIndex, m_cellCache[cell].RowSpan);
      }

      UIElement* pChild = (*get_Children())[cell];
      if (pChild != NULL)
      {
            pChild->Measure(Size(measureWidth, measureHeight));
      }
}

 
float Grid::get_MeasureSizeForRange(ICollection* pCollection, unsigned int start, unsigned int count)
{
    float measureSize = 0;
    int currentIndex = (start + count) - 1;
    do
    {
        DefinitionBase* pDefinition = (DefinitionBase*) (Object*)pCollection->GetItemAt(currentIndex);
        
        measureSize += pDefinition->get_SizeType() == LayoutTimeSizeType::Auto ? pDefinition->get_MinSize() : pDefinition->get_MeasureSize();
    }
    while (--currentIndex >= (int)start);

    return measureSize;
}



 
void Grid::set_FinalSize(ICollection* pCollection, float finalSize)
{
    int startIndex = 0;
    int endIndex = pCollection->get_Count();
    float diffSize = finalSize;
    float starSum = 0;
    float summedSize = 0;

    for (int definitionIndex = 0; definitionIndex < pCollection->get_Count(); definitionIndex++)
    {
        DefinitionBase* pDefinition = (DefinitionBase*)(Object*)pCollection->GetItemAt(definitionIndex);
        float maxSize = 0;
        if (pDefinition->get_UserSize().get_IsStar())
        {
            m_tempDefinitions[startIndex++] = pDefinition;
            starSum += pDefinition->get_UserSize().get_Value();
            maxSize = pDefinition->get_UserMaxSize();
            pDefinition->set_SizeCache(Float::IsZero(pDefinition->get_UserSize().get_Value()) ? 0 : (Float::Max(pDefinition->get_MinSizeForArrange(), maxSize) / pDefinition->get_UserSize().get_Value()));
            continue;
        }
        m_tempDefinitions[--endIndex] = pDefinition;

        float definitionSize = 0;
        switch (pDefinition->get_UserSize().get_GridUnitType())
        {
        case GridUnitType::Auto:
            definitionSize = pDefinition->get_MinSizeForArrange();
            break;

        case GridUnitType::Pixel:
            definitionSize = pDefinition->get_UserSize().get_Value();
            break;

        default:
            break;
        }

        maxSize = pDefinition->get_UserMaxSize();

        pDefinition->set_SizeCache(Float::Max(pDefinition->get_MinSizeForArrange(), Float::Min(definitionSize, maxSize)));

        diffSize -= pDefinition->get_SizeCache();
        summedSize += pDefinition->get_SizeCache();
    }
    // Array.Sort(baseArray1, 0, startIndex, Grid.s_starDistributionOrderComparer);

    float perStar = Float::IsZero(starSum) ? 0 : (diffSize / starSum);

    for (int starIndex = 0; starIndex < startIndex; starIndex++)
    {
        DefinitionBase* pDefinition = m_tempDefinitions[starIndex];

        float maxSize = pDefinition->get_UserMaxSize();
        float calculatedSize = pDefinition->get_UserSize().get_Value() * perStar;
        pDefinition->set_SizeCache(Float::Max(pDefinition->get_MinSizeForArrange(), Float::Min(calculatedSize, maxSize)));

        summedSize += pDefinition->get_SizeCache();
        if (calculatedSize > maxSize)
        {
            starSum -= pDefinition->get_UserSize().get_Value();
            diffSize -= pDefinition->get_SizeCache();
            perStar = Float::IsZero(starSum) ? 0 : (diffSize / starSum);
        }
    }

    if ((summedSize > finalSize) && !Float::AreClose(summedSize, finalSize))
    {
//        Array.Sort(baseArray1, 0, definitions.Length, Grid.s_distributionOrderComparer);

        float remainingSpace = finalSize - summedSize;
        for (int definitionIndex = 0; definitionIndex < pCollection->get_Count(); definitionIndex++)
        {
            DefinitionBase* pDefinition = (DefinitionBase*)(Object*)pCollection->GetItemAt(definitionIndex);
            DefinitionBase* pTempDefinition = m_tempDefinitions[definitionIndex];

            float newSize = pTempDefinition->get_SizeCache() + (remainingSpace / ((float) (pCollection->get_Count() - definitionIndex)));
            newSize = Float::Max(newSize, pTempDefinition->get_MinSizeForArrange());
            newSize = Float::Min(newSize, pTempDefinition->get_SizeCache());
            remainingSpace -= newSize - (pTempDefinition->get_SizeCache());
            pTempDefinition->set_SizeCache(newSize);
        }
    }


    ((DefinitionBase*)(Object*)(pCollection->GetItemAt(0)))->set_FinalOffset(0);
    for (int definitionIndex = 0; definitionIndex < pCollection->get_Count(); definitionIndex++)
    {
        DefinitionBase* pDefinition = (DefinitionBase*)(Object*)pCollection->GetItemAt(definitionIndex);
        DefinitionBase* pDefinitionNext = (DefinitionBase*)(Object*)pCollection->GetItemAt((definitionIndex + 1) % pCollection->get_Count());

        pDefinitionNext->set_FinalOffset(pDefinition->get_FinalOffset() + pDefinition->get_SizeCache());
    }
}

void Grid::EnsureMinSizeInDefinitionRange(ICollection* pCollection, int start, int count, float requestedSize, float percentReferenceSize)
{
    if(Float::IsZero(requestedSize))
    {
        return;
    }

    int lastIndex = start + count;
    int num2 = 0;
    float num3 = 0;
    float num4 = 0;
    float num5 = 0;
    float num6 = 0;

    for (int currentIndex = start; currentIndex < lastIndex; currentIndex++)
    {
        DefinitionBase* pDefinition = (DefinitionBase*)(Object*)pCollection->GetItemAt(currentIndex);

        float num8 = pDefinition->get_MinSize();
        float num9 = pDefinition->get_PreferredSize();
        float num10 = Float::Max(pDefinition->get_UserMaxSize(), num8);
        num3 += num8;
        num4 += num9;
        num5 += num10;

        pDefinition->set_SizeCache(num10);
        if (num6 < num10)
        {
            num6 = num10;
        }
        if (pDefinition->get_UserSize().get_IsAuto())
        {
            num2++;
        }
        m_tempDefinitions[currentIndex - start] = pDefinition;
    }

    if (requestedSize > num3)
    {
        if (requestedSize <= num4)
        {
//            Array.Sort(m_tempDefinitions, 0, count, Grid.s_spanPreferredDistributionOrderComparer);
            int num12 = 0;
            float num11 = requestedSize;
            while (num12 < num2)
            {
                num11 -= m_tempDefinitions[num12]->get_MinSize();
                num12++;
            }
            while (num12 < count)
            {
                float num13 = Float::Min(num11 / ((float) (count - num12)), m_tempDefinitions[num12]->get_PreferredSize());
                if (num13 > m_tempDefinitions[num12]->get_MinSize())
                {
                    m_tempDefinitions[num12]->UpdateMinSize(num13);
                }
                num11 -= num13;
                num12++;
            }
        }
        else if (requestedSize <= num5)
        {
//            Array.Sort(m_tempDefinitions, 0, count, Grid.s_spanMaxDistributionOrderComparer);
            int num15 = 0;
            float num14 = requestedSize - num4;
            while (num15 < (count - num2))
            {
                float num16 = m_tempDefinitions[num15]->get_PreferredSize();
                float num17 = num16 + (num14 / ((float) ((count - num2) - num15)));
                m_tempDefinitions[num15]->UpdateMinSize(Float::Min(num17, m_tempDefinitions[num15]->get_SizeCache()));
                num14 -= m_tempDefinitions[num15]->get_MinSize() - num16;
                num15++;
            }
            while (num15 < count)
            {
                float num18 = m_tempDefinitions[num15]->get_MinSize();
                float num19 = num18 + (num14 / ((float) (count - num15)));
                m_tempDefinitions[num15]->UpdateMinSize(Float::Min(num19, m_tempDefinitions[num15]->get_SizeCache()));
                num14 -= m_tempDefinitions[num15]->get_MinSize() - num18;
                num15++;
            }
        }
        else
        {
            float num20 = requestedSize / ((float) count);
            if ((num20 < num6) && !Float::AreClose(num20, num6))
            {
                float num21 = (num6 * count) - num5;
                float num22 = requestedSize - num5;
                for (int num23 = 0; num23 < count; num23++)
                {
                    float num24 = ((num6 - m_tempDefinitions[num23]->get_SizeCache()) * num22) / num21;
                    m_tempDefinitions[num23]->UpdateMinSize(m_tempDefinitions[num23]->get_SizeCache() + num24);
                }
            }
            else
            {
                for (int num25 = 0; num25 < count; num25++)
                {
                    m_tempDefinitions[num25]->UpdateMinSize(num20);
                }
            }
        }
    }
}

void Grid::OnCellAttachedPropertyChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    Visual* pVisual = object_cast<Visual>(pDO);
    if(pVisual == NULL)
    {
        return;
    }

    pVisual = pVisual->get_VisualParent();
    if(pVisual == NULL)
    {
        return;
    }
    Grid* pGrid = object_cast<Grid>(pVisual);
    if(pGrid == NULL)
    {
        return;
    }
    pGrid->m_cellsStructureDirty = true;
    pGrid->InvalidateMeasure();
}


float Grid::GetFinalColumnDefinitionWidth(int columnIndex)
{
    float finalOffset = 0;

    ColumnDefinitionCollection* pColumns = get_ColumnDefinitions();

    finalOffset = (*pColumns)[(columnIndex + 1) % pColumns->get_Count()]->get_FinalOffset();
    if (columnIndex != 0)
    {
        finalOffset -= (*pColumns)[columnIndex]->get_FinalOffset();
    }
    return finalOffset;
}

float Grid::GetFinalRowDefinitionHeight(int rowIndex)
{
    float finalOffset = 0;

    RowDefinitionCollection* pRows = get_RowDefinitions();

    finalOffset = (*pRows)[(rowIndex + 1) % pRows->get_Count()]->get_FinalOffset();
    if (rowIndex != 0)
    {
        finalOffset -= (*pRows)[rowIndex]->get_FinalOffset();
    }
    return finalOffset;
}


}; // namespace AVUI
