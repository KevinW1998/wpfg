#pragma once

#include <AVUIItemStructList.h>
#include <AVUIVisual.h>
#include <AVUIDependencyProperty.h>
#include <AVUIDrawingContext.h>
#include <AVUIThickness.h>
#include <AVUISize.h>
#include <AVUIMouse.h>
#include <AVUIKeyboard.h>
#include <AVUILoadInitialize.h>

namespace AVUI {

class PresentationSource
{
public:
    static Visual* CriticalFromVisual(Visual* pVisual);
};

class LayoutTransformData : public Object
{
public:
    LayoutTransformData() { }

    Transform* get_Transform() { return m_pTransform; }

    void CreateTransformSnapshot(Transform* pSourceTransform);
    Size get_UntransformedDS() { return m_untransformedDS; }
    void set_UntransformedDS(const Size& size) { m_untransformedDS = size; }

    DECLARE_ELEMENT(LayoutTransformData, Object);

private:
    Size m_untransformedDS;
    TRefCountedPtr<Transform> m_pTransform;
};

class SizeBox : public Object
{
public:
    DECLARE_ELEMENT(SizeBox, Object);

    // Methods
    SizeBox(Size size) : m_width(size.get_Width()), m_height(size.get_Height()) { };
    SizeBox(float width, float height) : m_width(width), m_height(height) { }

    // Properties
    float get_Width() { return m_width; }
    void set_Width(float width) { m_width = width; }
    float get_Height() { return m_height; }
    void set_Height(float height) { m_height = height; }

private:
    float m_height;
    float m_width;

};


class LayoutManager;
class Style;
class AnimationTimeline;
class MouseEventArgs;
class MouseButtonEventArgs;
class MouseWheelEventArgs;
class ResourceDictionary;
class INameScope;
class CommandBinding;
class ExecutedRoutedEventArgs;
class RequestBringIntoViewEventArgs;
class TriggerCollection;
class BindingExpressionBase;
class BindingBase;
class QueryCursorEventArgs;
class Cursor;
class SizeChangedInfo;
class FrameworkTemplate;
class FocusWithinProperty;
class MouseCaptureWithinProperty;
class MouseOverProperty;
class ContextMenu;

class UIElement : public Visual, public ILoadInitialize
{
    friend class Mouse;
    friend class XamlParser;
    friend class LayoutManager;
    friend class CommandManager;
    friend class Keyboard;
    friend class EventTrigger;
    friend class ItemsControl;
    friend class Helper;
    friend class ReverseInheritProperty;
    friend class FocusWithinProperty;
    friend class MouseCaptureWithinProperty;
    friend class MouseOverProperty;
    friend class ElementObjectRef;
    friend class StyleHelper;
    friend class PopupRoot;
    friend class Visual;
    friend class FrameworkObject;
    friend class FrameworkElement;
public:

    UIElement();
    virtual ~UIElement();

    void Measure(const Size& constraintSize);
    void Arrange(const Rect& finalRect);

    bool CaptureMouse();
    void ReleaseMouseCapture();

    virtual void BeginInit();
    virtual void EndInit();

    const Size& get_DesiredSize() { static Size defaultSize; return get_VisibilityCache() == Visibility::Collapsed ? defaultSize : m_desiredSize; };
    const Size& get_RenderSize() { static Size defaultSize; return get_VisibilityCache() == Visibility::Collapsed ? defaultSize : m_renderSize; };
    const Size& get_PreviousAvailableSize() { return m_previousAvailableSize; };
    const Rect& get_PreviousArrangeRect() { return m_finalRect; };

    void ApplyAnimationClock(const DependencyProperty& property, AnimationClock *pClock, HandoffBehavior::Enum);
    void BeginAnimation(const DependencyProperty& property, AnimationTimeline* pTimeline, HandoffBehavior::Enum handoffBehavior = HandoffBehavior::SnapshotAndReplace);
    void EndAnimation(const DependencyProperty& property);

    const Object* get_AnimationBaseValue() const;

    void InvalidateMeasure();
    void InvalidateArrange();
    void InvalidateVisual();

    bool get_IsMeasureValid() const { return !get_MeasureDirty(); }
    bool get_IsArrangeValid() const { return !get_ArrangeDirty(); }

