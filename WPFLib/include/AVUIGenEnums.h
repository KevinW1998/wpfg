#pragma once
#include <AVUIEnumClass.h>


namespace AVUI {
class MenuItemRole : public EnumClass
{
public:
    enum Enum
    {
        TopLevelItem,
        TopLevelHeader,
        SubmenuItem,
        SubmenuHeader,
    };

    MenuItemRole() : EnumClass(GET_TYPE(MenuItemRole)) { };
    ~MenuItemRole() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ResizeMode : public EnumClass
{
public:
    enum Enum
    {
        NoResize,
        CanMinimize,
        CanResize,
        CanResizeWithGrip,
    };

    ResizeMode() : EnumClass(GET_TYPE(ResizeMode)) { };
    ~ResizeMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class WindowState : public EnumClass
{
public:
    enum Enum
    {
        Normal,
        Minimized,
        Maximized,
    };

    WindowState() : EnumClass(GET_TYPE(WindowState)) { };
    ~WindowState() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class WindowStyle : public EnumClass
{
public:
    enum Enum
    {
        None,
        SingleBorderWindow,
        ThreeDBorderWindow,
        ToolWindow,
    };

    WindowStyle() : EnumClass(GET_TYPE(WindowStyle)) { };
    ~WindowStyle() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class MouseButtonState : public EnumClass
{
public:
    enum Enum
    {
        Released,
        Pressed,
    };

    MouseButtonState() : EnumClass(GET_TYPE(MouseButtonState)) { };
    ~MouseButtonState() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class PopupAnimation : public EnumClass
{
public:
    enum Enum
    {
        None,
        Fade,
        Slide,
        Scroll,
    };

    PopupAnimation() : EnumClass(GET_TYPE(PopupAnimation)) { };
    ~PopupAnimation() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class PlacementMode : public EnumClass
{
public:
    enum Enum
    {
        Absolute,
        Relative,
        Bottom,
        Center,
        Right,
        AbsolutePoint,
        RelativePoint,
        Mouse,
        MousePoint,
        Left,
        Top,
        Custom,
    };

    PlacementMode() : EnumClass(GET_TYPE(PlacementMode)) { };
    ~PlacementMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class CursorType : public EnumClass
{
public:
    enum Enum
    {
        None,
        No,
        Arrow,
        AppStarting,
        Cross,
        Help,
        IBeam,
        SizeAll,
        SizeNESW,
        SizeNS,
        SizeNWSE,
        SizeWE,
        UpArrow,
        Wait,
        Hand,
        Pen,
        ScrollNS,
        ScrollWE,
        ScrollAll,
        ScrollN,
        ScrollS,
        ScrollW,
        ScrollE,
        ScrollNW,
        ScrollNE,
        ScrollSW,
        ScrollSE,
        ArrowCD,
    };

    CursorType() : EnumClass(GET_TYPE(CursorType)) { };
    ~CursorType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class BindingStatus : public EnumClass
{
public:
    enum Enum
    {
        Unattached,
        Inactive,
        Active,
        Detached,
        AsyncRequestPending,
        PathError,
        UpdateTargetError,
        UpdateSourceError,
    };

    BindingStatus() : EnumClass(GET_TYPE(BindingStatus)) { };
    ~BindingStatus() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class BindingMode : public EnumClass
{
public:
    enum Enum
    {
        TwoWay,
        OneWay,
        OneTime,
        OneWayToSource,
        Default,
    };

    BindingMode() : EnumClass(GET_TYPE(BindingMode)) { };
    ~BindingMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class UpdateSourceTrigger : public EnumClass
{
public:
    enum Enum
    {
        Default,
        PropertyChanged,
        LostFocus,
        Explicit,
    };

    UpdateSourceTrigger() : EnumClass(GET_TYPE(UpdateSourceTrigger)) { };
    ~UpdateSourceTrigger() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class FlowDirection : public EnumClass
{
public:
    enum Enum
    {
        LeftToRight,
        RightToLeft,
    };

    FlowDirection() : EnumClass(GET_TYPE(FlowDirection)) { };
    ~FlowDirection() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class RelativeSourceMode : public EnumClass
{
public:
    enum Enum
    {
        PreviousData,
        TemplatedParent,
        Self,
        FindAncestor,
    };

    RelativeSourceMode() : EnumClass(GET_TYPE(RelativeSourceMode)) { };
    ~RelativeSourceMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class StretchDirection : public EnumClass
{
public:
    enum Enum
    {
        Both,
        UpOnly,
        DownOnly,
    };

    StretchDirection() : EnumClass(GET_TYPE(StretchDirection)) { };
    ~StretchDirection() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class BitmapCreateOptions : public EnumClass
{
public:
    enum Enum
    {
        None = 0,
        PreservePixelFormat = 1,
        DelayCreation = 2,
        IgnoreColorProfile = 4,
        IgnoreImageCache = 8,
    };

    BitmapCreateOptions() : EnumClass(GET_TYPE(BitmapCreateOptions)) { };
    ~BitmapCreateOptions() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class BitmapCacheOption : public EnumClass
{
public:
    enum Enum
    {
        Default = 0,
        OnDemand = 0,
        OnLoad = 1,
        None = 2,
    };

    BitmapCacheOption() : EnumClass(GET_TYPE(BitmapCacheOption)) { };
    ~BitmapCacheOption() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class Dock : public EnumClass
{
public:
    enum Enum
    {
        Left,
        Top,
        Right,
        Bottom,
    };

    Dock() : EnumClass(GET_TYPE(Dock)) { };
    ~Dock() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class GridViewColumnHeaderRole : public EnumClass
{
public:
    enum Enum
    {
        Normal,
        Floating,
        Padding,
    };

    GridViewColumnHeaderRole() : EnumClass(GET_TYPE(GridViewColumnHeaderRole)) { };
    ~GridViewColumnHeaderRole() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ColumnMeasureState : public EnumClass
{
public:
    enum Enum
    {
        Init,
        Headered,
        Data,
        SpecificWidth,
    };

    ColumnMeasureState() : EnumClass(GET_TYPE(ColumnMeasureState)) { };
    ~ColumnMeasureState() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class SelectionMode : public EnumClass
{
public:
    enum Enum
    {
        Single,
        Multiple,
        Extended,
    };

    SelectionMode() : EnumClass(GET_TYPE(SelectionMode)) { };
    ~SelectionMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class GeneratorDirection : public EnumClass
{
public:
    enum Enum
    {
        Forward,
        Backward,
    };

    GeneratorDirection() : EnumClass(GET_TYPE(GeneratorDirection)) { };
    ~GeneratorDirection() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class GeneratorStatus : public EnumClass
{
public:
    enum Enum
    {
        NotStarted,
        GeneratingContainers,
        ContainersGenerated,
        Error,
    };

    GeneratorStatus() : EnumClass(GET_TYPE(GeneratorStatus)) { };
    ~GeneratorStatus() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class CollectionViewFlags : public EnumClass
{
public:
    enum Enum
    {
        UpdatedOutsideDispatcher = 0x2,
        ShouldProcessCollectionChanged = 0x4,
        NeedsRefresh = 0x80,
        IsMultiThreadCollectionChangeAllowed = 0x100,
        IsDynamic = 0x20,
        IsDataInGroupOrder = 0x40,
        CachedIsEmpty = 0x200,
        IsCurrentAfterLast = 0x10,
        IsCurrentBeforeFirst = 0x8,
    };

    CollectionViewFlags() : EnumClass(GET_TYPE(CollectionViewFlags)) { };
    ~CollectionViewFlags() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class GridResizeDirection : public EnumClass
{
public:
    enum Enum
    {
        Auto,
        Columns,
        Rows,
    };

    GridResizeDirection() : EnumClass(GET_TYPE(GridResizeDirection)) { };
    ~GridResizeDirection() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class GridResizeBehavior : public EnumClass
{
public:
    enum Enum
    {
        BasedOnAlignment,
        CurrentAndNext,
        PreviousAndCurrent,
        PreviousAndNext,
    };

    GridResizeBehavior() : EnumClass(GET_TYPE(GridResizeBehavior)) { };
    ~GridResizeBehavior() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class NavigationStatus : public EnumClass
{
public:
    enum Enum
    {
        Idle,
        Navigating,
        NavigationFailed,
        Navigated,
        Stopped,
    };

    NavigationStatus() : EnumClass(GET_TYPE(NavigationStatus)) { };
    ~NavigationStatus() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class NotifyCollectionChangedAction : public EnumClass
{
public:
    enum Enum
    {
        Add,
        Remove,
        Replace,
        Move,
        Reset,
    };

    NotifyCollectionChangedAction() : EnumClass(GET_TYPE(NotifyCollectionChangedAction)) { };
    ~NotifyCollectionChangedAction() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class TileMode : public EnumClass
{
public:
    enum Enum
    {
        None,
        FlipX,
        FlipY,
        FlipXY,
        Tile,
    };

    TileMode() : EnumClass(GET_TYPE(TileMode)) { };
    ~TileMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class SweepDirection : public EnumClass
{
public:
    enum Enum
    {
        Clockwise,
        Counterclockwise,
    };

    SweepDirection() : EnumClass(GET_TYPE(SweepDirection)) { };
    ~SweepDirection() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class AlignmentX : public EnumClass
{
public:
    enum Enum
    {
        Left,
        Center,
        Right,
    };

    AlignmentX() : EnumClass(GET_TYPE(AlignmentX)) { };
    ~AlignmentX() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class AlignmentY : public EnumClass
{
public:
    enum Enum
    {
        Top,
        Center,
        Bottom,
    };

    AlignmentY() : EnumClass(GET_TYPE(AlignmentY)) { };
    ~AlignmentY() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class BrushMappingMode : public EnumClass
{
public:
    enum Enum
    {
        Absolute,
        RelativeToBoundingBox,
    };

    BrushMappingMode() : EnumClass(GET_TYPE(BrushMappingMode)) { };
    ~BrushMappingMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class TextAlignment : public EnumClass
{
public:
    enum Enum
    {
        Left,
        Right,
        Center,
        Justify,
    };

    TextAlignment() : EnumClass(GET_TYPE(TextAlignment)) { };
    ~TextAlignment() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class GradientSpreadMethod : public EnumClass
{
public:
    enum Enum
    {
        Pad,
        Reflect,
        Repeat,
    };

    GradientSpreadMethod() : EnumClass(GET_TYPE(GradientSpreadMethod)) { };
    ~GradientSpreadMethod() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class TextWrapping : public EnumClass
{
public:
    enum Enum
    {
        NoWrap,
        Wrap,
    };

    TextWrapping() : EnumClass(GET_TYPE(TextWrapping)) { };
    ~TextWrapping() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class TextTrimming : public EnumClass
{
public:
    enum Enum
    {
        CharacterEllipsis,
        None,
    };

    TextTrimming() : EnumClass(GET_TYPE(TextTrimming)) { };
    ~TextTrimming() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ScrollBarVisibility : public EnumClass
{
public:
    enum Enum
    {
        Disabled,
        Auto,
        Hidden,
        Visible,
    };

    ScrollBarVisibility() : EnumClass(GET_TYPE(ScrollBarVisibility)) { };
    ~ScrollBarVisibility() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class Visibility : public EnumClass
{
public:
    enum Enum
    {
        Collapsed,
        Hidden,
        Visible,
    };

    Visibility() : EnumClass(GET_TYPE(Visibility)) { };
    ~Visibility() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class GridUnitType : public EnumClass
{
public:
    enum Enum
    {
        Auto,
        Pixel,
        Star,
    };

    GridUnitType() : EnumClass(GET_TYPE(GridUnitType)) { };
    ~GridUnitType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class MouseButton : public EnumClass
{
public:
    enum Enum
    {
        Left,
        Middle,
        Right,
    };

    MouseButton() : EnumClass(GET_TYPE(MouseButton)) { };
    ~MouseButton() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class LayoutTimeSizeType : public EnumClass
{
public:
    enum Enum
    {
        None = 0x0,
        Pixel = 0x1,
        Auto = 0x2,
        Star = 0x4,
    };

    LayoutTimeSizeType() : EnumClass(GET_TYPE(LayoutTimeSizeType)) { };
    ~LayoutTimeSizeType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class FillRule : public EnumClass
{
public:
    enum Enum
    {
        EvenOdd,
        NonZero,
    };

    FillRule() : EnumClass(GET_TYPE(FillRule)) { };
    ~FillRule() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class RoutingStrategy : public EnumClass
{
public:
    enum Enum
    {
        Tunnel,
        Bubble,
        Direct,
        Broadcast,
    };

    RoutingStrategy() : EnumClass(GET_TYPE(RoutingStrategy)) { };
    ~RoutingStrategy() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class SourceValueType : public EnumClass
{
public:
    enum Enum
    {
        Property,
        Indexer,
        Direct,
    };

    SourceValueType() : EnumClass(GET_TYPE(SourceValueType)) { };
    ~SourceValueType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class DrillIn : public EnumClass
{
public:
    enum Enum
    {
        Always,
        Never,
        IfNeeded,
    };

    DrillIn() : EnumClass(GET_TYPE(DrillIn)) { };
    ~DrillIn() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class SlipBehavior : public EnumClass
{
public:
    enum Enum
    {
        Grow,
        Slip,
    };

    SlipBehavior() : EnumClass(GET_TYPE(SlipBehavior)) { };
    ~SlipBehavior() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class LogicalOp : public EnumClass
{
public:
    enum Enum
    {
        Equals,
        NotEquals,
    };

    LogicalOp() : EnumClass(GET_TYPE(LogicalOp)) { };
    ~LogicalOp() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class AnimationType : public EnumClass
{
public:
    enum Enum
    {
        Automatic,
        From,
        To,
        By,
        FromTo,
        FromBy,
    };

    AnimationType() : EnumClass(GET_TYPE(AnimationType)) { };
    ~AnimationType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class FillBehavior : public EnumClass
{
public:
    enum Enum
    {
        HoldEnd,
        Stop,
    };

    FillBehavior() : EnumClass(GET_TYPE(FillBehavior)) { };
    ~FillBehavior() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class TimeState : public EnumClass
{
public:
    enum Enum
    {
        Stopped,
        Paused,
        Running,
    };

    TimeState() : EnumClass(GET_TYPE(TimeState)) { };
    ~TimeState() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ClockState : public EnumClass
{
public:
    enum Enum
    {
        Active,
        Filling,
        Stopped,
    };

    ClockState() : EnumClass(GET_TYPE(ClockState)) { };
    ~ClockState() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ClockFlags : public EnumClass
{
public:
    enum Enum
    {
        None = 0x0,
        IsTimeManager = 0x1,
        IsBackwardsProgressingGlobal = 0x2,
        IsInteractivelyPaused = 0x4,
        IsInteractivelyStopped = 0x8,
        PendingInteractivePause = 0x10,
        PendingInteractiveResume = 0x20,
        PendingInteractiveStop = 0x40,
        PendingInteractiveRemove = 0x80,
        CanSlip = 0x100,
        CurrentStateInvalidatedEventRaised = 0x200,
        CurrentTimeInvalidatedEventRaised = 0x400,
        CurrentGlobalSpeedInvalidatedEventRaised = 0x800,
        CompletedEventRaised = 0x1000,
        RemoveRequestedEventRaised = 0x2000,
        IsInEventQueue = 0x4000,
        NeedsTicksWhenActive = 0x8000,
        NeedsPostfixTraversal = 0x10000,
        PauseStateChangeDuringTick = 0x20000,
        RootBeginPending = 0x40000,
        HasControllableRoot = 0x80000,
        AutoReverse = 0x100000,
    };

    ClockFlags() : EnumClass(GET_TYPE(ClockFlags)) { };
    ~ClockFlags() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class PropertyValueType : public EnumClass
{
public:
    enum Enum
    {
        Set,
        Trigger,
        PropertyTriggerResource,
        DataTrigger,
        DataTriggerResource,
        TemplateBinding,
        Resource,
    };

    PropertyValueType() : EnumClass(GET_TYPE(PropertyValueType)) { };
    ~PropertyValueType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ValueLookupType : public EnumClass
{
public:
    enum Enum
    {
        Simple,
        Trigger,
        PropertyTriggerResource,
        DataTrigger,
        DataTriggerResource,
        TemplateBinding,
        Resource,
    };

    ValueLookupType() : EnumClass(GET_TYPE(ValueLookupType)) { };
    ~ValueLookupType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class RequestFlags : public EnumClass
{
public:
    enum Enum
    {
        FullyResolved = 0,
        AnimationBaseValue = 1,
        CoercionBaseValue = 2,
        DeferredReferences = 4,
        SkipDefault = 8,
        RawEntry = 0x10,
    };

    RequestFlags() : EnumClass(GET_TYPE(RequestFlags)) { };
    ~RequestFlags() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class OperationType : public EnumClass
{
public:
    enum Enum
    {
        Unknown,
        AddChild,
        RemoveChild,
        Inherit,
        ChangeMutableDefaultValue,
    };

    OperationType() : EnumClass(GET_TYPE(OperationType)) { };
    ~OperationType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class BaseValueSource : public EnumClass
{
public:
    enum Enum
    {
        Unknown,
        Default,
        Inherited,
        ThemeStyle,
        ThemeStyleTrigger,
        Style,
        TemplateTrigger,
        StyleTrigger,
        ImplicitReference,
        ParentTemplate,
        ParentTemplateTrigger,
        Local,
    };

    BaseValueSource() : EnumClass(GET_TYPE(BaseValueSource)) { };
    ~BaseValueSource() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class FullValueSource : public EnumClass
{
public:
    enum Enum
    {
        IsExpression = 0x10,
        IsAnimated = 0x20,
        IsCoerced = 0x40,
        IsDeferredReference = 0x80,
        HasExpressionMarker = 0x100,
        ModifiersMask = 0x70,
        ValueSourceMask = 0xF,
    };

    FullValueSource() : EnumClass(GET_TYPE(FullValueSource)) { };
    ~FullValueSource() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class UpdateResult : public EnumClass
{
public:
    enum Enum
    {
        None = 0,
        ValueChanged = 1,
        NotificationSent = 2,
    };

    UpdateResult() : EnumClass(GET_TYPE(UpdateResult)) { };
    ~UpdateResult() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class Stretch : public EnumClass
{
public:
    enum Enum
    {
        None,
        Fill,
        Uniform,
        UniformToFill,
    };

    Stretch() : EnumClass(GET_TYPE(Stretch)) { };
    ~Stretch() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class PropertyFlag : public EnumClass
{
public:
    enum Enum
    {
        None = 0x0,
        AffectsMeasure = 0x1,
        AffectsArrange = 0x2,
        AffectsRender = 0x4,
        AutoCreate = 0x10,
        ReadOnly = 0x20,
        IsContentProperty = 0x40,
        IsAttachedProperty = 0x80,
        IgnoreSubPropertyInvalidation = 0x100,
        Inherits = 0x200,
    };

    PropertyFlag() : EnumClass(GET_TYPE(PropertyFlag)) { };
    ~PropertyFlag() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class HorizontalAlignment : public EnumClass
{
public:
    enum Enum
    {
        Left,
        Right,
        Center,
        Stretch,
    };

    HorizontalAlignment() : EnumClass(GET_TYPE(HorizontalAlignment)) { };
    ~HorizontalAlignment() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class VerticalAlignment : public EnumClass
{
public:
    enum Enum
    {
        Top,
        Bottom,
        Center,
        Stretch,
    };

    VerticalAlignment() : EnumClass(GET_TYPE(VerticalAlignment)) { };
    ~VerticalAlignment() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class Orientation : public EnumClass
{
public:
    enum Enum
    {
        Vertical,
        Horizontal,
    };

    Orientation() : EnumClass(GET_TYPE(Orientation)) { };
    ~Orientation() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class HandoffBehavior : public EnumClass
{
public:
    enum Enum
    {
        SnapshotAndReplace,
        Compose,
    };

    HandoffBehavior() : EnumClass(GET_TYPE(HandoffBehavior)) { };
    ~HandoffBehavior() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ClickMode : public EnumClass
{
public:
    enum Enum
    {
        Release,
        Press,
        Hover,
    };

    ClickMode() : EnumClass(GET_TYPE(ClickMode)) { };
    ~ClickMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class KnownColor : public EnumClass
{
public:
    enum Enum
    {
        AliceBlue = 0xfff0f8ff,
        AntiqueWhite = 0xfffaebd7,
        Aqua = 0xff00ffff,
        Aquamarine = 0xff7fffd4,
        Azure = 0xfff0ffff,
        Bisque = 0xffffe4c4,
        Black = 0xff000000,
        BlanchedAlmond = 0xffffebcd,
        Blue = 0xff0000ff,
        BlueViolet = 0xff8a2be2,
        Brown = 0xffa52a2a,
        BurlyWood = 0xffdeb887,
        CadetBlue = 0xff5f9ea0,
        Chartreuse = 0xff7fff00,
        Chocolate = 0xffd2691e,
        Coral = 0xffff7f50,
        CornflowerBlue = 0xff6495ed,
        Cornsilk = 0xfffff8dc,
        Crimson = 0xffdc143c,
        Cyan = 0xff00ffff,
        DarkBlue = 0xff00008b,
        DarkCyan = 0xff008b8b,
        DarkGoldenrod = 0xffb8860b,
        DarkGray = 0xffa9a9a9,
        DarkGreen = 0xff006400,
        DarkKhaki = 0xffbdb76b,
        DarkMagenta = 0xff8b008b,
        DarkOliveGreen = 0xff556b2f,
        DarkOrange = 0xffff8c00,
        DarkOrchid = 0xff9932cc,
        DarkRed = 0xff8b0000,
        DarkSalmon = 0xffe9967a,
        DarkSeaGreen = 0xff8fbc8f,
        DarkSlateBlue = 0xff483d8b,
        DarkSlateGray = 0xff2f4f4f,
        DarkTurquoise = 0xff00ced1,
        DarkViolet = 0xff9400d3,
        DeepPink = 0xffff1493,
        DeepSkyBlue = 0xff00bfff,
        DimGray = 0xff696969,
        DodgerBlue = 0xff1e90ff,
        Firebrick = 0xffb22222,
        FloralWhite = 0xfffffaf0,
        ForestGreen = 0xff228b22,
        Fuchsia = 0xffff00ff,
        Gainsboro = 0xffdcdcdc,
        GhostWhite = 0xfff8f8ff,
        Gold = 0xffffd700,
        Goldenrod = 0xffdaa520,
        Gray = 0xff808080,
        Green = 0xff008000,
        GreenYellow = 0xffadff2f,
        Honeydew = 0xfff0fff0,
        HotPink = 0xffff69b4,
        IndianRed = 0xffcd5c5c,
        Indigo = 0xff4b0082,
        Ivory = 0xfffffff0,
        Khaki = 0xfff0e68c,
        Lavender = 0xffe6e6fa,
        LavenderBlush = 0xfffff0f5,
        LawnGreen = 0xff7cfc00,
        LemonChiffon = 0xfffffacd,
        LightBlue = 0xffadd8e6,
        LightCoral = 0xfff08080,
        LightCyan = 0xffe0ffff,
        LightGoldenrodYellow = 0xfffafad2,
        LightGray = 0xffd3d3d3,
        LightGreen = 0xff90ee90,
        LightPink = 0xffffb6c1,
        LightSalmon = 0xffffa07a,
        LightSeaGreen = 0xff20b2aa,
        LightSkyBlue = 0xff87cefa,
        LightSlateGray = 0xff778899,
        LightSteelBlue = 0xffb0c4de,
        LightYellow = 0xffffffe0,
        Lime = 0xff00ff00,
        LimeGreen = 0xff32cd32,
        Linen = 0xfffaf0e6,
        Magenta = 0xffff00ff,
        Maroon = 0xff800000,
        MediumAquamarine = 0xff66cdaa,
        MediumBlue = 0xff0000cd,
        MediumOrchid = 0xffba55d3,
        MediumPurple = 0xff9370db,
        MediumSeaGreen = 0xff3cb371,
        MediumSlateBlue = 0xff7b68ee,
        MediumSpringGreen = 0xff00fa9a,
        MediumTurquoise = 0xff48d1cc,
        MediumVioletRed = 0xffc71585,
        MidnightBlue = 0xff191970,
        MintCream = 0xfff5fffa,
        MistyRose = 0xffffe4e1,
        Moccasin = 0xffffe4b5,
        NavajoWhite = 0xffffdead,
        Navy = 0xff000080,
        OldLace = 0xfffdf5e6,
        Olive = 0xff808000,
        OliveDrab = 0xff6b8e23,
        Orange = 0xffffa500,
        OrangeRed = 0xffff4500,
        Orchid = 0xffda70d6,
        PaleGoldenrod = 0xffeee8aa,
        PaleGreen = 0xff98fb98,
        PaleTurquoise = 0xffafeeee,
        PaleVioletRed = 0xffdb7093,
        PapayaWhip = 0xffffefd5,
        PeachPuff = 0xffffdab9,
        Peru = 0xffcd853f,
        Pink = 0xffffc0cb,
        Plum = 0xffdda0dd,
        PowderBlue = 0xffb0e0e6,
        Purple = 0xff800080,
        Red = 0xffff0000,
        RosyBrown = 0xffbc8f8f,
        RoyalBlue = 0xff4169e1,
        SaddleBrown = 0xff8b4513,
        Salmon = 0xfffa8072,
        SandyBrown = 0xfff4a460,
        SeaGreen = 0xff2e8b57,
        SeaShell = 0xfffff5ee,
        Sienna = 0xffa0522d,
        Silver = 0xffc0c0c0,
        SkyBlue = 0xff87ceeb,
        SlateBlue = 0xff6a5acd,
        SlateGray = 0xff708090,
        Snow = 0xfffffafa,
        SpringGreen = 0xff00ff7f,
        SteelBlue = 0xff4682b4,
        Tan = 0xffd2b48c,
        Teal = 0xff008080,
        Thistle = 0xffd8bfd8,
        Tomato = 0xffff6347,
        Transparent = 0xffffff,
        Turquoise = 0xff40e0d0,
        UnknownColor = 1,
        Violet = 0xffee82ee,
        Wheat = 0xfff5deb3,
        White = 0xffffffff,
        WhiteSmoke = 0xfff5f5f5,
        Yellow = 0xffffff00,
        YellowGreen = 0xff9acd32,
    };

    KnownColor() : EnumClass(GET_TYPE(KnownColor)) { };
    ~KnownColor() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class Key : public EnumClass
{
public:
    enum Enum
    {
        A = 0x2c,
        AbntC1 = 0x93,
        AbntC2 = 0x94,
        Add = 0x55,
        Apps = 0x48,
        Attn = 0xa3,
        B = 0x2d,
        Back = 2,
        BrowserBack = 0x7a,
        BrowserFavorites = 0x7f,
        BrowserForward = 0x7b,
        BrowserHome = 0x80,
        BrowserRefresh = 0x7c,
        BrowserSearch = 0x7e,
        BrowserStop = 0x7d,
        C = 0x2e,
        Cancel = 1,
        Capital = 8,
        CapsLock = 8,
        Clear = 5,
        CrSel = 0xa4,
        D = 0x2f,
        D0 = 0x22,
        D1 = 0x23,
        D2 = 0x24,
        D3 = 0x25,
        D4 = 0x26,
        D5 = 0x27,
        D6 = 40,
        D7 = 0x29,
        D8 = 0x2a,
        D9 = 0x2b,
        DbeAlphanumeric = 0x9d,
        DbeCodeInput = 0xa7,
        DbeDbcsChar = 0xa1,
        DbeDetermineString = 0xa9,
        DbeEnterDialogConversionMode = 170,
        DbeEnterImeConfigureMode = 0xa5,
        DbeEnterWordRegisterMode = 0xa4,
        DbeFlushString = 0xa6,
        DbeHiragana = 0x9f,
        DbeKatakana = 0x9e,
        DbeNoCodeInput = 0xa8,
        DbeNoRoman = 0xa3,
        DbeRoman = 0xa2,
        DbeSbcsChar = 160,
        Decimal = 0x58,
        Delete = 0x20,
        Divide = 0x59,
        Down = 0x1a,
        E = 0x30,
        End = 0x15,
        Enter = 6,
        EraseEof = 0xa6,
        Escape = 13,
        Execute = 0x1d,
        ExSel = 0xa5,
        F = 0x31,
        F1 = 90,
        F10 = 0x63,
        F11 = 100,
        F12 = 0x65,
        F13 = 0x66,
        F14 = 0x67,
        F15 = 0x68,
        F16 = 0x69,
        F17 = 0x6a,
        F18 = 0x6b,
        F19 = 0x6c,
        F2 = 0x5b,
        F20 = 0x6d,
        F21 = 110,
        F22 = 0x6f,
        F23 = 0x70,
        F24 = 0x71,
        F3 = 0x5c,
        F4 = 0x5d,
        F5 = 0x5e,
        F6 = 0x5f,
        F7 = 0x60,
        F8 = 0x61,
        F9 = 0x62,
        FinalMode = 11,
        G = 50,
        H = 0x33,
        HangulMode = 9,
        HanjaMode = 12,
        Help = 0x21,
        Home = 0x16,
        I = 0x34,
        ImeAccept = 0x10,
        ImeConvert = 14,
        ImeModeChange = 0x11,
        ImeNonConvert = 15,
        ImeProcessed = 0x9b,
        Insert = 0x1f,
        J = 0x35,
        JunjaMode = 10,
        K = 0x36,
        KanaMode = 9,
        KanjiMode = 12,
        L = 0x37,
        LaunchApplication1 = 0x8a,
        LaunchApplication2 = 0x8b,
        LaunchMail = 0x88,
        Left = 0x17,
        LeftAlt = 120,
        LeftCtrl = 0x76,
        LeftShift = 0x74,
        LineFeed = 4,
        LWin = 70,
        M = 0x38,
        MediaNextTrack = 0x84,
        MediaPlayPause = 0x87,
        MediaPreviousTrack = 0x85,
        MediaStop = 0x86,
        Multiply = 0x54,
        N = 0x39,
        Next = 20,
        NoName = 0xa9,
        None = 0,
        NumLock = 0x72,
        NumPad0 = 0x4a,
        NumPad1 = 0x4b,
        NumPad2 = 0x4c,
        NumPad3 = 0x4d,
        NumPad4 = 0x4e,
        NumPad5 = 0x4f,
        NumPad6 = 80,
        NumPad7 = 0x51,
        NumPad8 = 0x52,
        NumPad9 = 0x53,
        O = 0x3a,
        Oem1 = 140,
        Oem102 = 0x9a,
        Oem2 = 0x91,
        Oem3 = 0x92,
        Oem4 = 0x95,
        Oem5 = 150,
        Oem6 = 0x97,
        Oem7 = 0x98,
        Oem8 = 0x99,
        OemAttn = 0x9d,
        OemAuto = 160,
        OemBackslash = 0x9a,
        OemBackTab = 0xa2,
        OemClear = 0xab,
        OemCloseBrackets = 0x97,
        OemComma = 0x8e,
        OemCopy = 0x9f,
        OemEnlw = 0xa1,
        OemFinish = 0x9e,
        OemMinus = 0x8f,
        OemOpenBrackets = 0x95,
        OemPeriod = 0x90,
        OemPipe = 150,
        OemPlus = 0x8d,
        OemQuestion = 0x91,
        OemQuotes = 0x98,
        OemSemicolon = 140,
        OemTilde = 0x92,
        P = 0x3b,
        Pa1 = 170,
        PageDown = 20,
        PageUp = 0x13,
        Pause = 7,
        Play = 0xa7,
        Print = 0x1c,
        PrintScreen = 30,
        Prior = 0x13,
        Q = 60,
        R = 0x3d,
        Return = 6,
        Right = 0x19,
        RightAlt = 0x79,
        RightCtrl = 0x77,
        RightShift = 0x75,
        RWin = 0x47,
        S = 0x3e,
        Scroll = 0x73,
        Select = 0x1b,
        SelectMedia = 0x89,
        Separator = 0x56,
        Sleep = 0x49,
        Snapshot = 30,
        Space = 0x12,
        Subtract = 0x57,
        System = 0x9c,
        T = 0x3f,
        Tab = 3,
        U = 0x40,
        Up = 0x18,
        V = 0x41,
        VolumeDown = 130,
        VolumeMute = 0x81,
        VolumeUp = 0x83,
        W = 0x42,
        X = 0x43,
        Y = 0x44,
        Z = 0x45,
        Zoom = 0xa8,
    };

    Key() : EnumClass(GET_TYPE(Key)) { };
    ~Key() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class ModifierKeys : public EnumClass
{
public:
    enum Enum
    {
        None = 0x0,
        Alt = 0x1,
        Control = 0x2,
        Shift = 0x4,
    };

    ModifierKeys() : EnumClass(GET_TYPE(ModifierKeys)) { };
    ~ModifierKeys() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class FocusNavigationDirection : public EnumClass
{
public:
    enum Enum
    {
        Next,
        Previous,
        First,
        Last,
        Left,
        Right,
        Up,
        Down,
    };

    FocusNavigationDirection() : EnumClass(GET_TYPE(FocusNavigationDirection)) { };
    ~FocusNavigationDirection() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class KeyboardNavigationMode : public EnumClass
{
public:
    enum Enum
    {
        Continue,
        Cycle,
        None,
        Contained,
        Local,
    };

    KeyboardNavigationMode() : EnumClass(GET_TYPE(KeyboardNavigationMode)) { };
    ~KeyboardNavigationMode() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};

class KeyTimeType : public EnumClass
{
public:
    enum Enum
    {
        Percent,
        TimeSpan,
        Uniform,
    };

    KeyTimeType() : EnumClass(GET_TYPE(KeyTimeType)) { };
    ~KeyTimeType() { };
    private:
    static const EnumClass::EnumStringMapping cs_rgMapping[];    static const unsigned int cs_cMapping;    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }
    virtual unsigned int get_MappingCount() const { return cs_cMapping; }


};


};// Namespace AVUI
