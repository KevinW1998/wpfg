#include <AVUICommon.h>
#include <AVUIGenEnums.h>

namespace AVUI {

const EnumClass::EnumStringMapping MenuItemRole::cs_rgMapping[] = 
{
    TopLevelItem, L"TopLevelItem",
    TopLevelHeader, L"TopLevelHeader",
    SubmenuItem, L"SubmenuItem",
    SubmenuHeader, L"SubmenuHeader",
};

const unsigned int MenuItemRole::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(MenuItemRole)
namespace AVUI {
const EnumClass::EnumStringMapping ResizeMode::cs_rgMapping[] = 
{
    NoResize, L"NoResize",
    CanMinimize, L"CanMinimize",
    CanResize, L"CanResize",
    CanResizeWithGrip, L"CanResizeWithGrip",
};

const unsigned int ResizeMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ResizeMode)
namespace AVUI {
const EnumClass::EnumStringMapping WindowState::cs_rgMapping[] = 
{
    Normal, L"Normal",
    Minimized, L"Minimized",
    Maximized, L"Maximized",
};

const unsigned int WindowState::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(WindowState)
namespace AVUI {
const EnumClass::EnumStringMapping WindowStyle::cs_rgMapping[] = 
{
    None, L"None",
    SingleBorderWindow, L"SingleBorderWindow",
    ThreeDBorderWindow, L"ThreeDBorderWindow",
    ToolWindow, L"ToolWindow",
};

const unsigned int WindowStyle::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(WindowStyle)
namespace AVUI {
const EnumClass::EnumStringMapping MouseButtonState::cs_rgMapping[] = 
{
    Released, L"Released",
    Pressed, L"Pressed",
};

const unsigned int MouseButtonState::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(MouseButtonState)
namespace AVUI {
const EnumClass::EnumStringMapping PopupAnimation::cs_rgMapping[] = 
{
    None, L"None",
    Fade, L"Fade",
    Slide, L"Slide",
    Scroll, L"Scroll",
};

const unsigned int PopupAnimation::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(PopupAnimation)
namespace AVUI {
const EnumClass::EnumStringMapping PlacementMode::cs_rgMapping[] = 
{
    Absolute, L"Absolute",
    Relative, L"Relative",
    Bottom, L"Bottom",
    Center, L"Center",
    Right, L"Right",
    AbsolutePoint, L"AbsolutePoint",
    RelativePoint, L"RelativePoint",
    Mouse, L"Mouse",
    MousePoint, L"MousePoint",
    Left, L"Left",
    Top, L"Top",
    Custom, L"Custom",
};

const unsigned int PlacementMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(PlacementMode)
namespace AVUI {
const EnumClass::EnumStringMapping CursorType::cs_rgMapping[] = 
{
    None, L"None",
    No, L"No",
    Arrow, L"Arrow",
    AppStarting, L"AppStarting",
    Cross, L"Cross",
    Help, L"Help",
    IBeam, L"IBeam",
    SizeAll, L"SizeAll",
    SizeNESW, L"SizeNESW",
    SizeNS, L"SizeNS",
    SizeNWSE, L"SizeNWSE",
    SizeWE, L"SizeWE",
    UpArrow, L"UpArrow",
    Wait, L"Wait",
    Hand, L"Hand",
    Pen, L"Pen",
    ScrollNS, L"ScrollNS",
    ScrollWE, L"ScrollWE",
    ScrollAll, L"ScrollAll",
    ScrollN, L"ScrollN",
    ScrollS, L"ScrollS",
    ScrollW, L"ScrollW",
    ScrollE, L"ScrollE",
    ScrollNW, L"ScrollNW",
    ScrollNE, L"ScrollNE",
    ScrollSW, L"ScrollSW",
    ScrollSE, L"ScrollSE",
    ArrowCD, L"ArrowCD",
};

const unsigned int CursorType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(CursorType)
namespace AVUI {
const EnumClass::EnumStringMapping BindingStatus::cs_rgMapping[] = 
{
    Unattached, L"Unattached",
    Inactive, L"Inactive",
    Active, L"Active",
    Detached, L"Detached",
    AsyncRequestPending, L"AsyncRequestPending",
    PathError, L"PathError",
    UpdateTargetError, L"UpdateTargetError",
    UpdateSourceError, L"UpdateSourceError",
};

const unsigned int BindingStatus::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(BindingStatus)
namespace AVUI {
const EnumClass::EnumStringMapping BindingMode::cs_rgMapping[] = 
{
    TwoWay, L"TwoWay",
    OneWay, L"OneWay",
    OneTime, L"OneTime",
    OneWayToSource, L"OneWayToSource",
    Default, L"Default",
};

const unsigned int BindingMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(BindingMode)
namespace AVUI {
const EnumClass::EnumStringMapping UpdateSourceTrigger::cs_rgMapping[] = 
{
    Default, L"Default",
    PropertyChanged, L"PropertyChanged",
    LostFocus, L"LostFocus",
    Explicit, L"Explicit",
};

const unsigned int UpdateSourceTrigger::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(UpdateSourceTrigger)
namespace AVUI {
const EnumClass::EnumStringMapping FlowDirection::cs_rgMapping[] = 
{
    LeftToRight, L"LeftToRight",
    RightToLeft, L"RightToLeft",
};

const unsigned int FlowDirection::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(FlowDirection)
namespace AVUI {
const EnumClass::EnumStringMapping RelativeSourceMode::cs_rgMapping[] = 
{
    PreviousData, L"PreviousData",
    TemplatedParent, L"TemplatedParent",
    Self, L"Self",
    FindAncestor, L"FindAncestor",
};

const unsigned int RelativeSourceMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(RelativeSourceMode)
namespace AVUI {
const EnumClass::EnumStringMapping StretchDirection::cs_rgMapping[] = 
{
    Both, L"Both",
    UpOnly, L"UpOnly",
    DownOnly, L"DownOnly",
};

const unsigned int StretchDirection::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(StretchDirection)
namespace AVUI {
const EnumClass::EnumStringMapping BitmapCreateOptions::cs_rgMapping[] = 
{
    None, L"None",
    PreservePixelFormat, L"PreservePixelFormat",
    DelayCreation, L"DelayCreation",
    IgnoreColorProfile, L"IgnoreColorProfile",
    IgnoreImageCache, L"IgnoreImageCache",
};

const unsigned int BitmapCreateOptions::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(BitmapCreateOptions)
namespace AVUI {
const EnumClass::EnumStringMapping BitmapCacheOption::cs_rgMapping[] = 
{
    Default, L"Default",
    OnDemand, L"OnDemand",
    OnLoad, L"OnLoad",
    None, L"None",
};

const unsigned int BitmapCacheOption::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(BitmapCacheOption)
namespace AVUI {
const EnumClass::EnumStringMapping Dock::cs_rgMapping[] = 
{
    Left, L"Left",
    Top, L"Top",
    Right, L"Right",
    Bottom, L"Bottom",
};

const unsigned int Dock::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(Dock)
namespace AVUI {
const EnumClass::EnumStringMapping GridViewColumnHeaderRole::cs_rgMapping[] = 
{
    Normal, L"Normal",
    Floating, L"Floating",
    Padding, L"Padding",
};

const unsigned int GridViewColumnHeaderRole::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(GridViewColumnHeaderRole)
namespace AVUI {
const EnumClass::EnumStringMapping ColumnMeasureState::cs_rgMapping[] = 
{
    Init, L"Init",
    Headered, L"Headered",
    Data, L"Data",
    SpecificWidth, L"SpecificWidth",
};

const unsigned int ColumnMeasureState::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ColumnMeasureState)
namespace AVUI {
const EnumClass::EnumStringMapping SelectionMode::cs_rgMapping[] = 
{
    Single, L"Single",
    Multiple, L"Multiple",
    Extended, L"Extended",
};

const unsigned int SelectionMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(SelectionMode)
namespace AVUI {
const EnumClass::EnumStringMapping GeneratorDirection::cs_rgMapping[] = 
{
    Forward, L"Forward",
    Backward, L"Backward",
};

const unsigned int GeneratorDirection::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(GeneratorDirection)
namespace AVUI {
const EnumClass::EnumStringMapping GeneratorStatus::cs_rgMapping[] = 
{
    NotStarted, L"NotStarted",
    GeneratingContainers, L"GeneratingContainers",
    ContainersGenerated, L"ContainersGenerated",
    Error, L"Error",
};

const unsigned int GeneratorStatus::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(GeneratorStatus)
namespace AVUI {
const EnumClass::EnumStringMapping CollectionViewFlags::cs_rgMapping[] = 
{
    UpdatedOutsideDispatcher, L"UpdatedOutsideDispatcher",
    ShouldProcessCollectionChanged, L"ShouldProcessCollectionChanged",
    NeedsRefresh, L"NeedsRefresh",
    IsMultiThreadCollectionChangeAllowed, L"IsMultiThreadCollectionChangeAllowed",
    IsDynamic, L"IsDynamic",
    IsDataInGroupOrder, L"IsDataInGroupOrder",
    CachedIsEmpty, L"CachedIsEmpty",
    IsCurrentAfterLast, L"IsCurrentAfterLast",
    IsCurrentBeforeFirst, L"IsCurrentBeforeFirst",
};

const unsigned int CollectionViewFlags::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(CollectionViewFlags)
namespace AVUI {
const EnumClass::EnumStringMapping GridResizeDirection::cs_rgMapping[] = 
{
    Auto, L"Auto",
    Columns, L"Columns",
    Rows, L"Rows",
};

const unsigned int GridResizeDirection::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(GridResizeDirection)
namespace AVUI {
const EnumClass::EnumStringMapping GridResizeBehavior::cs_rgMapping[] = 
{
    BasedOnAlignment, L"BasedOnAlignment",
    CurrentAndNext, L"CurrentAndNext",
    PreviousAndCurrent, L"PreviousAndCurrent",
    PreviousAndNext, L"PreviousAndNext",
};

const unsigned int GridResizeBehavior::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(GridResizeBehavior)
namespace AVUI {
const EnumClass::EnumStringMapping NavigationStatus::cs_rgMapping[] = 
{
    Idle, L"Idle",
    Navigating, L"Navigating",
    NavigationFailed, L"NavigationFailed",
    Navigated, L"Navigated",
    Stopped, L"Stopped",
};

const unsigned int NavigationStatus::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(NavigationStatus)
namespace AVUI {
const EnumClass::EnumStringMapping NotifyCollectionChangedAction::cs_rgMapping[] = 
{
    Add, L"Add",
    Remove, L"Remove",
    Replace, L"Replace",
    Move, L"Move",
    Reset, L"Reset",
};

const unsigned int NotifyCollectionChangedAction::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(NotifyCollectionChangedAction)
namespace AVUI {
const EnumClass::EnumStringMapping TileMode::cs_rgMapping[] = 
{
    None, L"None",
    FlipX, L"FlipX",
    FlipY, L"FlipY",
    FlipXY, L"FlipXY",
    Tile, L"Tile",
};

const unsigned int TileMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(TileMode)
namespace AVUI {
const EnumClass::EnumStringMapping SweepDirection::cs_rgMapping[] = 
{
    Clockwise, L"Clockwise",
    Counterclockwise, L"Counterclockwise",
};

const unsigned int SweepDirection::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(SweepDirection)
namespace AVUI {
const EnumClass::EnumStringMapping AlignmentX::cs_rgMapping[] = 
{
    Left, L"Left",
    Center, L"Center",
    Right, L"Right",
};

const unsigned int AlignmentX::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(AlignmentX)
namespace AVUI {
const EnumClass::EnumStringMapping AlignmentY::cs_rgMapping[] = 
{
    Top, L"Top",
    Center, L"Center",
    Bottom, L"Bottom",
};

const unsigned int AlignmentY::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(AlignmentY)
namespace AVUI {
const EnumClass::EnumStringMapping BrushMappingMode::cs_rgMapping[] = 
{
    Absolute, L"Absolute",
    RelativeToBoundingBox, L"RelativeToBoundingBox",
};

const unsigned int BrushMappingMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(BrushMappingMode)
namespace AVUI {
const EnumClass::EnumStringMapping TextAlignment::cs_rgMapping[] = 
{
    Left, L"Left",
    Right, L"Right",
    Center, L"Center",
    Justify, L"Justify",
};

const unsigned int TextAlignment::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(TextAlignment)
namespace AVUI {
const EnumClass::EnumStringMapping GradientSpreadMethod::cs_rgMapping[] = 
{
    Pad, L"Pad",
    Reflect, L"Reflect",
    Repeat, L"Repeat",
};

const unsigned int GradientSpreadMethod::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(GradientSpreadMethod)
namespace AVUI {
const EnumClass::EnumStringMapping TextWrapping::cs_rgMapping[] = 
{
    NoWrap, L"NoWrap",
    Wrap, L"Wrap",
};

const unsigned int TextWrapping::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(TextWrapping)
namespace AVUI {
const EnumClass::EnumStringMapping TextTrimming::cs_rgMapping[] = 
{
    CharacterEllipsis, L"CharacterEllipsis",
    None, L"None",
};

const unsigned int TextTrimming::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(TextTrimming)
namespace AVUI {
const EnumClass::EnumStringMapping ScrollBarVisibility::cs_rgMapping[] = 
{
    Disabled, L"Disabled",
    Auto, L"Auto",
    Hidden, L"Hidden",
    Visible, L"Visible",
};

const unsigned int ScrollBarVisibility::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ScrollBarVisibility)
namespace AVUI {
const EnumClass::EnumStringMapping Visibility::cs_rgMapping[] = 
{
    Collapsed, L"Collapsed",
    Hidden, L"Hidden",
    Visible, L"Visible",
};

const unsigned int Visibility::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(Visibility)
namespace AVUI {
const EnumClass::EnumStringMapping GridUnitType::cs_rgMapping[] = 
{
    Auto, L"Auto",
    Pixel, L"Pixel",
    Star, L"Star",
};

const unsigned int GridUnitType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(GridUnitType)
namespace AVUI {
const EnumClass::EnumStringMapping MouseButton::cs_rgMapping[] = 
{
    Left, L"Left",
    Middle, L"Middle",
    Right, L"Right",
};

const unsigned int MouseButton::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(MouseButton)
namespace AVUI {
const EnumClass::EnumStringMapping LayoutTimeSizeType::cs_rgMapping[] = 
{
    None, L"None",
    Pixel, L"Pixel",
    Auto, L"Auto",
    Star, L"Star",
};

const unsigned int LayoutTimeSizeType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(LayoutTimeSizeType)
namespace AVUI {
const EnumClass::EnumStringMapping FillRule::cs_rgMapping[] = 
{
    EvenOdd, L"EvenOdd",
    NonZero, L"NonZero",
};

const unsigned int FillRule::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(FillRule)
namespace AVUI {
const EnumClass::EnumStringMapping RoutingStrategy::cs_rgMapping[] = 
{
    Tunnel, L"Tunnel",
    Bubble, L"Bubble",
    Direct, L"Direct",
    Broadcast, L"Broadcast",
};

const unsigned int RoutingStrategy::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(RoutingStrategy)
namespace AVUI {
const EnumClass::EnumStringMapping SourceValueType::cs_rgMapping[] = 
{
    Property, L"Property",
    Indexer, L"Indexer",
    Direct, L"Direct",
};

const unsigned int SourceValueType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(SourceValueType)
namespace AVUI {
const EnumClass::EnumStringMapping DrillIn::cs_rgMapping[] = 
{
    Always, L"Always",
    Never, L"Never",
    IfNeeded, L"IfNeeded",
};

const unsigned int DrillIn::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(DrillIn)
namespace AVUI {
const EnumClass::EnumStringMapping SlipBehavior::cs_rgMapping[] = 
{
    Grow, L"Grow",
    Slip, L"Slip",
};

const unsigned int SlipBehavior::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(SlipBehavior)
namespace AVUI {
const EnumClass::EnumStringMapping LogicalOp::cs_rgMapping[] = 
{
    Equals, L"Equals",
    NotEquals, L"NotEquals",
};

const unsigned int LogicalOp::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(LogicalOp)
namespace AVUI {
const EnumClass::EnumStringMapping AnimationType::cs_rgMapping[] = 
{
    Automatic, L"Automatic",
    From, L"From",
    To, L"To",
    By, L"By",
    FromTo, L"FromTo",
    FromBy, L"FromBy",
};

const unsigned int AnimationType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(AnimationType)
namespace AVUI {
const EnumClass::EnumStringMapping FillBehavior::cs_rgMapping[] = 
{
    HoldEnd, L"HoldEnd",
    Stop, L"Stop",
};

const unsigned int FillBehavior::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(FillBehavior)
namespace AVUI {
const EnumClass::EnumStringMapping TimeState::cs_rgMapping[] = 
{
    Stopped, L"Stopped",
    Paused, L"Paused",
    Running, L"Running",
};

const unsigned int TimeState::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(TimeState)
namespace AVUI {
const EnumClass::EnumStringMapping ClockState::cs_rgMapping[] = 
{
    Active, L"Active",
    Filling, L"Filling",
    Stopped, L"Stopped",
};

const unsigned int ClockState::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ClockState)
namespace AVUI {
const EnumClass::EnumStringMapping ClockFlags::cs_rgMapping[] = 
{
    None, L"None",
    IsTimeManager, L"IsTimeManager",
    IsBackwardsProgressingGlobal, L"IsBackwardsProgressingGlobal",
    IsInteractivelyPaused, L"IsInteractivelyPaused",
    IsInteractivelyStopped, L"IsInteractivelyStopped",
    PendingInteractivePause, L"PendingInteractivePause",
    PendingInteractiveResume, L"PendingInteractiveResume",
    PendingInteractiveStop, L"PendingInteractiveStop",
    PendingInteractiveRemove, L"PendingInteractiveRemove",
    CanSlip, L"CanSlip",
    CurrentStateInvalidatedEventRaised, L"CurrentStateInvalidatedEventRaised",
    CurrentTimeInvalidatedEventRaised, L"CurrentTimeInvalidatedEventRaised",
    CurrentGlobalSpeedInvalidatedEventRaised, L"CurrentGlobalSpeedInvalidatedEventRaised",
    CompletedEventRaised, L"CompletedEventRaised",
    RemoveRequestedEventRaised, L"RemoveRequestedEventRaised",
    IsInEventQueue, L"IsInEventQueue",
    NeedsTicksWhenActive, L"NeedsTicksWhenActive",
    NeedsPostfixTraversal, L"NeedsPostfixTraversal",
    PauseStateChangeDuringTick, L"PauseStateChangeDuringTick",
    RootBeginPending, L"RootBeginPending",
    HasControllableRoot, L"HasControllableRoot",
    AutoReverse, L"AutoReverse",
};

const unsigned int ClockFlags::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ClockFlags)
namespace AVUI {
const EnumClass::EnumStringMapping PropertyValueType::cs_rgMapping[] = 
{
    Set, L"Set",
    Trigger, L"Trigger",
    PropertyTriggerResource, L"PropertyTriggerResource",
    DataTrigger, L"DataTrigger",
    DataTriggerResource, L"DataTriggerResource",
    TemplateBinding, L"TemplateBinding",
    Resource, L"Resource",
};

const unsigned int PropertyValueType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(PropertyValueType)
namespace AVUI {
const EnumClass::EnumStringMapping ValueLookupType::cs_rgMapping[] = 
{
    Simple, L"Simple",
    Trigger, L"Trigger",
    PropertyTriggerResource, L"PropertyTriggerResource",
    DataTrigger, L"DataTrigger",
    DataTriggerResource, L"DataTriggerResource",
    TemplateBinding, L"TemplateBinding",
    Resource, L"Resource",
};

const unsigned int ValueLookupType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ValueLookupType)
namespace AVUI {
const EnumClass::EnumStringMapping RequestFlags::cs_rgMapping[] = 
{
    FullyResolved, L"FullyResolved",
    AnimationBaseValue, L"AnimationBaseValue",
    CoercionBaseValue, L"CoercionBaseValue",
    DeferredReferences, L"DeferredReferences",
    SkipDefault, L"SkipDefault",
    RawEntry, L"RawEntry",
};

const unsigned int RequestFlags::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(RequestFlags)
namespace AVUI {
const EnumClass::EnumStringMapping OperationType::cs_rgMapping[] = 
{
    Unknown, L"Unknown",
    AddChild, L"AddChild",
    RemoveChild, L"RemoveChild",
    Inherit, L"Inherit",
    ChangeMutableDefaultValue, L"ChangeMutableDefaultValue",
};

const unsigned int OperationType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(OperationType)
namespace AVUI {
const EnumClass::EnumStringMapping BaseValueSource::cs_rgMapping[] = 
{
    Unknown, L"Unknown",
    Default, L"Default",
    Inherited, L"Inherited",
    ThemeStyle, L"ThemeStyle",
    ThemeStyleTrigger, L"ThemeStyleTrigger",
    Style, L"Style",
    TemplateTrigger, L"TemplateTrigger",
    StyleTrigger, L"StyleTrigger",
    ImplicitReference, L"ImplicitReference",
    ParentTemplate, L"ParentTemplate",
    ParentTemplateTrigger, L"ParentTemplateTrigger",
    Local, L"Local",
};

const unsigned int BaseValueSource::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(BaseValueSource)
namespace AVUI {
const EnumClass::EnumStringMapping FullValueSource::cs_rgMapping[] = 
{
    IsExpression, L"IsExpression",
    IsAnimated, L"IsAnimated",
    IsCoerced, L"IsCoerced",
    IsDeferredReference, L"IsDeferredReference",
    HasExpressionMarker, L"HasExpressionMarker",
    ModifiersMask, L"ModifiersMask",
    ValueSourceMask, L"ValueSourceMask",
};

const unsigned int FullValueSource::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(FullValueSource)
namespace AVUI {
const EnumClass::EnumStringMapping UpdateResult::cs_rgMapping[] = 
{
    None, L"None",
    ValueChanged, L"ValueChanged",
    NotificationSent, L"NotificationSent",
};

const unsigned int UpdateResult::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(UpdateResult)
namespace AVUI {
const EnumClass::EnumStringMapping Stretch::cs_rgMapping[] = 
{
    None, L"None",
    Fill, L"Fill",
    Uniform, L"Uniform",
    UniformToFill, L"UniformToFill",
};

const unsigned int Stretch::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(Stretch)
namespace AVUI {
const EnumClass::EnumStringMapping PropertyFlag::cs_rgMapping[] = 
{
    None, L"None",
    AffectsMeasure, L"AffectsMeasure",
    AffectsArrange, L"AffectsArrange",
    AffectsRender, L"AffectsRender",
    AutoCreate, L"AutoCreate",
    ReadOnly, L"ReadOnly",
    IsContentProperty, L"IsContentProperty",
    IsAttachedProperty, L"IsAttachedProperty",
    IgnoreSubPropertyInvalidation, L"IgnoreSubPropertyInvalidation",
    Inherits, L"Inherits",
};

const unsigned int PropertyFlag::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(PropertyFlag)
namespace AVUI {
const EnumClass::EnumStringMapping HorizontalAlignment::cs_rgMapping[] = 
{
    Left, L"Left",
    Right, L"Right",
    Center, L"Center",
    Stretch, L"Stretch",
};

const unsigned int HorizontalAlignment::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(HorizontalAlignment)
namespace AVUI {
const EnumClass::EnumStringMapping VerticalAlignment::cs_rgMapping[] = 
{
    Top, L"Top",
    Bottom, L"Bottom",
    Center, L"Center",
    Stretch, L"Stretch",
};

const unsigned int VerticalAlignment::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(VerticalAlignment)
namespace AVUI {
const EnumClass::EnumStringMapping Orientation::cs_rgMapping[] = 
{
    Vertical, L"Vertical",
    Horizontal, L"Horizontal",
};

const unsigned int Orientation::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(Orientation)
namespace AVUI {
const EnumClass::EnumStringMapping HandoffBehavior::cs_rgMapping[] = 
{
    SnapshotAndReplace, L"SnapshotAndReplace",
    Compose, L"Compose",
};

const unsigned int HandoffBehavior::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(HandoffBehavior)
namespace AVUI {
const EnumClass::EnumStringMapping ClickMode::cs_rgMapping[] = 
{
    Release, L"Release",
    Press, L"Press",
    Hover, L"Hover",
};

const unsigned int ClickMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ClickMode)
namespace AVUI {
const EnumClass::EnumStringMapping KnownColor::cs_rgMapping[] = 
{
    AliceBlue, L"AliceBlue",
    AntiqueWhite, L"AntiqueWhite",
    Aqua, L"Aqua",
    Aquamarine, L"Aquamarine",
    Azure, L"Azure",
    Bisque, L"Bisque",
    Black, L"Black",
    BlanchedAlmond, L"BlanchedAlmond",
    Blue, L"Blue",
    BlueViolet, L"BlueViolet",
    Brown, L"Brown",
    BurlyWood, L"BurlyWood",
    CadetBlue, L"CadetBlue",
    Chartreuse, L"Chartreuse",
    Chocolate, L"Chocolate",
    Coral, L"Coral",
    CornflowerBlue, L"CornflowerBlue",
    Cornsilk, L"Cornsilk",
    Crimson, L"Crimson",
    Cyan, L"Cyan",
    DarkBlue, L"DarkBlue",
    DarkCyan, L"DarkCyan",
    DarkGoldenrod, L"DarkGoldenrod",
    DarkGray, L"DarkGray",
    DarkGreen, L"DarkGreen",
    DarkKhaki, L"DarkKhaki",
    DarkMagenta, L"DarkMagenta",
    DarkOliveGreen, L"DarkOliveGreen",
    DarkOrange, L"DarkOrange",
    DarkOrchid, L"DarkOrchid",
    DarkRed, L"DarkRed",
    DarkSalmon, L"DarkSalmon",
    DarkSeaGreen, L"DarkSeaGreen",
    DarkSlateBlue, L"DarkSlateBlue",
    DarkSlateGray, L"DarkSlateGray",
    DarkTurquoise, L"DarkTurquoise",
    DarkViolet, L"DarkViolet",
    DeepPink, L"DeepPink",
    DeepSkyBlue, L"DeepSkyBlue",
    DimGray, L"DimGray",
    DodgerBlue, L"DodgerBlue",
    Firebrick, L"Firebrick",
    FloralWhite, L"FloralWhite",
    ForestGreen, L"ForestGreen",
    Fuchsia, L"Fuchsia",
    Gainsboro, L"Gainsboro",
    GhostWhite, L"GhostWhite",
    Gold, L"Gold",
    Goldenrod, L"Goldenrod",
    Gray, L"Gray",
    Green, L"Green",
    GreenYellow, L"GreenYellow",
    Honeydew, L"Honeydew",
    HotPink, L"HotPink",
    IndianRed, L"IndianRed",
    Indigo, L"Indigo",
    Ivory, L"Ivory",
    Khaki, L"Khaki",
    Lavender, L"Lavender",
    LavenderBlush, L"LavenderBlush",
    LawnGreen, L"LawnGreen",
    LemonChiffon, L"LemonChiffon",
    LightBlue, L"LightBlue",
    LightCoral, L"LightCoral",
    LightCyan, L"LightCyan",
    LightGoldenrodYellow, L"LightGoldenrodYellow",
    LightGray, L"LightGray",
    LightGreen, L"LightGreen",
    LightPink, L"LightPink",
    LightSalmon, L"LightSalmon",
    LightSeaGreen, L"LightSeaGreen",
    LightSkyBlue, L"LightSkyBlue",
    LightSlateGray, L"LightSlateGray",
    LightSteelBlue, L"LightSteelBlue",
    LightYellow, L"LightYellow",
    Lime, L"Lime",
    LimeGreen, L"LimeGreen",
    Linen, L"Linen",
    Magenta, L"Magenta",
    Maroon, L"Maroon",
    MediumAquamarine, L"MediumAquamarine",
    MediumBlue, L"MediumBlue",
    MediumOrchid, L"MediumOrchid",
    MediumPurple, L"MediumPurple",
    MediumSeaGreen, L"MediumSeaGreen",
    MediumSlateBlue, L"MediumSlateBlue",
    MediumSpringGreen, L"MediumSpringGreen",
    MediumTurquoise, L"MediumTurquoise",
    MediumVioletRed, L"MediumVioletRed",
    MidnightBlue, L"MidnightBlue",
    MintCream, L"MintCream",
    MistyRose, L"MistyRose",
    Moccasin, L"Moccasin",
    NavajoWhite, L"NavajoWhite",
    Navy, L"Navy",
    OldLace, L"OldLace",
    Olive, L"Olive",
    OliveDrab, L"OliveDrab",
    Orange, L"Orange",
    OrangeRed, L"OrangeRed",
    Orchid, L"Orchid",
    PaleGoldenrod, L"PaleGoldenrod",
    PaleGreen, L"PaleGreen",
    PaleTurquoise, L"PaleTurquoise",
    PaleVioletRed, L"PaleVioletRed",
    PapayaWhip, L"PapayaWhip",
    PeachPuff, L"PeachPuff",
    Peru, L"Peru",
    Pink, L"Pink",
    Plum, L"Plum",
    PowderBlue, L"PowderBlue",
    Purple, L"Purple",
    Red, L"Red",
    RosyBrown, L"RosyBrown",
    RoyalBlue, L"RoyalBlue",
    SaddleBrown, L"SaddleBrown",
    Salmon, L"Salmon",
    SandyBrown, L"SandyBrown",
    SeaGreen, L"SeaGreen",
    SeaShell, L"SeaShell",
    Sienna, L"Sienna",
    Silver, L"Silver",
    SkyBlue, L"SkyBlue",
    SlateBlue, L"SlateBlue",
    SlateGray, L"SlateGray",
    Snow, L"Snow",
    SpringGreen, L"SpringGreen",
    SteelBlue, L"SteelBlue",
    Tan, L"Tan",
    Teal, L"Teal",
    Thistle, L"Thistle",
    Tomato, L"Tomato",
    Transparent, L"Transparent",
    Turquoise, L"Turquoise",
    UnknownColor, L"UnknownColor",
    Violet, L"Violet",
    Wheat, L"Wheat",
    White, L"White",
    WhiteSmoke, L"WhiteSmoke",
    Yellow, L"Yellow",
    YellowGreen, L"YellowGreen",
};

const unsigned int KnownColor::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(KnownColor)
namespace AVUI {
const EnumClass::EnumStringMapping Key::cs_rgMapping[] = 
{
    A, L"A",
    AbntC1, L"AbntC1",
    AbntC2, L"AbntC2",
    Add, L"Add",
    Apps, L"Apps",
    Attn, L"Attn",
    B, L"B",
    Back, L"Back",
    BrowserBack, L"BrowserBack",
    BrowserFavorites, L"BrowserFavorites",
    BrowserForward, L"BrowserForward",
    BrowserHome, L"BrowserHome",
    BrowserRefresh, L"BrowserRefresh",
    BrowserSearch, L"BrowserSearch",
    BrowserStop, L"BrowserStop",
    C, L"C",
    Cancel, L"Cancel",
    Capital, L"Capital",
    CapsLock, L"CapsLock",
    Clear, L"Clear",
    CrSel, L"CrSel",
    D, L"D",
    D0, L"D0",
    D1, L"D1",
    D2, L"D2",
    D3, L"D3",
    D4, L"D4",
    D5, L"D5",
    D6, L"D6",
    D7, L"D7",
    D8, L"D8",
    D9, L"D9",
    DbeAlphanumeric, L"DbeAlphanumeric",
    DbeCodeInput, L"DbeCodeInput",
    DbeDbcsChar, L"DbeDbcsChar",
    DbeDetermineString, L"DbeDetermineString",
    DbeEnterDialogConversionMode, L"DbeEnterDialogConversionMode",
    DbeEnterImeConfigureMode, L"DbeEnterImeConfigureMode",
    DbeEnterWordRegisterMode, L"DbeEnterWordRegisterMode",
    DbeFlushString, L"DbeFlushString",
    DbeHiragana, L"DbeHiragana",
    DbeKatakana, L"DbeKatakana",
    DbeNoCodeInput, L"DbeNoCodeInput",
    DbeNoRoman, L"DbeNoRoman",
    DbeRoman, L"DbeRoman",
    DbeSbcsChar, L"DbeSbcsChar",
    Decimal, L"Decimal",
    Delete, L"Delete",
    Divide, L"Divide",
    Down, L"Down",
    E, L"E",
    End, L"End",
    Enter, L"Enter",
    EraseEof, L"EraseEof",
    Escape, L"Escape",
    Execute, L"Execute",
    ExSel, L"ExSel",
    F, L"F",
    F1, L"F1",
    F10, L"F10",
    F11, L"F11",
    F12, L"F12",
    F13, L"F13",
    F14, L"F14",
    F15, L"F15",
    F16, L"F16",
    F17, L"F17",
    F18, L"F18",
    F19, L"F19",
    F2, L"F2",
    F20, L"F20",
    F21, L"F21",
    F22, L"F22",
    F23, L"F23",
    F24, L"F24",
    F3, L"F3",
    F4, L"F4",
    F5, L"F5",
    F6, L"F6",
    F7, L"F7",
    F8, L"F8",
    F9, L"F9",
    FinalMode, L"FinalMode",
    G, L"G",
    H, L"H",
    HangulMode, L"HangulMode",
    HanjaMode, L"HanjaMode",
    Help, L"Help",
    Home, L"Home",
    I, L"I",
    ImeAccept, L"ImeAccept",
    ImeConvert, L"ImeConvert",
    ImeModeChange, L"ImeModeChange",
    ImeNonConvert, L"ImeNonConvert",
    ImeProcessed, L"ImeProcessed",
    Insert, L"Insert",
    J, L"J",
    JunjaMode, L"JunjaMode",
    K, L"K",
    KanaMode, L"KanaMode",
    KanjiMode, L"KanjiMode",
    L, L"L",
    LaunchApplication1, L"LaunchApplication1",
    LaunchApplication2, L"LaunchApplication2",
    LaunchMail, L"LaunchMail",
    Left, L"Left",
    LeftAlt, L"LeftAlt",
    LeftCtrl, L"LeftCtrl",
    LeftShift, L"LeftShift",
    LineFeed, L"LineFeed",
    LWin, L"LWin",
    M, L"M",
    MediaNextTrack, L"MediaNextTrack",
    MediaPlayPause, L"MediaPlayPause",
    MediaPreviousTrack, L"MediaPreviousTrack",
    MediaStop, L"MediaStop",
    Multiply, L"Multiply",
    N, L"N",
    Next, L"Next",
    NoName, L"NoName",
    None, L"None",
    NumLock, L"NumLock",
    NumPad0, L"NumPad0",
    NumPad1, L"NumPad1",
    NumPad2, L"NumPad2",
    NumPad3, L"NumPad3",
    NumPad4, L"NumPad4",
    NumPad5, L"NumPad5",
    NumPad6, L"NumPad6",
    NumPad7, L"NumPad7",
    NumPad8, L"NumPad8",
    NumPad9, L"NumPad9",
    O, L"O",
    Oem1, L"Oem1",
    Oem102, L"Oem102",
    Oem2, L"Oem2",
    Oem3, L"Oem3",
    Oem4, L"Oem4",
    Oem5, L"Oem5",
    Oem6, L"Oem6",
    Oem7, L"Oem7",
    Oem8, L"Oem8",
    OemAttn, L"OemAttn",
    OemAuto, L"OemAuto",
    OemBackslash, L"OemBackslash",
    OemBackTab, L"OemBackTab",
    OemClear, L"OemClear",
    OemCloseBrackets, L"OemCloseBrackets",
    OemComma, L"OemComma",
    OemCopy, L"OemCopy",
    OemEnlw, L"OemEnlw",
    OemFinish, L"OemFinish",
    OemMinus, L"OemMinus",
    OemOpenBrackets, L"OemOpenBrackets",
    OemPeriod, L"OemPeriod",
    OemPipe, L"OemPipe",
    OemPlus, L"OemPlus",
    OemQuestion, L"OemQuestion",
    OemQuotes, L"OemQuotes",
    OemSemicolon, L"OemSemicolon",
    OemTilde, L"OemTilde",
    P, L"P",
    Pa1, L"Pa1",
    PageDown, L"PageDown",
    PageUp, L"PageUp",
    Pause, L"Pause",
    Play, L"Play",
    Print, L"Print",
    PrintScreen, L"PrintScreen",
    Prior, L"Prior",
    Q, L"Q",
    R, L"R",
    Return, L"Return",
    Right, L"Right",
    RightAlt, L"RightAlt",
    RightCtrl, L"RightCtrl",
    RightShift, L"RightShift",
    RWin, L"RWin",
    S, L"S",
    Scroll, L"Scroll",
    Select, L"Select",
    SelectMedia, L"SelectMedia",
    Separator, L"Separator",
    Sleep, L"Sleep",
    Snapshot, L"Snapshot",
    Space, L"Space",
    Subtract, L"Subtract",
    System, L"System",
    T, L"T",
    Tab, L"Tab",
    U, L"U",
    Up, L"Up",
    V, L"V",
    VolumeDown, L"VolumeDown",
    VolumeMute, L"VolumeMute",
    VolumeUp, L"VolumeUp",
    W, L"W",
    X, L"X",
    Y, L"Y",
    Z, L"Z",
    Zoom, L"Zoom",
};

const unsigned int Key::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(Key)
namespace AVUI {
const EnumClass::EnumStringMapping ModifierKeys::cs_rgMapping[] = 
{
    None, L"None",
    Alt, L"Alt",
    Control, L"Control",
    Shift, L"Shift",
};

const unsigned int ModifierKeys::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(ModifierKeys)
namespace AVUI {
const EnumClass::EnumStringMapping FocusNavigationDirection::cs_rgMapping[] = 
{
    Next, L"Next",
    Previous, L"Previous",
    First, L"First",
    Last, L"Last",
    Left, L"Left",
    Right, L"Right",
    Up, L"Up",
    Down, L"Down",
};

const unsigned int FocusNavigationDirection::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(FocusNavigationDirection)
namespace AVUI {
const EnumClass::EnumStringMapping KeyboardNavigationMode::cs_rgMapping[] = 
{
    Continue, L"Continue",
    Cycle, L"Cycle",
    None, L"None",
    Contained, L"Contained",
    Local, L"Local",
};

const unsigned int KeyboardNavigationMode::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(KeyboardNavigationMode)
namespace AVUI {
const EnumClass::EnumStringMapping KeyTimeType::cs_rgMapping[] = 
{
    Percent, L"Percent",
    TimeSpan, L"TimeSpan",
    Uniform, L"Uniform",
};

const unsigned int KeyTimeType::cs_cMapping = sizeof(cs_rgMapping) / sizeof(cs_rgMapping[0]);};
REGISTER_ENUM_TYPE(KeyTimeType)
namespace AVUI {

};