    bool get_IsInMeasureQueue() const { return ReadFlag(CoreFlags::InMeasureQueue); }
    void set_IsInMeasureQueue(bool inMeasureQueue) { WriteFlag(CoreFlags::InMeasureQueue, inMeasureQueue); }
    bool get_IsInArrangeQueue() const { return ReadFlag(CoreFlags::InArrangeQueue); }
    void set_IsInArrangeQueue(bool inArrangeQueue) { WriteFlag(CoreFlags::InArrangeQueue, inArrangeQueue); }

    Object* FindName(const String& Name);
    Object* FindName(const String& Name, DependencyObject** ppScopeOwner);

    void UpdateLayout();

    DECLARE_ELEMENT(UIElement, Visual);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Tag, Object, NULL, PropertyFlag::None);

    static const AVUI::DependencyProperty& get_ContextMenuProperty();
    ContextMenu* get_ContextMenu() { return (ContextMenu*)(Object*)GetValue(get_ContextMenuProperty()); }
    void set_ContextMenu(ContextMenu* pContextMenu) { return SetValue(get_ContextMenuProperty(), (Object*)pContextMenu); }


    static const AVUI::DependencyProperty& get_ToolTipProperty();
    Object* get_ToolTip() { return GetValue(get_ToolTipProperty()); }
    void set_ToolTip(Object* pToolTip) { return SetValue(get_ToolTipProperty(), pToolTip); }

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Cursor, Cursor, NULL, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnCursorChanged));

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(DataContext, Object, NULL, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnDataContextChanged, UIElement::CoerceDataContext));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(RenderTransform, Transform, NULL, PropertyFlag::AffectsArrange);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(LayoutTransform, Transform, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Style, Style, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, UIElement::OnStyleChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Resources, ResourceDictionary, NULL, PropertyFlag::AutoCreate);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Margin, Thickness, Thickness(0.0f), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Width, float, Float::get_NaN(), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MinWidth, float, 0.0f, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MaxWidth, float, Float::get_PositiveInfinity(), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Height,   float, Float::get_NaN(), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MinHeight, float, 0.0f, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MaxHeight, float, Float::get_PositiveInfinity(), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Opacity, float, 1.0f, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnOpacityChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Focusable, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsMouseOver, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsMouseCaptureWithin, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsMouseCaptured, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsFocused, bool, false, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnIsFocusedChanged));
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsKeyboardFocused, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsKeyboardFocusWithin, bool, false, PropertyFlag::None);

    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(ActualWidth, float, 0.0f, DependencyPropertyMetadata(PropertyFlag::ReadOnly, UIElement::GetActualWidth));
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(ActualHeight, float, 0.0f, DependencyPropertyMetadata(PropertyFlag::ReadOnly, UIElement::GetActualHeight));

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(IsVisible, bool, false, DependencyPropertyMetadata(PropertyFlag::ReadOnly, UIElement::GetIsVisible, UIElement::OnIsVisibleChanged));

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsHitTestVisible, bool, true, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnIsHitTestVisibleChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsEnabled, bool, true, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnIsEnabledChanged, UIElement::CoerceIsEnabled));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RenderTransformOrigin, Point, Point(), PropertyFlag::AffectsArrange);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ClipToBounds, bool, false, DependencyPropertyMetadata(PropertyFlag::AffectsArrange, UIElement::OnClipToBoundsChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ForceCursor, bool, false, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnCursorChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(SnapsToDevicePixels, bool, false, PropertyFlag::None);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Visibility, Visibility, Visibility::Visible, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, UIElement::OnVisibilityChanged));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(HorizontalAlignment, HorizontalAlignment, HorizontalAlignment::Stretch, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(VerticalAlignment, VerticalAlignment, VerticalAlignment::Stretch, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(FlowDirection, FlowDirection, FlowDirection::LeftToRight, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(OpacityMask, Brush, NULL, DependencyPropertyMetadata(PropertyFlag::None, UIElement::OnOpacityMaskChanged));
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Triggers, TriggerCollection, false);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(TemplatedParent, DependencyObject, false);

    ALIAS_ROUTED_EVENT(Mouse, GotMouseCapture, MouseEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, LostMouseCapture, MouseEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, PreviewMouseMove,  MouseEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, MouseMove,  MouseEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, PreviewMouseDown,  MouseButtonEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, MouseDown,  MouseButtonEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, PreviewMouseUp,    MouseButtonEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, MouseUp,    MouseButtonEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, MouseWheel, MouseEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, MouseEnter, MouseEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, MouseLeave, MouseEventArgs);
    ALIAS_ROUTED_EVENT(Mouse, QueryCursor, QueryCursorEventArgs);

    ALIAS_ROUTED_EVENT(Keyboard, KeyDown, KeyEventArgs);
    ALIAS_ROUTED_EVENT(Keyboard, KeyUp, KeyEventArgs);
    ALIAS_ROUTED_EVENT(Keyboard, KeyChar, KeyCharEventArgs);

    IMPLEMENT_ROUTED_EVENT(PreviewMouseLeftButtonDown, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(MouseLeftButtonDown, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(PreviewMouseLeftButtonUp, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(MouseLeftButtonUp, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(PreviewMouseRightButtonDown, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(MouseRightButtonDown, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(PreviewMouseRightButtonUp, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(MouseRightButtonUp, MouseButtonEventArgs, RoutingStrategy::Direct);


    IMPLEMENT_ROUTED_EVENT(SizeChanged, RoutedEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(RequestBringIntoView, RequestBringIntoViewEventArgs, RoutingStrategy::Bubble);

    IMPLEMENT_ROUTED_EVENT(Loaded, RoutedEventArgs, RoutingStrategy::Broadcast);
    IMPLEMENT_ROUTED_EVENT(Unloaded, RoutedEventArgs, RoutingStrategy::Broadcast);
    IMPLEMENT_EVENT(ResourcesChanged, EventArgs);

    IMPLEMENT_ROUTED_EVENT(GotFocus, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(LostFocus, RoutedEventArgs, RoutingStrategy::Bubble);

    IMPLEMENT_EVENT(IsKeyboardFocusWithinChanged, DependencyPropertyChangedEventArgs);
    IMPLEMENT_EVENT(IsMouseCaptureWithinChanged, DependencyPropertyChangedEventArgs);

    IMPLEMENT_EVENT(IsEnabledChanged, DependencyPropertyChangedEventArgs);
    IMPLEMENT_EVENT(IsHitTestVisibleChanged, DependencyPropertyChangedEventArgs);
    IMPLEMENT_EVENT(IsVisibleChanged, DependencyPropertyChangedEventArgs);

    void RaiseEvent(RoutedEventArgs* pArgs);

    Object* FindResource(const Object* pKey);
    Object* FindLocalResource(const Object* pKey);

    // Logical parent as UI element, is a weak reference to prevent circular dependency
    DependencyObject* get_Parent() { return m_pLogicalParent; }

    void AddLogicalChild(Object* pChild);
    void RemoveLogicalChild(Object* pChild);

    void BringIntoView();
    void BringIntoView(const Rect& rect);

    void AddHandler(const RoutedEvent* pRoutedEvent, Delegate* pEventHandler);
    void RemoveHandler(const RoutedEvent* pRoutedEvent, Delegate* pEventHandler);

    TriggerCollection* get_Triggers();

    virtual unsigned int get_VisualChildCount() { return m_pTemplateChild != NULL ? 1 : 0; }
    virtual Visual* get_VisualChild(unsigned int idx) { return m_pTemplateChild; }

    bool ApplyTemplate();
    virtual void OnPreApplyTemplate() { }
    virtual void OnApplyTemplate() { };

    virtual bool BlockReverseInheritance() { return false; }

    bool get_MeasureDirty() const { return ReadFlag(CoreFlags::MeasureDirty); }
    bool get_ArrangeDirty() const { return ReadFlag(CoreFlags::ArrangeDirty); }
    bool get_NeverMeasured() const { return ReadFlag(CoreFlags::NeverMeasured); }
    bool get_NeverArranged() const { return ReadFlag(CoreFlags::NeverArranged); }

    void set_BypassLayoutPolicies(bool bypassLayoutPolicies) { WriteFlag(CoreFlags::BypassLayoutPolicies, bypassLayoutPolicies); }
    bool get_BypassLayoutPolicies() const { return ReadFlag(CoreFlags::BypassLayoutPolicies); }

    void set_IsInitialized(bool isInitialized) { WriteFlag(CoreFlags::IsInitialized, isInitialized); }
    bool get_IsInitialized() const { return ReadFlag(CoreFlags::IsInitialized); }

    bool get_ClipToBoundsCache() const { return ReadFlag(CoreFlags::ClipToBounds); }
    void set_ClipToBoundsCache(bool clipToBounds) { WriteFlag(CoreFlags::ClipToBounds, clipToBounds); }

    bool get_IsVisible() const { return m_isVisibleCache; }

    DependencyObject* get_TemplatedParent() { return m_pTemplatedParent; }

    static DependencyObject* GetFrameworkParent(Object* pCurrent);

    TRefCountedPtr<BindingExpressionBase> SetBinding(const DependencyProperty& dp, BindingBase* pBinding);

    DependencyObject* GetTemplateChild(const String& childName);

    bool get_IsLoaded() { return true; } // TODO -- Review IsLoaded

protected:

    virtual void OnTemplatedParentSet() { };

    virtual void OnPropertyChanged(const DependencyPropertyChangedEventArgs&);
    virtual void OnRender(DrawingContext& drawingContext);
    virtual void OnChildDesiredSizeChanged(UIElement* pUIElement); 
    virtual void OnVisualParentChanged(DependencyObject* pOldParent);
    virtual void OnIsMouseCaptureWithinChanged(DependencyPropertyChangedEventArgs* pArgs) { } // NO IMPLEMENTATION IN BASE
    virtual void OnIsKeyboardFocusWithinChanged(DependencyPropertyChangedEventArgs* pArgs) { } // NO IMPLEMENTATION IN BASE

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    virtual void BuildRouteCore(EventRoute* pRoute, RoutedEventArgs* pArgs) { } // NO IMPLEMENTATION IN BASE
    virtual void AddToEventRouteCore(EventRoute* pRoute, RoutedEventArgs* pArgs);

    virtual void OnGotMouseCapture(MouseEventArgs* pArgs) { };
    virtual void OnLostMouseCapture(MouseEventArgs* pArgs) { };

    virtual void OnPreviewMouseMove(MouseEventArgs* pArgs) { };
    virtual void OnMouseMove(MouseEventArgs* pArgs) { };
    virtual void OnPreviewMouseDown(MouseButtonEventArgs* pArgs) { };
    virtual void OnMouseDown(MouseButtonEventArgs* pArgs) { };
    virtual void OnPreviewMouseUp(MouseButtonEventArgs* pArgs) { };
    virtual void OnMouseUp(MouseButtonEventArgs* pArgs) { };

    virtual void OnPreviewMouseLeftButtonDown(MouseButtonEventArgs* pArgs) { };
    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs) { };
    virtual void OnPreviewMouseLeftButtonUp(MouseButtonEventArgs* pArgs) { };
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs) { };
    virtual void OnPreviewMouseRightButtonDown(MouseButtonEventArgs* pArgs) { };
    virtual void OnMouseRightButtonDown(MouseButtonEventArgs* pArgs) { };
    virtual void OnPreviewMouseRightButtonUp(MouseButtonEventArgs* pArgs) { };
    virtual void OnMouseRightButtonUp(MouseButtonEventArgs* pArgs) { };
    virtual void OnRenderSizeChanged(SizeChangedInfo* pSizeInfo);


    virtual void OnMouseWheel(MouseWheelEventArgs* pArgs) { };
    virtual void OnMouseEnter(MouseEventArgs* pArgs) { };
    virtual void OnMouseLeave(MouseEventArgs* pArgs) { };
    virtual void OnQueryCursor(QueryCursorEventArgs* pArgs) { };

    virtual void OnKeyDown(KeyEventArgs* pArgs) { };
    virtual void OnKeyUp(KeyEventArgs* pArgs) { };
    virtual void OnKeyChar(KeyCharEventArgs* pArgs) { };

    virtual void OnGotFocus(RoutedEventArgs* pArgs);
    virtual void OnLostFocus(RoutedEventArgs* pArgs);

    virtual void OnInitialized(EventArgs* pEventArgs);


    virtual bool GetLayoutClip(const Size& layoutSlotSize, Rect* pRect);

    bool HasNonDefaultValue(const DependencyProperty& dp);


    virtual FrameworkTemplate* get_TemplateInternal() { return NULL; }
    bool get_HasTemplateGeneratedSubTree() const { return ReadFlag(CoreFlags::HasTemplateGeneratedSubTree); }

    LayoutManager* get_LayoutManager();

    DependencyObject* GetUIParentCore() { return m_pLogicalParent; }

    virtual TRefCountedPtr<HitTestResult> HitTestCore(PointHitTestParameters* pHitTestParameters);

private:

    void InputHitTest(const Point& pt, DependencyObject** ppEnabledHit, DependencyObject** ppRawHit);
    void InputHitTestFilterCallback(Object* pPotentialHitTestTarget, HitTestFilterArgs* pArgs);

    void OnLoaded(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    void OnUnloaded(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }

    void ChangeLogicalParent(DependencyObject* pNewParent);
    virtual void OnNewParent(DependencyObject* pNewParent);

    static FocusWithinProperty* get_FocusWithinProperty();
    static MouseCaptureWithinProperty* get_MouseCaptureWithinProperty();
    static MouseOverProperty* get_MouseOverProperty();

    TRefCountedPtr<Expression> GetExpressionCore(const DependencyProperty* pDP, DependencyPropertyMetadata* pMetadata);

    void set_TemplatedParent(DependencyObject* pDO) { m_pTemplatedParent = pDO; OnTemplatedParentSet(); }

    void RaiseIsMouseCaptureWithinChanged(DependencyPropertyChangedEventArgs* pArgs);
    void RaiseIsKeyboardFocusWithinChanged(DependencyPropertyChangedEventArgs* pArgs);

    static void SynchronizeForceInheritProperties(UIElement* pUIElement, DependencyObject* pParent);
    static void InvalidateForceInheritPropertyOnChildren(Visual* pVisual, const DependencyProperty& dependencyProperty);

    virtual DependencyObject* get_InheritanceParent() { return get_VisualParent(); }

    UIElement* get_TemplateChild() { return m_pTemplateChild; }
    void set_TemplateChild(UIElement* pUIElement) 
    { 
        if(m_pTemplateChild != pUIElement)
        {
            RemoveVisualChild(m_pTemplateChild);

            m_pTemplateChild = pUIElement;

            AddVisualChild(m_pTemplateChild);
        }
    }


    void TryFireInitialized();
    void PrivateInitialized();

    static void StaticConstructor();
    void PrivateSetFocused(bool isFocused);

    void GetRawValue(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pEntry);

    virtual void EvaluateBaseValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pNewEntry);
    virtual void EvaluateAnimatedValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pNewEntry);

    Size MeasureCore(const Size& availableSize);
    void ArrangeCore(const Rect& finalRect);

    void WriteFlag(unsigned int flag, bool value) { m_flags = (value ? m_flags | flag : m_flags & ~flag); }
    bool ReadFlag(unsigned int flag) const { return (m_flags & flag) != 0; }

    void set_MeasureDirty(bool measureDirty) { WriteFlag(CoreFlags::MeasureDirty, measureDirty); }
    void set_ArrangeDirty(bool arrangeDirty) { WriteFlag(CoreFlags::ArrangeDirty, arrangeDirty); }
    void set_MeasureInProgress(bool measureInProgress) { WriteFlag(CoreFlags::MeasureInProgress, measureInProgress); }
    bool get_MeasureInProgress() const { return ReadFlag(CoreFlags::MeasureInProgress); }
    void set_MeasureDuringArrange(bool measureDuringArrange) { WriteFlag(CoreFlags::MeasureDuringArrange, measureDuringArrange); }
    bool get_MeasureDuringArrange() const { return ReadFlag(CoreFlags::MeasureDuringArrange); }
    void set_NeverMeasured(bool neverMeasured) { WriteFlag(CoreFlags::NeverMeasured, neverMeasured); }

    void set_RenderingInvalidated(bool renderingInvalidated) { WriteFlag(CoreFlags::RenderingInvalidated, renderingInvalidated); }
    bool get_RenderingInvalidated() const { return ReadFlag(CoreFlags::RenderingInvalidated); }

    void set_HasTemplateGeneratedSubTree(bool hasTemplateGeneratedSubTree) { WriteFlag(CoreFlags::HasTemplateGeneratedSubTree, hasTemplateGeneratedSubTree); }

    void set_NeedsClipBounds(bool needsClipBounds) { WriteFlag(CoreFlags::NeedsClipBounds, needsClipBounds); }
    bool get_NeedsClipBounds() const { return ReadFlag(CoreFlags::NeedsClipBounds); }

    void set_NeverArranged(bool neverArranged) { WriteFlag(CoreFlags::NeverArranged, neverArranged); }
    void set_ArrangeInProgress(bool arrangeInProgress) { WriteFlag(CoreFlags::ArrangeInProgress, arrangeInProgress); }
    bool get_ArrangeInProgress() const { return ReadFlag(CoreFlags::ArrangeInProgress); }

    void set_HasStyleEverBeenFetched(bool hasStyleEverBeenFetched) { WriteFlag(CoreFlags::HasStyleEverBeenFetched, hasStyleEverBeenFetched); }
    bool get_HasStyleEverBeenFetched() const { return ReadFlag(CoreFlags::HasStyleEverBeenFetched); }

    void set_HasImplicitStyleFromResources(bool hasImplicitStyleFromResources) { WriteFlag(CoreFlags::HasImplicitStyleFromResources, hasImplicitStyleFromResources); }
    bool get_HasImplicitStyleFromResources() const { return ReadFlag(CoreFlags::HasImplicitStyleFromResources); }

    void set_IsStyleSetFromGenerator(bool isStyleSetFromGenerator) { WriteFlag(CoreFlags::IsStyleSetFromGenerator, isStyleSetFromGenerator); }
    bool get_IsStyleSetFromGenerator() const { return ReadFlag(CoreFlags::IsStyleSetFromGenerator); }

    void set_IsInitPending(bool isInitPending) { WriteFlag(CoreFlags::IsInitPending, isInitPending); }
    bool get_IsInitPending() const { return ReadFlag(CoreFlags::IsInitPending); }

    void set_IsRequestingExpression(bool isRequestingExpression) { WriteFlag(CoreFlags::IsRequestingExpression, isRequestingExpression); }
    bool get_IsRequestingExpression() const { return ReadFlag(CoreFlags::IsRequestingExpression); }

    bool IsRenderable();
    void ensureClip(const Size& layoutSlotSize);
    bool markForSizeChangedIfNeeded(const Size& oldSize, const Size& newSize);

    Vector ComputeAlignmentOffset(const Size& slotSize, const Size& clippedSize);
    void SetLayoutOffset(const Vector& offset, const Size& oldRenderSize);

    Point GetRenderTransformOrigin();

    Size FindMaximalAreaLocalSpaceRect(Transform* pLayoutTransform, const Size& marginSize);

    virtual Object* AdjustEventSource(RoutedEventArgs* pArgs) { return NULL; } // NO IMPLEMENTATION ON BASE
    static void BuildRouteHelper(DependencyObject* pDO, EventRoute* pRoute, RoutedEventArgs* pArgs);
    void AddToEventRoute(EventRoute* pRoute, RoutedEventArgs* pArgs);
    void BuildRoute(EventRoute* pRoute, RoutedEventArgs* pArgs);
    void AddListenersToRoute(EventRoute* pRoute, const RoutedEvent* pEvent);


    static void OnQueryCursorOverride(Object* pSender, QueryCursorEventArgs* pArgs);
    static void OnPreviewMouseMoveThunk(Object* pSender, MouseEventArgs* pArgs) { ((UIElement*) pSender)->OnPreviewMouseMove(pArgs); }
    static void OnMouseMoveThunk(Object* pSender, MouseEventArgs* pArgs) { ((UIElement*) pSender)->OnMouseMove(pArgs); }
    static void OnPreviewMouseDownThunk(Object* pSender, MouseButtonEventArgs* pArgs);
    static void OnMouseDownThunk(Object* pSender, MouseButtonEventArgs* pArgs);
    static void OnPreviewMouseUpThunk(Object* pSender, MouseButtonEventArgs* pArgs);
    static void OnMouseUpThunk(Object* pSender, MouseButtonEventArgs* pArgs);
    static void OnMouseWheelThunk(Object* pSender, MouseEventArgs* pArgs);
    static void OnMouseEnterThunk(Object* pSender, MouseEventArgs* pArgs) { ((UIElement*) pSender)->OnMouseEnter((MouseEventArgs*) pArgs); }
    static void OnMouseLeaveThunk(Object* pSender, MouseEventArgs* pArgs) { ((UIElement*) pSender)->OnMouseLeave((MouseEventArgs*) pArgs); }
    static void OnKeyDownThunk(Object* pSender, KeyEventArgs* pArgs);
    static void OnKeyUpThunk(Object* pSender, KeyEventArgs* pArgs) { ((UIElement*) pSender)->OnKeyUp((KeyEventArgs*) pArgs); }
    static void OnKeyCharThunk(Object* pSender, KeyCharEventArgs* pArgs) { ((UIElement*) pSender)->OnKeyChar((KeyCharEventArgs*) pArgs); }
    static void OnExecutedThunk(Object* pSender, ExecutedRoutedEventArgs* pArgs);
    static void OnPreviewExecutedThunk(Object* pSender, ExecutedRoutedEventArgs* pArgs);

    static void OnGotMouseCaptureThunk(Object* pSender, MouseEventArgs* pArgs) { ((UIElement*) pSender)->OnGotMouseCapture(pArgs); }
    static void OnLostMouseCaptureThunk(Object* pSender, MouseEventArgs* pArgs) { ((UIElement*) pSender)->OnLostMouseCapture(pArgs); }

    static void OnPreviewMouseLeftButtonDownThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnPreviewMouseLeftButtonDown(pArgs); }
    static void OnMouseLeftButtonDownThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnMouseLeftButtonDown(pArgs); }
    static void OnPreviewMouseLeftButtonUpThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnPreviewMouseLeftButtonUp(pArgs); }
    static void OnMouseLeftButtonUpThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnMouseLeftButtonUp(pArgs); }

    static void OnPreviewMouseRightButtonDownThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnPreviewMouseRightButtonDown(pArgs); }
    static void OnMouseRightButtonDownThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnMouseRightButtonDown(pArgs); }
    static void OnPreviewMouseRightButtonUpThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnPreviewMouseRightButtonUp(pArgs); }
    static void OnMouseRightButtonUpThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((UIElement*) pSender)->OnMouseRightButtonUp(pArgs); }



    static void OnStyleChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnClipToBoundsChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnOpacityChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnOpacityMaskChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e);
    static void OnIsFocusedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnVisibilityChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e);
    static void OnIsHitTestVisibleChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnIsEnabledChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static ObjectPtr CoerceIsEnabled(DependencyObject* pDO, Object* pValue);

    static void OnDataContextChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnCursorChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static ObjectPtr CoerceDataContext(DependencyObject* pDO, Object* pValue);

    static void OnIsVisibleChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    static void PropagateResumeLayout(Visual* pParent, Visual* pV);
    static void PropagateSuspendLayout(Visual* pV);


    Visibility::Enum get_VisibilityCache() { return m_visibilityCache; }
    void set_VisibilityCache(Visibility::Enum visibility) { m_visibilityCache = visibility; }
    void switchVisibilityIfNeeded(Visibility::Enum visibility);
    void ensureVisible();
    void ensureInvisible(bool isCollapsed);
    void signalDesiredSizeChange();

    void UpdateIsVisibleCache();


    void AddStyleHandlersToEventRoute(EventRoute* pRoute, RoutedEventArgs* pArgs);

    static INameScope* FindScope(UIElement* pUIElement, DependencyObject** ppScopeOwner);

    Style* FindImplicitStyleResource();
    bool GetValueFromTemplatedParent(const DependencyProperty& dp, EffectiveValueEntry* pEntry);
    ObjectPtr GetInheritableValue(const DependencyProperty& dp, DependencyPropertyMetadata* pMmetadata);
    void CrackMouseButtonEventAndReRaiseEvent(MouseButtonEventArgs* pArgs);
    void ReRaiseEventAs(RoutedEventArgs* pArgs, const RoutedEvent* pNewEvent);
    static const RoutedEvent* CrackMouseButtonEvent(MouseButtonEventArgs* pArgs);

    static ObjectPtr GetActualWidth(DependencyObject* pDO, BaseValueSource::Enum* pValueSource);
    static ObjectPtr GetActualHeight(DependencyObject* pDO, BaseValueSource::Enum* pValueSource);
    static ObjectPtr GetIsVisible(DependencyObject* pDO, BaseValueSource::Enum* pValueSource);

    static UncommonField<ItemStructList<TRefCountedPtr<CommandBinding> > > CommandBindingsField;  
    static UncommonField<SizeBox> UnclippedDesiredSizeField;  
    static UncommonField<LayoutTransformData> LayoutTransformDataField;  

    ItemStructList<TRefCountedPtr<CommandBinding> >* get_CommandBindings();

    UIElement* get_UIParent();

    Style* m_pStyleCache;

    TWeakPtr<DependencyObject> m_pLogicalParent;

    struct MinMax
    {
        float minWidth;
        float maxWidth;
        float minHeight;
        float maxHeight;
        MinMax(UIElement* pElement);
    };
 
    Size m_previousAvailableSize;
    Size m_desiredSize;

    Size m_renderSize;
    Rect m_finalRect;
    TRefCountedPtr<UIElement> m_pTemplateChild;

    Visibility::Enum m_visibilityCache;

    DependencyObject* m_pTemplatedParent;

    // Should this just be a bitfield struct?
    class CoreFlags
    {
    public:
        enum Enum
        {
            None =               0x0,
            NeverMeasured =      0x1,
            NeverArranged =      0x2,
            MeasureInProgress =  0x4,
            MeasureDuringArrange = 0x8,
            MeasureDirty =       0x10,

            ArrangeInProgress =  0x20,
            ArrangeDirty =       0x40,

            NeedsClipBounds =       0x80,
            InMeasureQueue  =       0x100,
            InArrangeQueue  =       0x200,
            HasStyleEverBeenFetched = 0x400,
            HasImplicitStyleFromResources = 0x800,
            IsStyleSetFromGenerator = 0x1000,
            BypassLayoutPolicies = 0x2000,
            RenderingInvalidated = 0x4000,
            ClipToBounds = 0x8000,
            HasTemplateGeneratedSubTree = 0x10000,
            HasTemplateChildBeenOnApplied = 0x20000,
            IsRequestingExpression = 0x40000,
            IsHitTestInvisible = 0x80000,
            IsInitPending      =0x100000,
            IsInitialized      =0x200000,

            IsKeyboardFocusWithinCache =0x400000,
            IsKeyboardFocusWithinChanged =0x800000,

            IsMouseOverCache =0x1000000,
            IsMouseOverChanged =0x2000000,

            IsMouseCaptureWithinCache =0x4000000,
            IsMouseCaptureWithinChanged =0x8000000

        };
    };

    unsigned int m_flags;
    bool m_isVisibleCache : 1;

};

typedef TRefCountedPtr<UIElement> UIElementPtr;
typedef TWeakPtr<UIElement> UIElementWeakPtr;


class SizeChangedInfo : public Object
{
public:
    DECLARE_ELEMENT(SizeChangedInfo, Object);

    // Methods
    SizeChangedInfo(UIElement* pElement, Size size, bool widthChanged, bool heightChanged) : m_pUIElement(pElement), m_previousSize(size), m_widthChanged(widthChanged), m_heightChanged(heightChanged) { };

    bool get_WidthChanged() { return m_widthChanged; }
    bool get_HeightChanged() { return m_heightChanged; }
    Size get_PreviousSize() { return m_previousSize; }
    Size get_NewSize() { return m_pUIElement->get_RenderSize(); }
    UIElement* get_Element() { return m_pUIElement; }

private:

    TRefCountedPtr<UIElement> m_pUIElement;
    bool m_heightChanged;
    bool m_widthChanged;
    Size m_previousSize;

};


}; // namespace AVUI
