#pragma once

#include <AVUIDependencyObject.h>
#include <AVUIGridLength.h>

namespace AVUI {

class Grid;

class DefinitionBase : public DependencyObject
{
    friend class GridSplitter;
public:

    friend class RowDefinition;
    friend class ColumnDefinition;
    friend class Grid;

    DECLARE_ELEMENT(DefinitionBase, DependencyObject);

private:

    DefinitionBase(bool isColumnDefinition) : m_isColumnDefinition(isColumnDefinition), m_layoutWasUpdated(false), m_sizeType(LayoutTimeSizeType::None) { }

    float get_FinalOffset() { return m_finalOffset; }
    void set_FinalOffset(float finalOffset) { m_finalOffset = finalOffset; }

    float get_UserMaxSize();
    float get_UserMinSize();

    float get_MinSize() { return m_minSize; }
    float get_MinSizeForArrange() { return m_minSize; }

    float get_SizeCache() { return m_sizeCache; }
    void  set_SizeCache(float sizeCache) { m_sizeCache = sizeCache; }

    float get_MeasureSize() { return m_measureSize; };
    void set_MeasureSize(float measureSize) { m_measureSize = measureSize; }

    float get_PreferredSize()
    {
        float preferredSize = get_MinSize();
        if(m_sizeType != LayoutTimeSizeType::Auto && preferredSize < m_measureSize)
        {
            preferredSize = m_measureSize;
        }
        return preferredSize;
    }

    void UpdateMinSize(float minSize) { m_minSize = Float::Max(m_minSize, minSize); }

    const GridLength& get_UserSize();

    LayoutTimeSizeType::Enum get_SizeType() { return m_sizeType; }
    void set_SizeType(LayoutTimeSizeType::Enum sizeType) { m_sizeType = sizeType; }

    void OnBeforeLayout(Grid* pGrid);

    TWeakPtr<Grid> m_pGrid;

    bool m_isColumnDefinition;
    float m_measureSize;
    float m_minSize;
    float m_finalOffset;
    float m_sizeCache;

    bool m_layoutWasUpdated;
    LayoutTimeSizeType::Enum m_sizeType;
};

}; // namespace AVUI
