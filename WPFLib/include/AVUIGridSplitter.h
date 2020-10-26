
#pragma once

#include <AVUIThumb.h>
#include <AVUIGridLength.h>

namespace AVUI {
class DefinitionBase;
class Grid;

class GridSplitter : public Thumb
{
public:
    GridSplitter() { };
    ~GridSplitter() { };

    DECLARE_ELEMENT(GridSplitter, Thumb);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(ResizeDirection, GridResizeDirection, GridResizeDirection::Auto, DependencyPropertyMetadata(PropertyFlag::None, GridSplitter::UpdateCursor));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(ResizeBehavior, GridResizeBehavior, GridResizeBehavior::BasedOnAlignment, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyboardIncrement, float, 10.0f, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(DragIncrement, float, 1.0f, PropertyFlag::AffectsMeasure);

protected:

    virtual void OnRenderSizeChanged(SizeChangedInfo* pSizeInfo);

private:
    void MoveSplitter(float horizontalChange, float verticalChange);

    class SplitBehavior
    {
    public:
        enum Enum
        {
            Resize1,
            Resize2,
            Split,
        };           
    };

    class ResizeData : public Object
    {
    public:
        // Fields
        DefinitionBase* Definition1;
        int Definition1Index;
        DefinitionBase* Definition2;
        int Definition2Index;
        Grid* pGrid;
        float MaxChange;
        float MinChange;
        float OriginalDefinition1ActualLength;
        GridLength OriginalDefinition1Length;
        float OriginalDefinition2ActualLength;
        GridLength OriginalDefinition2Length;
        GridResizeBehavior::Enum ResizeBehavior;
        GridResizeDirection::Enum ResizeDirection;
        SplitBehavior::Enum SplitBehavior;
        int SplitterIndex;
        float SplitterLength;
    };
     



    static void OnDragStarted(Object* pSender, RoutedEventArgs* pEventArgs) { ((GridSplitter*)pSender)->OnDragStarted(pEventArgs); }
    static void OnDragDelta(Object* pSender, DragDeltaEventArgs* pEventArgs) { ((GridSplitter*)pSender)->OnDragDelta(pEventArgs); }
    static void OnDragCompleted(Object* pSender, RoutedEventArgs* pEventArgs) { ((GridSplitter*)pSender)->OnDragCompleted(pEventArgs); }

    void OnDragStarted(RoutedEventArgs* pEventArgs);
    void OnDragDelta(DragDeltaEventArgs* pEventArgs);
    void OnDragCompleted(RoutedEventArgs* pEventArgs);

    static void StaticConstructor();

    void InitializeData();
    DefinitionBase* GetGridDefinition(Grid* pGrid, int index, GridResizeDirection::Enum direction);
    GridResizeBehavior::Enum GetEffectiveResizeBehavior(GridResizeDirection::Enum direction);
    GridResizeDirection::Enum GetEffectiveResizeDirection();
    bool SetupDefinitionsToResize();
    bool IsStar(DefinitionBase* pDefinition);
    float GetActualLength(DefinitionBase* pDefinition);
    void SetLengths(float definition1Pixels, float definition2Pixels);
    void SetDefinitionLength(DefinitionBase* pDefinition, const GridLength& length);
    void CancelResize();
    void GetDeltaConstraints(float* pMinDelta, float* pMaxDelta);
    static TRefCountedPtr<Object> CoerceCursor(DependencyObject* pDO, Object* pValue);
    static void UpdateCursor(Object* pDO, const DependencyPropertyChangedEventArgs& e);

 

    TRefCountedPtr<ResizeData> m_pResizeData;
};

}; // namespace AVUI
 
