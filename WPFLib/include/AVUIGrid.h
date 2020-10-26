#pragma once

#include <AVUIPanel.h>
#include <AVUIRowDefinitionCollection.h>
#include <AVUIColumnDefinitionCollection.h>

namespace AVUI {

class Grid : public Panel
{
    friend class RowDefinitionCollection;
    friend class ColumnDefinitionCollection;
    friend class RowDefinition;
    friend class ColumnDefinition;
public:
    Grid();
    ~Grid();

    DECLARE_ELEMENT(Grid, Panel);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ShowGridLines,   bool, false, PropertyFlag::AffectsRender);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Column,   int, 0, DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnCellAttachedPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Row,   int, 0, DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnCellAttachedPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ColumnSpan,   int, 1, DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnCellAttachedPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RowSpan,   int, 1, DependencyPropertyMetadata(PropertyFlag::IsAttachedProperty, OnCellAttachedPropertyChanged));

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(ColumnDefinitions, ColumnDefinitionCollection, false);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(RowDefinitions, RowDefinitionCollection, false);

    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

    RowDefinitionCollection* get_RowDefinitions() { return &m_rowDefinitions; }
    ColumnDefinitionCollection* get_ColumnDefinitions() { return &m_columnDefinitions; }

private:

    static int get_Column(DependencyObject* pChild) { return UnboxValue<int>(pChild->GetValue(get_ColumnProperty())); }
    static int get_Row(DependencyObject* pChild) { return UnboxValue<int>(pChild->GetValue(get_RowProperty())); }

    static int get_ColumnSpan(DependencyObject* pChild) { return UnboxValue<int>(pChild->GetValue(get_ColumnSpanProperty())); }
    static int get_RowSpan(DependencyObject* pChild) { return UnboxValue<int>(pChild->GetValue(get_RowSpanProperty())); }


    struct SpanKey
    {
        int Start;
        int Count;
        bool U;
        float Value;
    };


    struct CellCache
    {
        int ColumnIndex;
        int RowIndex;
        int ColumnSpan;
        int RowSpan;
        LayoutTimeSizeType::Enum SizeTypeU;
        LayoutTimeSizeType::Enum SizeTypeV;
        int Next;

        bool get_IsStarU() { return (SizeTypeU & LayoutTimeSizeType::Star) != 0; }
        bool get_IsAutoU() { return (SizeTypeU & LayoutTimeSizeType::Auto) != 0; }
        bool get_IsStarV() { return (SizeTypeV & LayoutTimeSizeType::Star) != 0; }
        bool get_IsAutoV() { return (SizeTypeV & LayoutTimeSizeType::Auto) != 0; }
    }; 


    Object* get_RowDefinitionsValue() { return &m_rowDefinitions; }
    Object* get_ColumnDefinitionsValue() { return &m_columnDefinitions; }

    void ValidateDefinitionsLayout(ICollection* pCollection, bool treatStarAsAuto);
    void ValidateCells();
    void ValidateCellsCore();
    void MeasureCellsGroup(int cellsHead, const Size& referenceSize, bool ignoreDesiredSizeU, bool forceInfinityV);
    void ResolveStar(ICollection* pCollection, float referenceSize);
    float CalculateDesiredSize(ICollection* pCollection);

    void set_Valid();
    void set_FinalSize(ICollection* pCollection, float finalSize);
    float get_FinalSizeForRange(ICollection* pCollection, unsigned int start, unsigned int count);
    LayoutTimeSizeType::Enum get_LengthTypeForRange(ICollection* pCollection, int start, int count);
    float GetFinalColumnDefinitionWidth(int columnIndex);
    float GetFinalRowDefinitionHeight(int rowIndex);

 
 

    void RegisterSpan(int start, int count, bool u, float value);
    void MeasureCell(int cell, bool forceInfinityV);
    float get_MeasureSizeForRange(ICollection* pCollection, unsigned int start, unsigned int count);
    void EnsureMinSizeInDefinitionRange(ICollection* pCollection, int start, int count, float requestedSize, float percentReferenceSize);

 
    RowDefinitionCollection m_rowDefinitions;
    ColumnDefinitionCollection m_columnDefinitions;

    void PrivateColumnDefinitionsChanged() { InvalidateMeasure(); }
    void PrivateRowDefinitionsChanged() { InvalidateMeasure(); }
    virtual void OnChildrenChanged() { m_cellsStructureDirty = true; Panel::OnChildrenChanged(); }
    static void OnCellAttachedPropertyChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    struct 
    {
        bool m_sizeToContentU : 1;
        bool m_sizeToContentV : 1;
        bool m_listenToNotifications : 1;
        bool m_measureOverrideInProgress : 1;
        bool m_arrangeOverrideInProgress : 1;
        bool m_cellsStructureDirty : 1;
        bool m_hasStarCellsU : 1;
        bool m_hasStarCellsV : 1;
        bool m_hasGroup3CellsInAutoRows : 1;
    };

    int m_cellGroup1;
    int m_cellGroup2;
    int m_cellGroup3;
    int m_cellGroup4;

    ItemStructList<CellCache> m_cellCache;
    ItemStructList<DefinitionBase*> m_tempDefinitions;
    ItemStructList<SpanKey> m_tempSpans;
};


}; // namespace AVUI
