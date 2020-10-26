#include <AVUICommon.h>
#include <AVUILayoutManager.h>
#include <AVUIUIElement.h>
#include <AVUIGenEnums.h>
#include <AVUITransform.h>
#include <AVUIThickness.h>
#include <AVUIVisualDrawingContext.h>
#include <AVUITranslateTransform.h>
#include <AVUITransformGroup.h>
#include <AVUITransformCollection.h>
#include <AVUIDispatcher.h>
#include <AVUIStyle.h>
#include <AVUIStyleHelper.h>
#include <AVUIAnimationStorage.h>
#include <AVUIClockGroup.h>
#include <AVUIStoryboard.h>
#include <AVUIResourceDictionary.h>
#include <AVUIControl.h>
#include <AVUIControlTemplate.h>
#include <AVUINameScope.h>
#include <AVUIApplication.h>
#include <AVUIInputManager.h>
#include <AVUICommandManager.h>
#include <AVUIRoutedCommand.h>
#include <AVUIRequestBringIntoViewEventArgs.h>
#include <AVUIEventTrigger.h>
#include <AVUIMatrix.h>
#include <AVUIMatrixTransform.h>
#include <AVUIHelper.h>
#include <AVUIDeferredReference.h>
#include <AVUIBindingOperations.h>
#include <AVUIReverseInheritProperties.h>
#include <AVUILength.h>
#include <AVUIToolTipService.h>
#include <AVUIContextMenuService.h>
#include <AVUIVisualTreeHelper.h>
#include <AVUIUIElementHelper.h>

REGISTER_ELEMENT_BEGIN(AVUI::UIElement)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_INTERFACE(ILoadInitialize)
    REGISTER_PROPERTY(Tag);
    REGISTER_PROPERTY(ContextMenu);
    REGISTER_PROPERTY(Style);
    REGISTER_PROPERTY(DataContext);
    REGISTER_PROPERTY(LayoutTransform);
    REGISTER_PROPERTY(RenderTransform);
    REGISTER_PROPERTY(RenderTransformOrigin);
    REGISTER_PROPERTY(Resources);
    REGISTER_PROPERTY(Margin);
    REGISTER_PROPERTY(Width);
    REGISTER_PROPERTY(MaxWidth);
    REGISTER_PROPERTY(MinWidth);
    REGISTER_PROPERTY(Height);
    REGISTER_PROPERTY(MaxHeight);
    REGISTER_PROPERTY(MinHeight);
    REGISTER_PROPERTY(IsMouseOver);
    REGISTER_PROPERTY(IsFocused);
    REGISTER_PROPERTY(IsHitTestVisible);
    REGISTER_PROPERTY(IsEnabled);
    REGISTER_PROPERTY(IsVisible);
    REGISTER_PROPERTY(ClipToBounds);
    REGISTER_PROPERTY(Visibility);
    REGISTER_PROPERTY(Opacity);
    REGISTER_PROPERTY(OpacityMask);
    REGISTER_PROPERTY(HorizontalAlignment);
    REGISTER_PROPERTY(VerticalAlignment);
    REGISTER_PROPERTY(Triggers);
    REGISTER_PROPERTY(TemplatedParent);
    REGISTER_PROPERTY(FlowDirection);
    REGISTER_PROPERTY(Cursor);
    REGISTER_PROPERTY(ForceCursor);
    REGISTER_PROPERTY(SnapsToDevicePixels);
    REGISTER_PROPERTY(Focusable);
    REGISTER_PROPERTY(ActualWidth);
    REGISTER_PROPERTY(ActualHeight);
    REGISTER_PROPERTY(ToolTip);


    REGISTER_ROUTED_EVENT(Loaded);
    REGISTER_ROUTED_EVENT(Unloaded);
    REGISTER_ROUTED_EVENT(SizeChanged);
    REGISTER_ROUTED_EVENT(PreviewMouseMove);
    REGISTER_ROUTED_EVENT(MouseMove);
    REGISTER_ROUTED_EVENT(PreviewMouseDown);
    REGISTER_ROUTED_EVENT(MouseDown);
    REGISTER_ROUTED_EVENT(PreviewMouseUp);
    REGISTER_ROUTED_EVENT(MouseUp);
    REGISTER_ROUTED_EVENT(MouseWheel);
    REGISTER_ROUTED_EVENT(MouseEnter);
    REGISTER_ROUTED_EVENT(MouseLeave);
    REGISTER_ROUTED_EVENT(PreviewMouseLeftButtonDown);
    REGISTER_ROUTED_EVENT(MouseLeftButtonDown);
    REGISTER_ROUTED_EVENT(PreviewMouseLeftButtonUp);
    REGISTER_ROUTED_EVENT(MouseLeftButtonUp);
    REGISTER_ROUTED_EVENT(PreviewMouseRightButtonDown);
    REGISTER_ROUTED_EVENT(MouseRightButtonDown);
    REGISTER_ROUTED_EVENT(PreviewMouseRightButtonUp);
    REGISTER_ROUTED_EVENT(MouseRightButtonUp);
    REGISTER_ROUTED_EVENT(QueryCursor);

    REGISTER_ROUTED_EVENT(KeyDown);
    REGISTER_ROUTED_EVENT(KeyUp);
    REGISTER_ROUTED_EVENT(KeyChar);
    REGISTER_ROUTED_EVENT(RequestBringIntoView);
    REGISTER_ROUTED_EVENT(GotFocus);
    REGISTER_ROUTED_EVENT(LostFocus);


    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor);

REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::RoutedEvent);
REGISTER_ELEMENT(AVUI::LayoutTransformData);
REGISTER_ELEMENT(AVUI::SizeBox);
REGISTER_ELEMENT(AVUI::RequestBringIntoViewEventArgs);
REGISTER_ELEMENT(AVUI::SizeChangedInfo);


namespace AVUI {


void UIElement::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_GotMouseCaptureEvent(), MouseEventHandler::Create(OnGotMouseCaptureThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_LostMouseCaptureEvent(), MouseEventHandler::Create(OnLostMouseCaptureThunk), false);

    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_PreviewMouseMoveEvent(), MouseEventHandler::Create(OnPreviewMouseMoveThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseMoveEvent(), MouseEventHandler::Create(OnMouseMoveThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_PreviewMouseDownEvent(), MouseButtonEventHandler::Create(OnPreviewMouseDownThunk), true);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseDownEvent(), MouseButtonEventHandler::Create(OnMouseDownThunk), true);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_PreviewMouseUpEvent(), MouseButtonEventHandler::Create(OnPreviewMouseUpThunk), true);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseUpEvent(), MouseButtonEventHandler::Create(OnMouseUpThunk), true);

    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_PreviewMouseLeftButtonDownEvent(), MouseButtonEventHandler::Create(OnPreviewMouseLeftButtonDownThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseLeftButtonDownEvent(), MouseButtonEventHandler::Create(OnMouseLeftButtonDownThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_PreviewMouseLeftButtonUpEvent(), MouseButtonEventHandler::Create(OnPreviewMouseLeftButtonUpThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseLeftButtonUpEvent(), MouseButtonEventHandler::Create(OnMouseLeftButtonUpThunk), false);

    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_PreviewMouseRightButtonDownEvent(), MouseButtonEventHandler::Create(OnPreviewMouseRightButtonDownThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseRightButtonDownEvent(), MouseButtonEventHandler::Create(OnMouseRightButtonDownThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_PreviewMouseRightButtonUpEvent(), MouseButtonEventHandler::Create(OnPreviewMouseRightButtonUpThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseRightButtonUpEvent(), MouseButtonEventHandler::Create(OnMouseRightButtonUpThunk), false);


    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_QueryCursorEvent(), QueryCursorEventHandler::Create(OnQueryCursorOverride), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseWheelEvent(), MouseEventHandler::Create(OnMouseWheelThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseEnterEvent(), MouseEventHandler::Create(OnMouseEnterThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_MouseLeaveEvent(), MouseEventHandler::Create(OnMouseLeaveThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_KeyDownEvent(), KeyEventHandler::Create(OnKeyDownThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_KeyUpEvent(), KeyEventHandler::Create(OnKeyUpThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_KeyCharEvent(), KeyCharEventHandler::Create(OnKeyCharThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), CommandManager::get_ExecutedEvent(), ExecutedRoutedEventHandler::Create(OnExecutedThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), CommandManager::get_PreviewExecutedEvent(), ExecutedRoutedEventHandler::Create(OnPreviewExecutedThunk), false);

    static LengthTypeConverter lengthConverter;
    get_WidthProperty().RegisterPropertyConverter(&lengthConverter);
    get_HeightProperty().RegisterPropertyConverter(&lengthConverter);
    get_MaxWidthProperty().RegisterPropertyConverter(&lengthConverter);
    get_MaxHeightProperty().RegisterPropertyConverter(&lengthConverter);



}


UncommonField<ItemStructList<TRefCountedPtr<CommandBinding> > > UIElement::CommandBindingsField;
UncommonField<SizeBox> UIElement::UnclippedDesiredSizeField;  
UncommonField<LayoutTransformData> UIElement::LayoutTransformDataField;  

UIElement::UIElement() : m_flags(0), m_pStyleCache(NULL), m_visibilityCache(Visibility::Visible), m_pLogicalParent(NULL), m_pTemplatedParent(NULL), m_isVisibleCache(false)
{
    set_NeverMeasured(true);
    set_NeverArranged(true);

    // This really doesn't work right -- vtbl pointers aren't valid yet, and get_type won't return most derived type
    set_ClipToBoundsCache(UnboxValue<bool>(get_ClipToBoundsProperty().GetMetadata(get_Type())->get_DefaultValue()));
}

UIElement::~UIElement()
{
    if(m_pTemplateChild != NULL)
    {
        FrameworkTemplate::ClearTemplateChildList(this);

        RemoveVisualChild(m_pTemplateChild);
        m_pTemplateChild = NULL;
    }
}

Size UIElement::MeasureOverride(const Size&)
{
    return Size(0.0f, 0.0f);
}
Size UIElement::ArrangeOverride(const Size&)
{
    return Size(0.0f, 0.0f);
}

void UIElement::UpdateLayout()
{
    Dispatcher::get_CurrentDispatcher()->get_LayoutManager()->Update();
}


void UIElement::ApplyAnimationClock(const DependencyProperty& dp, AnimationClock* pClock, HandoffBehavior::Enum handoffBehavior)
{
}
void UIElement::BeginAnimation(const DependencyProperty& dp, AnimationTimeline* pTimeline, HandoffBehavior::Enum handoffBehavior)
{
    AnimationStorage::BeginAnimation(this, dp, pTimeline, handoffBehavior);
}

void UIElement::EndAnimation(const DependencyProperty& dp)
{
    AnimationStorage::EndAnimation(this, dp);
}

const Object* UIElement::get_AnimationBaseValue() const
{
    return NULL;
}

void UIElement::OnRender(DrawingContext& drawingContext)
{

}


void UIElement::AddHandler(const RoutedEvent* pRoutedEvent, Delegate* pEventHandler)
{
    AddEventHandler(pRoutedEvent, pEventHandler);
}

void UIElement::RemoveHandler(const RoutedEvent* pRoutedEvent, Delegate* pEventHandler)
{
    RemoveEventHandler(pRoutedEvent, pEventHandler);
}

                
bool UIElement::ApplyTemplate() 
{ 
    OnPreApplyTemplate();

    bool flag = false;

    FrameworkTemplate* pTemplate = get_TemplateInternal();
    if(!get_HasTemplateGeneratedSubTree() && pTemplate != NULL)
    {
        m_pTemplateChild = object_cast<UIElement>(pTemplate->CreateInstance(this));
        set_HasTemplateGeneratedSubTree(true);

        StyleHelper::ExecuteOnApplyEnterExitActions(this, object_cast<ControlTemplate>(pTemplate));


        OnApplyTemplate();
        

        return true;
    }

    return false;
}

void UIElement::Measure(const Size& availableSize)
{
    TryFireInitialized();

    if (Float::IsNaN(availableSize.get_Width()) || Float::IsNaN(availableSize.get_Height()))
    {
        LibraryCriticalError();
    }

    bool neverMeasured = get_NeverMeasured();

    if(neverMeasured)
    {
        switchVisibilityIfNeeded(get_VisibilityCache());
    }

    if(get_VisibilityCache() == Visibility::Collapsed)
    {
        if(get_IsInMeasureQueue())
        {
            get_LayoutManager()->RemoveFromMeasureQueue(this);
        }
        if(!Float::AreClose(availableSize, m_previousAvailableSize))
        {
            set_MeasureDirty(true);
            m_previousAvailableSize = availableSize;
        }
    }
    else if ((!get_IsMeasureValid() || neverMeasured) || !Float::AreClose(availableSize, m_previousAvailableSize))
    {
        set_NeverMeasured(false);
        Size previousDesiredSize = get_DesiredSize();

        InvalidateArrange();
        InvalidateVisual();

        set_MeasureInProgress(true);

        Size desiredSize = MeasureCore(availableSize);

        set_MeasureInProgress(false);
        m_previousAvailableSize = availableSize;

        if (Float::IsInfinity(desiredSize.get_Width()) || Float::IsInfinity(desiredSize.get_Height()))
        {
            LibraryCriticalError();
        }
        if (Float::IsNaN(desiredSize.get_Width()) || Float::IsNaN(desiredSize.get_Height()))
        {
            LibraryCriticalError();
        }

        set_MeasureDirty(false);

        if(get_IsInMeasureQueue())
        {
            get_LayoutManager()->RemoveFromMeasureQueue(this);
        }

        m_desiredSize = desiredSize;
        if (!get_MeasureDuringArrange() && !Float::AreClose(desiredSize, previousDesiredSize))
        {
            UIElement* pParent = get_UIParent();

            if (pParent != NULL && !pParent->get_MeasureInProgress())
            {
                pParent->OnChildDesiredSizeChanged(this);
            }
        }
    }
}


Size UIElement::MeasureCore(const Size& availableSize)
{
    ApplyTemplate();

    if(get_BypassLayoutPolicies())
    {
        return MeasureOverride(availableSize);
    }

    Thickness thickness1 = get_Margin();
    float marginWidth = thickness1.get_Left() + thickness1.get_Right();
    float marginHeight = thickness1.get_Top() + thickness1.get_Bottom();
    Size transformSpaceBounds = Size(Float::Max(availableSize.get_Width() - marginWidth, 0), Float::Max(availableSize.get_Height() - marginHeight,  0));

    MinMax minMax(this);

    TRefCountedPtr<LayoutTransformData> pData = LayoutTransformDataField.GetValue(this);
    Transform* pSourceTransform = get_LayoutTransform();

    if (pSourceTransform)
    {
        if (pData == NULL)
        {
            pData = object_allocate<LayoutTransformData>();
            LayoutTransformDataField.SetValue(this, pData);
        }
        pData->CreateTransformSnapshot(pSourceTransform);
        pData->set_UntransformedDS(Size());
    }
    else if (pData != NULL)
    {
        pData = NULL;
        LayoutTransformDataField.ClearValue(this);
    }

    if (pData != NULL)
    {
        transformSpaceBounds = FindMaximalAreaLocalSpaceRect(pData->get_Transform(), transformSpaceBounds);
    }

    transformSpaceBounds.set_Width(Float::Max(minMax.minWidth, Float::Min(transformSpaceBounds.get_Width(), minMax.maxWidth)));
    transformSpaceBounds.set_Height(Float::Max(minMax.minHeight, Float::Min(transformSpaceBounds.get_Height(), minMax.maxHeight)));

    Size contentSize = MeasureOverride(transformSpaceBounds);
    contentSize = Size(Float::Max(contentSize.get_Width(), minMax.minWidth), Float::Max(contentSize.get_Height(), minMax.minHeight));

    Size originalContentSize = contentSize;

    if (pData != NULL)
    {
        pData->set_UntransformedDS(originalContentSize);
        Rect rect = Rect::Transform(Rect(0, 0, originalContentSize.get_Width(), originalContentSize.get_Height()), pData->get_Transform()->get_Value());
        originalContentSize.set_Width(rect.get_Width());
        originalContentSize.set_Height(rect.get_Height());
    }
    bool flag = false;

    if (contentSize.get_Width() > minMax.maxWidth)
    {
        contentSize.set_Width(minMax.maxWidth);
        flag = true;
    }
    if (contentSize.get_Height() > minMax.maxHeight)
    {
        contentSize.set_Height(minMax.maxHeight);
        flag = true;
    }

   if (pData != NULL)
    {
        Rect rect2 = Rect::Transform(Rect(0, 0, contentSize.get_Width(), contentSize.get_Height()), pData->get_Transform()->get_Value());
        contentSize.set_Width(rect2.get_Width());
        contentSize.set_Height(rect2.get_Height());
    }

    float finalWidth = contentSize.get_Width() + marginWidth;
    float finalHeight = contentSize.get_Height() + marginHeight;
    if (finalWidth > availableSize.get_Width())
    {
        finalWidth = availableSize.get_Width();
        flag = true;
    }
    if (finalHeight > availableSize.get_Height())
    {
        finalHeight = availableSize.get_Height();
        flag = true;
    }

    TRefCountedPtr<SizeBox> pBox = UnclippedDesiredSizeField.GetValue(this);
    if (flag || finalWidth < 0 || finalHeight < 0)
    {
        if (pBox == NULL)
        {
            pBox = object_allocate<SizeBox>(originalContentSize);
            UnclippedDesiredSizeField.SetValue(this, pBox);
        }
        else
        {
            pBox->set_Width(contentSize.get_Width());
            pBox->set_Height(contentSize.get_Height());
        }
    }
    else if (pBox != NULL)
    {
        UnclippedDesiredSizeField.ClearValue(this);
    }

    return Size(Float::Max(0, finalWidth), Float::Max(0, finalHeight));
}


void UIElement::Arrange(const Rect& finalRect)
{
    if (Float::IsInfinity(finalRect.get_Width()) || 
        Float::IsInfinity(finalRect.get_Height()) || 
        Float::IsNaN(finalRect.get_Width()) || 
        Float::IsNaN(finalRect.get_Height()))
    {
        LibraryCriticalError();
    }

    if(get_VisibilityCache() == Visibility::Collapsed)
    {
        if(get_IsInArrangeQueue())
        {
            get_LayoutManager()->RemoveFromArrangeQueue(this);
        }
        m_finalRect = finalRect;

        return;
    }

    if (get_MeasureDirty() || get_NeverMeasured())
    {
        set_MeasureDuringArrange(true);
        Measure(finalRect.get_Size());
        set_MeasureDuringArrange(false);
    }

    if (!get_IsArrangeValid() || get_NeverArranged() || !Float::AreClose(finalRect, m_finalRect))
    {
        bool wasNeverArranged = get_NeverArranged();
        bool wasArrangeValid = get_IsArrangeValid() && wasNeverArranged;
        set_NeverArranged(false);
        set_ArrangeInProgress(true);

        Size oldRenderSize = get_RenderSize();

        ArrangeCore(finalRect);
        ensureClip(finalRect.get_Size());
        markForSizeChangedIfNeeded(oldRenderSize, get_RenderSize());
        set_ArrangeInProgress(false);

        m_finalRect = finalRect;
        set_ArrangeDirty(false);

        if(get_IsInArrangeQueue())
        {
            get_LayoutManager()->RemoveFromArrangeQueue(this);
        }

        bool skipRender = Float::AreClose(get_RenderSize(), oldRenderSize) && 
                           (wasArrangeValid || (!wasNeverArranged && !get_RenderingInvalidated()));

        if (!skipRender && IsRenderable())
        {
            VisualDrawingContext ctx(this);
            OnRender(ctx);
            set_RenderingInvalidated(false);
        }
    }
}


void UIElement::ArrangeCore(const Rect& finalRect)
{
    if (get_BypassLayoutPolicies())
    {
        Size renderSize = get_RenderSize();
        Size size2 = ArrangeOverride(finalRect.get_Size());
        m_renderSize = size2;
        SetLayoutOffset(Vector(finalRect.get_X(), finalRect.get_Y()), renderSize);
    }
    else
    {
        Size untransformedDS;
        set_NeedsClipBounds(false);
        Size transformSpaceBounds = finalRect.get_Size();
        Thickness margin = get_Margin();
        float marginWidth = margin.get_Left() + margin.get_Right();
        float marginHeight = margin.get_Top() + margin.get_Bottom();
        transformSpaceBounds.set_Width(Float::Max(0, transformSpaceBounds.get_Width() - marginWidth));
        transformSpaceBounds.set_Height(Float::Max(0, transformSpaceBounds.get_Height() - marginHeight));

        SizeBox* pBox = UnclippedDesiredSizeField.GetValue(this);
        if(pBox == NULL)
        {
            untransformedDS = Size(get_DesiredSize().get_Width() - marginWidth, get_DesiredSize().get_Height() - marginHeight);
        }
        else
        {
            untransformedDS = Size(pBox->get_Width(), pBox->get_Height());
        }

        if (Float::LessThan(transformSpaceBounds.get_Width(), untransformedDS.get_Width()))
        {
            set_NeedsClipBounds(true);
            transformSpaceBounds.set_Width(untransformedDS.get_Width());
        }
        if (Float::LessThan(transformSpaceBounds.get_Height(), untransformedDS.get_Height()))
        {
            set_NeedsClipBounds(true);
            transformSpaceBounds.set_Height(untransformedDS.get_Height());
        }
        if (get_HorizontalAlignment() != HorizontalAlignment::Stretch)
        {
            transformSpaceBounds.set_Width(untransformedDS.get_Width());
        }
        if (get_VerticalAlignment() != VerticalAlignment::Stretch)
        {
            transformSpaceBounds.set_Height(untransformedDS.get_Height());
        }

        LayoutTransformData* pData = LayoutTransformDataField.GetValue(this);
        if(pData != NULL)
        {
            transformSpaceBounds = FindMaximalAreaLocalSpaceRect(pData->get_Transform(), transformSpaceBounds);
            untransformedDS = pData->get_UntransformedDS();

            if (!Float::IsZero(transformSpaceBounds.get_Width()) && !Float::IsZero(transformSpaceBounds.get_Height()) && (Float::LessThan(transformSpaceBounds.get_Width(), untransformedDS.get_Width()) || Float::LessThan(transformSpaceBounds.get_Height(), untransformedDS.get_Height())))
            {
                transformSpaceBounds = untransformedDS;
            }
            if (Float::LessThan(transformSpaceBounds.get_Width(), untransformedDS.get_Width()))
            {
                set_NeedsClipBounds(true);
                transformSpaceBounds.set_Width(untransformedDS.get_Width());
            }
            if (Float::LessThan(transformSpaceBounds.get_Height(), untransformedDS.get_Height()))
            {
                set_NeedsClipBounds(true);
                transformSpaceBounds.set_Height(untransformedDS.get_Height());
            }
        }


        MinMax minMax(this);
        float maxWidth = Float::Max(untransformedDS.get_Width(), minMax.maxWidth);
        if (Float::LessThan(maxWidth, transformSpaceBounds.get_Width()))
        {
            set_NeedsClipBounds(true);
            transformSpaceBounds.set_Width(maxWidth);
        }
        float maxHeight = Float::Max(untransformedDS.get_Height(), minMax.maxHeight);
        if (Float::LessThan(maxHeight, transformSpaceBounds.get_Height()))
        {
            set_NeedsClipBounds(true);
            transformSpaceBounds.set_Height(maxHeight);
        }

        Size oldRenderSize = m_renderSize;

        Size renderSize = ArrangeOverride(transformSpaceBounds);

        m_renderSize = renderSize;

        Size clippedSize(Float::Min(renderSize.get_Width(), minMax.maxWidth), Float::Min(renderSize.get_Height(), minMax.maxHeight));
        set_NeedsClipBounds(get_NeedsClipBounds() || Float::LessThan(clippedSize.get_Width(), renderSize.get_Width()) || 
                           Float::LessThan(clippedSize.get_Height(), renderSize.get_Height()));

        if (pData != NULL)
        {
            Rect rect = Rect::Transform(Rect(0, 0, clippedSize.get_Width(), clippedSize.get_Height()), pData->get_Transform()->get_Value());
            clippedSize.set_Width(rect.get_Width());
            clippedSize.set_Height(rect.get_Height());
        }


        Size slotSize(Float::Max(0, finalRect.get_Width() - marginWidth), Float::Max(0, finalRect.get_Height() - marginHeight));
        set_NeedsClipBounds(get_NeedsClipBounds() || Float::LessThan(slotSize.get_Width(), clippedSize.get_Width()) || 
                           Float::LessThan(slotSize.get_Height(), clippedSize.get_Height()));

        Vector offset = ComputeAlignmentOffset(slotSize, clippedSize);

        offset.set_X(offset.get_X() + finalRect.get_X() + margin.get_Left());
        offset.set_Y(offset.get_Y() + finalRect.get_Y() + margin.get_Top());

        SetLayoutOffset(offset, oldRenderSize);
    }
}

Vector UIElement::ComputeAlignmentOffset(const Size& clientSize, const Size& inkSize)
{
    Vector offset;

    HorizontalAlignment::Enum horizontalAlignment = get_HorizontalAlignment();
    VerticalAlignment::Enum verticalAlignment = get_VerticalAlignment();

    if ((horizontalAlignment == HorizontalAlignment::Stretch) && (inkSize.get_Width() > clientSize.get_Width()))
    {
        horizontalAlignment = HorizontalAlignment::Left;
    }
    if ((verticalAlignment == VerticalAlignment::Stretch) && (inkSize.get_Height() > clientSize.get_Height()))
    {
        verticalAlignment = VerticalAlignment::Top;
    }
    if ((horizontalAlignment == HorizontalAlignment::Center) || (horizontalAlignment == HorizontalAlignment::Stretch))
    {
        offset.set_X((clientSize.get_Width() - inkSize.get_Width()) * 0.5f);
    }
    else if (horizontalAlignment == HorizontalAlignment::Right)
    {
        offset.set_X(clientSize.get_Width() - inkSize.get_Width());
    }
    else
    {
        offset.set_X(0);
    }
    if ((verticalAlignment == VerticalAlignment::Center) || (verticalAlignment == VerticalAlignment::Stretch))
    {
        offset.set_Y((clientSize.get_Height() - inkSize.get_Height()) * 0.5f);
    }
    else if (verticalAlignment == VerticalAlignment::Bottom)
    {
        offset.set_Y(clientSize.get_Height() - inkSize.get_Height());
    }
    else
    {
        offset.set_Y(0);
    }
    return offset;
}


Size UIElement::FindMaximalAreaLocalSpaceRect(Transform* pLayoutTransform, const Size& transformSpaceBounds)
{
    float width = transformSpaceBounds.get_Width();
    float height = transformSpaceBounds.get_Height();
    if (Float::IsZero(width) || Float::IsZero(height))
    {
        return Size(0, 0);
    }
    bool flag = Float::IsInfinity(width);
    bool flag2 = Float::IsInfinity(height);
    if (flag && flag2)
    {
        return Size(Float::get_PositiveInfinity(), Float::get_PositiveInfinity());
    }
    if (flag)
    {
        width = height;
    }
    else if (flag2)
    {
        height = width;
    }
    Matrix matrix = pLayoutTransform->get_Value();
    if (!matrix.HasInverse())
    {
        return Size(0, 0);
    }
    float num3 = matrix.GetM11();
    float num4 = matrix.GetM12();
    float num5 = matrix.GetM21();
    float num6 = matrix.GetM22();
    float num7 = 0;
    float num8 = 0;
    if (Float::IsZero(num4) || Float::IsZero(num5))
    {
        float num9 = flag2 ? Float::get_PositiveInfinity() : Float::Abs(height / num6);
        float num10 = flag ? Float::get_PositiveInfinity() : Float::Abs(width / num3);
        if (Float::IsZero(num4))
        {
            if (Float::IsZero(num5))
            {
                num8 = num9;
                num7 = num10;
            }
            else
            {
                num8 = Float::Min(0.5f * Float::Abs(width / num5), num9);
                num7 = num10 - ((num5 * num8) / num3);
            }
        }
        else
        {
            num7 = Float::Min(0.5f * Float::Abs(height / num4), num10);
            num8 = num9 - ((num4 * num7) / num6);
        }
    }
    else if (Float::IsZero(num3) || Float::IsZero(num6))
    {
        float num11 = Float::Abs(height / num4);
        float num12 = Float::Abs(width / num5);
        if (Float::IsZero(num3))
        {
            if (Float::IsZero(num6))
            {
                num8 = num12;
                num7 = num11;
            }
            else
            {
                num8 = Float::Min(0.5f * Float::Abs(height / num6), num12);
                num7 = num11 - ((num6 * num8) / num4);
            }
        }
        else
        {
            num7 = Float::Min(0.5f * Float::Abs((width / num3)), num11);
            num8 = num12 - ((num3 * num7) / num5);
        }
    }
    else
    {
        float num13 = Float::Abs(width / num3);
        float num14 = Float::Abs(width / num5);
        float num15 = Float::Abs(height / num4);
        float num16 = Float::Abs(height / num6);
        num7 = Float::Min(num15, num13) * 0.5f;
        num8 = Float::Min(num14, num16) * 0.5f;
        if ((Float::GreaterThanOrClose(num13, num15) && Float::LessThanOrClose(num14, num16)) || (Float::LessThanOrClose(num13, num15) && Float::GreaterThanOrClose(num14, num16)))
        {
            Rect rect = Rect::Transform(Rect(0, 0, num7, num8), pLayoutTransform->get_Value());
            float d = Float::Min(width / rect.get_Width(), height / rect.get_Height());
            if (!Float::IsNaN(d) && !Float::IsInfinity(d))
            {
                num7 *= d;
                num8 *= d;
            }
        }
    }
    return Size(num7, num8);
}

void UIElement::SetLayoutOffset(const Vector& offset, const Size& oldRenderSize)
{
    LayoutTransformData* pData = LayoutTransformDataField.GetValue(this);
    Transform* pRenderTransform = get_RenderTransform();

    if(pRenderTransform != NULL && pRenderTransform->get_IsIdentity())
    {
        pRenderTransform = NULL;
    }

    if (pRenderTransform == NULL && pData == NULL)
    {
        Vector currentOffset = get_VisualOffset();
        if (!Float::AreClose(currentOffset.get_X(), offset.get_X()) || !Float::AreClose(currentOffset.get_Y(), offset.get_Y()))
        {
            set_VisualOffset(Vector(offset.get_X(), offset.get_Y()));
        }

        set_VisualTransform(NULL);
    }
    else
    {
        set_VisualOffset(Vector());

        TransformGroupPtr pTransformGroup(new TransformGroup(), false);
        /*
        pTransformGroup.CanBeInheritanceContext = false;
        pTransformGroup.Children.CanBeInheritanceContext = false;
        */

        if(pData != NULL)
        {
            pTransformGroup->get_Children()->Add(pData->get_Transform());
            MinMax minMax(this);

            Size renderSize = get_RenderSize();

            renderSize.set_Width(Float::Min(renderSize.get_Width(), minMax.maxWidth));
            renderSize.set_Height(Float::Min(renderSize.get_Height(), minMax.maxHeight));
            Rect rect = Rect::Transform(Rect(renderSize), pData->get_Transform()->get_Value());
            pTransformGroup->get_Children()->Add(object_allocate<TranslateTransform>(-rect.get_X(), -rect.get_Y()));
        }

        if(pRenderTransform != NULL)
        {
            Point transformOrigin = GetRenderTransformOrigin();
            bool hasOrigin = (transformOrigin.get_X() != 0) || (transformOrigin.get_Y() != 0);
            if (hasOrigin)
            {
                TranslateTransformPtr pTransform(new TranslateTransform(-transformOrigin.get_X(), -transformOrigin.get_Y()), false);

                pTransformGroup->get_Children()->Add(pTransform);
            }
            pTransformGroup->get_Children()->Add(pRenderTransform);
            if (hasOrigin)
            {
                TranslateTransformPtr pTransform(new TranslateTransform(transformOrigin.get_X(), transformOrigin.get_Y()), false);

                pTransformGroup->get_Children()->Add(pTransform);
            }
        }

        TranslateTransformPtr pOffsetTransform(new TranslateTransform(offset.get_X(), offset.get_Y()), false);

        pTransformGroup->get_Children()->Add(pOffsetTransform);
        set_VisualTransform(pTransformGroup);
    }
}

bool UIElement::IsRenderable()
{
    if ((!get_NeverMeasured() && !get_NeverArranged()) && get_IsMeasureValid())
    {
        return get_IsArrangeValid();
    }
    return false;
}

UIElement::MinMax::MinMax(UIElement* pElement)
{
    maxHeight = pElement->get_MaxHeight();
    minHeight = pElement->get_MinHeight();
    float height = pElement->get_Height();

    float fixedValue = Float::IsNaN(height) ? Float::get_PositiveInfinity() : height;
    maxHeight = Float::Max(Float::Min(fixedValue, maxHeight), minHeight);
    fixedValue = Float::IsNaN(height) ? 0 : height;
    minHeight = Float::Max(Float::Min(maxHeight, fixedValue), minHeight);


    maxWidth = pElement->get_MaxWidth();
    minWidth = pElement->get_MinWidth();
    float width = pElement->get_Width();

    fixedValue = Float::IsNaN(width) ? Float::get_PositiveInfinity() : width;
    maxWidth = Float::Max(Float::Min(fixedValue, maxWidth), minWidth);
    fixedValue = Float::IsNaN(width) ? 0 : width;
    minWidth = Float::Max(Float::Min(maxWidth, fixedValue), minWidth);

}


void UIElement::OnChildDesiredSizeChanged(UIElement* pUIElement)
{
    if (get_IsMeasureValid())
    {
        InvalidateMeasure();
    }
}

UIElement* UIElement::get_UIParent()
{
    UIElement* pParent = object_cast<UIElement>(get_VisualParent());
    if (pParent == NULL)
    {
        pParent = object_cast<UIElement>(get_Parent());
    }

    return object_cast<UIElement>(pParent);
}

void UIElement::OnPropertyChanged(const DependencyPropertyChangedEventArgs& e)
{

    if(e.DependencyPropertyChanged != UIElement::get_StyleProperty())
    {
        if(get_TemplatedParent() != NULL && get_TemplatedParent()->IsType(GET_TYPE(Control)))
        {
            ControlTemplate* pTemplate = ((Control*)get_TemplatedParent())->get_Template();

            if(pTemplate != NULL)
            {
                StyleHelper::OnTriggerSourcePropertyInvalidated(get_TemplatedParent(), pTemplate, e.DependencyPropertyChanged, e, pTemplate->TriggerSourceRecordFromChildIndex, pTemplate->PropertyTriggersWithActions, get_TemplateChildIndex());
            }
        }

        if(m_pStyleCache != NULL)
        {
            StyleHelper::OnTriggerSourcePropertyInvalidated(this, NULL, e.DependencyPropertyChanged, e, m_pStyleCache->TriggerSourceRecordFromChildIndex, m_pStyleCache->PropertyTriggersWithActions, 0);
        }
    }

    unsigned int flags = e.DependencyPropertyChanged.get_Flags();

    if(!e.IsSubPropertyChange || !(flags & PropertyFlag::IgnoreSubPropertyInvalidation))
    {
        if(flags & PropertyFlag::AffectsMeasure)
        {
            InvalidateMeasure();
        }

        if(flags & PropertyFlag::AffectsArrange)
        {
            InvalidateArrange();
        }

        if(flags & PropertyFlag::AffectsRender)
        {
            InvalidateVisual();
        }
    }
    else
    {
        InvalidateBbox();
    }

    DependencyObject::OnPropertyChanged(e);
}

void UIElement::InvalidateMeasure()
{
    if(!get_MeasureDirty() && !get_MeasureInProgress())
    {
        if(!get_NeverMeasured())
        {
            get_LayoutManager()->AddToMeasureQueue(this);
        }

        set_MeasureDirty(true);
    }
}

void UIElement::InvalidateArrange()
{
    if(!get_ArrangeDirty() && !get_ArrangeInProgress())
    {
        if(!get_NeverArranged())
        {
            get_LayoutManager()->AddToArrangeQueue(this);
        }

        set_ArrangeDirty(true);
    }
}

void UIElement::InvalidateVisual()
{
    InvalidateArrange();
    set_RenderingInvalidated(true);
}

void UIElement::ensureClip(const Size& layoutSlotSize)
{ 
    Rect layoutClip;

    if(GetLayoutClip(layoutSlotSize, &layoutClip))
    {
        set_VisualClip(layoutClip);
    }
    else
    {
        ClearVisualClip();
    }
}

bool UIElement::markForSizeChangedIfNeeded(const Size& oldSize, const Size& newSize)
{
    bool widthChanged = !Float::AreClose(oldSize.get_Width(), newSize.get_Width());
    bool heightChanged = !Float::AreClose(oldSize.get_Height(), newSize.get_Height());

    if(Float::AreClose(oldSize, newSize))
    {
        return false;
    }

    SizeChangedInfo sizeChangedInfo(this, oldSize, widthChanged, heightChanged);
 
    OnRenderSizeChanged(&sizeChangedInfo);

    return true;

}

void UIElement::OnRenderSizeChanged(SizeChangedInfo* pSizeInfo)
{
    RoutedEventArgs sizeChangedEventArgs(&get_SizeChangedEvent());

    if (pSizeInfo->get_WidthChanged())
    {
//        set_HasWidthEverChanged(true);

        DependencyPropertyChangedEventArgs eventArgs(get_ActualWidthProperty(), BoxValue(pSizeInfo->get_PreviousSize().get_Width()), BoxValue(pSizeInfo->get_NewSize().get_Width()));
        NotifyPropertyChange(eventArgs);
    }
    if (pSizeInfo->get_HeightChanged())
    {
//        set_HasHeightEverChanged(true);

        DependencyPropertyChangedEventArgs eventArgs(get_ActualHeightProperty(), BoxValue(pSizeInfo->get_PreviousSize().get_Height()), BoxValue(pSizeInfo->get_NewSize().get_Height()));
        NotifyPropertyChange(eventArgs);
    }


    RaiseEvent(&sizeChangedEventArgs);
}



Point UIElement::GetRenderTransformOrigin()
{
    Point pointOrigin = get_RenderTransformOrigin();
    Size renderSize = get_RenderSize();

    return Point(renderSize.get_Width() * pointOrigin.get_X(), renderSize.get_Height() * pointOrigin.get_Y());
}

void UIElement::EvaluateBaseValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pNewEntry)
{
    if (dp == UIElement::get_StyleProperty())
    {
        set_HasStyleEverBeenFetched(true);
        set_HasImplicitStyleFromResources(false);
        set_IsStyleSetFromGenerator(false);
    }

    GetRawValue(dp, pMetadata, pNewEntry);

    Storyboard::GetComplexPathValue(this, dp, pNewEntry, pMetadata);

}
void UIElement::EvaluateAnimatedValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pEntry)
{
    if (get_HasAnimatedProperties())
    {
        AnimationStorage* pStorage = AnimationStorage::GetStorage(this, dp);

        if (pStorage != NULL)
        {
            ObjectPtr pAnimationBaseValue = pEntry->GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
            if (pEntry->get_IsDeferredReference())
            {
                pAnimationBaseValue = ((DeferredReference*) (Object*) pAnimationBaseValue)->GetValue(pEntry->get_BaseValueSource());
                pEntry->SetAnimationBaseValue(pAnimationBaseValue);
                pEntry->set_IsDeferredReference(false);
            }
            ObjectPtr pAnimatedValue = AnimationStorage::GetCurrentPropertyValue(pStorage, this, dp, pMetadata, pAnimationBaseValue);
            pEntry->SetAnimatedValue(pAnimatedValue, pAnimationBaseValue);
        }
    }
}



bool UIElement::GetValueFromTemplatedParent(const DependencyProperty& dp, EffectiveValueEntry* pEntry)
{
    Control* pControl = (Control*) get_TemplatedParent();
    ControlTemplate* pTemplate = pControl->get_Template();

    if (pTemplate != NULL)
    {
        return StyleHelper::GetValueFromTemplatedParent(pControl, get_TemplateChildIndex(), this, dp, pTemplate->ChildRecordFromChildIndex, pEntry);
    }
    return false;
}


void UIElement::GetRawValue(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pEntry)
{
    if ((pEntry->get_BaseValueSource() != BaseValueSource::Local || pEntry->GetFlattenedEntry(RequestFlags::FullyResolved).get_Value() == DependencyProperty::get_UnsetValue()) && 
        (get_TemplateChildIndex() == -1 || !GetValueFromTemplatedParent(dp, pEntry)))
    {
        if (dp != get_StyleProperty())
        {
            if (StyleHelper::GetValueFromStyleOrTemplate(this, dp, pEntry))
            {
                return;
            }
        }
        else
        {
            ObjectPtr pStyle = FindImplicitStyleResource();
            if (pStyle != DependencyProperty::get_UnsetValue())
            {
                set_HasImplicitStyleFromResources(true);
                pEntry->set_BaseValueSource(BaseValueSource::ImplicitReference);
                pEntry->set_Value(pStyle);
                return;
            }
        }
        if (pMetadata != NULL && pMetadata->get_IsInherited())
        {
            ObjectPtr pInheritableValue = GetInheritableValue(dp, pMetadata);
            if (pInheritableValue != DependencyProperty::get_UnsetValue())
            {
                pEntry->set_BaseValueSource(BaseValueSource::Inherited);
                pEntry->set_Value(pInheritableValue);
                pEntry->set_IsDeferredReference(pInheritableValue->IsType(GET_TYPE(DeferredReference)));
            }
        }
    }
}

ObjectPtr UIElement::GetInheritableValue(const DependencyProperty& dp, DependencyPropertyMetadata* pMmetadata)
{
    UIElement* pParent = object_cast<UIElement>(GetFrameworkParent(this));

    if(pParent != NULL)
    {
        EntryIndex entryIndex = pParent->LookupEntry(dp.get_PropertyID());
        return pParent->GetValueEntry(entryIndex, dp, pMmetadata, (RequestFlags::Enum)(RequestFlags::SkipDefault | RequestFlags::DeferredReferences)).get_Value();
    }

    /*
    // TODO -- Yet more inheritance
    if (!TreeWalkHelper.SkipNext(this.InheritanceBehavior) || fmetadata.OverridesInheritanceBehavior)
    {
        FrameworkContentElement fceParent;
        FrameworkElement feParent;
        InheritanceBehavior inheritanceBehavior = InheritanceBehavior.Default;
        bool flag = GetFrameworkParent(this, out feParent, out fceParent);
        while (flag)
        {
            bool flag2;
            if (feParent != null)
            {
                flag2 = TreeWalkHelper.IsInheritanceNode(feParent, dp, out inheritanceBehavior);
            }
            else
            {
                flag2 = TreeWalkHelper.IsInheritanceNode(fceParent, dp, out inheritanceBehavior);
            }
            if (TreeWalkHelper.SkipNow(inheritanceBehavior))
            {
                break;
            }
            if (flag2)
            {
                if (EventTrace.IsEnabled(EventTrace.Flags.performance, EventTrace.Level.verbose))
                {
                    string text = string.Format(CultureInfo.InvariantCulture, "[{0}]{1}({2})", new object[] { base.GetType().Name, dp.Name, base.GetHashCode() });
                    EventTrace.EventProvider.TraceEvent(EventTrace.PROPERTYPARENTCHECKGUID, 0, base.GetHashCode(), text);
                }
                DependencyObject obj2 = feParent;
                if (obj2 == null)
                {
                    obj2 = fceParent;
                }
                EntryIndex entryIndex = obj2.LookupEntry(dp.GlobalIndex);
                return obj2.GetValueEntry(entryIndex, dp, fmetadata, RequestFlags.SkipDefault | RequestFlags.DeferredReferences).Value;
            }
            if (TreeWalkHelper.SkipNext(inheritanceBehavior))
            {
                break;
            }
            if (feParent != null)
            {
                flag = GetFrameworkParent(feParent, out feParent, out fceParent);
            }
            else
            {
                flag = GetFrameworkParent(fceParent, out feParent, out fceParent);
            }
        }
    }
    */
    return DependencyProperty::get_UnsetValue();
} 

void UIElement::OnStyleChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = object_cast<UIElement>(pObject);
    StyleHelper::UpdateStyleCache(pUIElement, (Style*) e.pOldValue, (Style*) e.pNewValue, &pUIElement->m_pStyleCache);
}

void UIElement::OnClipToBoundsChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = object_cast<UIElement>(pObject);
    pUIElement->WriteFlag(CoreFlags::ClipToBounds, UnboxValue<bool>(e.pNewValue));
}

void UIElement::OnOpacityChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = object_cast<UIElement>(pObject);
    if(pUIElement->get_VisibilityCache() == Visibility::Visible)
    {
        pUIElement->set_VisualOpacity(UnboxValue<float>(e.pNewValue));
    }
}

void UIElement::OnOpacityMaskChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = object_cast<UIElement>(pObject);
    pUIElement->set_VisualOpacityMask((Brush*) e.pNewValue);
}

void UIElement::OnIsFocusedChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = object_cast<UIElement>(pObject);
    if(UnboxValue<bool>(e.pNewValue) == true)
    {
        pUIElement->OnGotFocus(object_allocate<RoutedEventArgs>(&get_GotFocusEvent(), pUIElement));
    }
    else
    {
        pUIElement->OnLostFocus(object_allocate<RoutedEventArgs>(&get_LostFocusEvent(), pUIElement));
    }

}

void UIElement::OnIsEnabledChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = (UIElement*) pDO;
    InvalidateForceInheritPropertyOnChildren(pUIElement, e.DependencyPropertyChanged);

    // invalidate focus/etc?
}

void UIElement::InvalidateForceInheritPropertyOnChildren(Visual* pVisual, const DependencyProperty& dp)
{
    for (unsigned int i = 0; i < pVisual->get_VisualChildCount(); i++)
    {
        Visual* pVisualChild = pVisual->get_VisualChild(i);
        if (pVisualChild != NULL)
        {
            UIElement* pElement = object_cast<UIElement>(pVisualChild);
            if (pElement != NULL)
            {
                if (dp == get_IsVisibleProperty())
                {
                    pElement->UpdateIsVisibleCache();
                }
                else
                {
                    pElement->CoerceValue(dp);
                }
            }
            else
            {
                InvalidateForceInheritPropertyOnChildren(pVisualChild, dp);
            }
        }
    }
}


ObjectPtr UIElement::CoerceIsEnabled(DependencyObject* pDO, Object* pValue)
{
    bool currentValue = UnboxValue<bool>(pValue);
    UIElement* pUIElement = (UIElement*) pDO;

    if (!currentValue)
    {
        return pValue;
    }

    UIElement* pParent = object_cast<UIElement>(pUIElement->get_VisualParent());

    if(pParent != NULL && !pParent->get_IsEnabled())
    {
        return BoxValue(false);
    }

    return BoxValue(true);
}


void UIElement::OnCursorChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pElement = (UIElement*) pDO;
    if (pElement->get_IsMouseOver())
    {
        Mouse::UpdateCursor();
    }
}

void UIElement::OnDataContextChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = (UIElement*) pDO;
    InvalidateForceInheritPropertyOnChildren(pUIElement, e.DependencyPropertyChanged);
}

ObjectPtr UIElement::CoerceDataContext(DependencyObject* pDO, Object* pValue)
{
    UIElement* pUIElement = (UIElement*) pDO;

    if(pValue != NULL)
    {
        return pValue;
    }

    UIElement* pParent = object_cast<UIElement>(pUIElement->get_VisualParent());

    if(pParent != NULL)
    {
        return pParent->get_DataContext();
    }

    return NULL;
}


void UIElement::OnGotFocus(RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
    BringIntoView();
}

void UIElement::OnLostFocus(RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void UIElement::OnIsHitTestVisibleChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = object_cast<UIElement>(pObject);

    pUIElement->WriteFlag(CoreFlags::IsHitTestInvisible, !UnboxValue<bool>(e.pNewValue));
}


LayoutManager* UIElement::get_LayoutManager()
{
    return Dispatcher::get_CurrentDispatcher()->get_LayoutManager();
}

void UIElement::RaiseEvent(RoutedEventArgs* pArgs)
{
    EventRoute route(pArgs->get_RoutedEvent());

    pArgs->set_Source(this);

    BuildRoute(&route, pArgs);

    route.InvokeHandlers(this, pArgs);

    pArgs->set_Source(pArgs->get_OriginalSource());
}

void UIElement::BuildRoute(EventRoute* pRoute, RoutedEventArgs* pArgs)
{
    UIElement::BuildRouteHelper(this, pRoute, pArgs);
}

void UIElement::BuildRouteHelper(DependencyObject* pDO, EventRoute* pRoute, RoutedEventArgs* pArgs)
{
    if(pRoute == NULL || pArgs == NULL || pArgs->get_Source() == NULL || pArgs->get_RoutedEvent() != pRoute->get_RoutedEvent())
    {
        LibraryCriticalError();
    }


    if (pArgs->get_RoutedEvent()->get_RoutingStrategy() == RoutingStrategy::Direct)
    {
        UIElement* pUIElement = object_cast<UIElement>(pDO);
        pUIElement->AddToEventRoute(pRoute, pArgs);
    }
    else if(pArgs->get_RoutedEvent()->get_RoutingStrategy() == RoutingStrategy::Broadcast)
    {
        UIElement* pUIElement = object_cast<UIElement>(pDO);
        if(pUIElement != NULL)
        {
            pUIElement->BuildRouteCore(pRoute, pArgs);
            pUIElement->AddToEventRoute(pRoute, pArgs);
        }

        Visual* pVisual = object_cast<Visual>(pDO);

        if(pVisual != NULL)
        {
            for(unsigned int idx = 0; idx < pVisual->get_VisualChildCount(); idx++)
            {
                BuildRouteHelper(pVisual->get_VisualChild(idx), pRoute, pArgs);
            }
        }
    }
    else
    {
        int loopDetector = 0;
        while (pDO != NULL)
        {
            UIElement* pUIElement = object_cast<UIElement>(pDO);
            if (loopDetector++ > 0x100)
            {
                LibraryCriticalError();
            }
            Object* pObject = NULL;
            if (pUIElement != NULL)
            {
                pObject = pUIElement->AdjustEventSource(pArgs);
            }
            if (pObject != NULL)
            {
                pRoute->AddSource(pObject);
            }
            if (pUIElement != NULL)
            {
                pUIElement->BuildRouteCore(pRoute, pArgs);
                pUIElement->AddToEventRoute(pRoute, pArgs);
                pDO = pUIElement->get_UIParent();
            }
            if (pDO == pArgs->get_Source())
            {
                pRoute->AddSource(pDO);
            }
        }
    }
}
 
void UIElement::AddToEventRoute(EventRoute* pRoute, RoutedEventArgs* pArgs)
{
    AddListenersToRoute(pRoute, pArgs->get_RoutedEvent());

    AddInstanceHandlersToRoute(pRoute);

    AddToEventRouteCore(pRoute, pArgs);
}

void UIElement::AddToEventRouteCore(EventRoute* pRoute, RoutedEventArgs* pArgs)
{
    AddStyleHandlersToEventRoute(pRoute, pArgs);
}

void UIElement::AddStyleHandlersToEventRoute(EventRoute* pRoute, RoutedEventArgs* pArgs)
{
    if(m_pStyleCache != NULL)
    {
        EventHandlersStore* pStore = m_pStyleCache->get_EventHandlersStore();
        if(pStore != NULL)
        {
            pStore->AddHandlersToRoute(this, pRoute);
        }
    }
}


void UIElement::AddListenersToRoute(EventRoute* pRoute, const RoutedEvent* pEvent)
{
    EventManager::AddListenersToRoute(pRoute, this, get_Type(), pEvent);
}
 

Object* UIElement::FindResource(const Object* pKey)
{
    UIElement* pUIElement = this;
    Object* pResource = NULL;

    while(pUIElement != NULL)
    {
        pResource = pUIElement->FindLocalResource(pKey);
        if(pResource != DependencyProperty::get_UnsetValue())
        {
            return pResource;
        }

        UIElement* pNewElement = pUIElement->get_UIParent();

        if(pNewElement == NULL)
        {
            pNewElement = object_cast<UIElement>(pUIElement->get_TemplatedParent());
        }

        pUIElement = pNewElement;
    }

    return Application::get_Current()->FindResource(pKey);
}

Object* UIElement::FindLocalResource(const Object* pKey)
{
    ResourceDictionary* pResourceDictionary = get_Resources();
    if(pResourceDictionary != NULL)
    {
        return pResourceDictionary->GetValue(pKey);
    }
    return DependencyProperty::get_UnsetValue();
}

Style* UIElement::FindImplicitStyleResource()
{
    Style* pStyle = NULL;
    const Type* pTypeCur = &(get_Type());

    while(pTypeCur != NULL)
    {
        pStyle = object_cast<Style>(FindResource(&get_Type()));
        if(pStyle != NULL)
        {
            return pStyle;
        }

        pTypeCur = pTypeCur->get_BaseType();
    }

    return NULL;
}

void UIElement::BeginInit()
{
    set_IsInitPending(true);
}

void UIElement::EndInit()
{
    set_IsInitPending(false);

    TryFireInitialized();
}

void UIElement::TryFireInitialized()
{
    if(!get_IsInitPending() && !get_IsInitialized())
    {
        set_IsInitialized(true);
        PrivateInitialized();
        OnInitialized(NULL);
    }
}

void UIElement::PrivateInitialized()
{
    EventTrigger::ProcessTriggerCollection(this);

    // TODO -- Remove this, deal with inheritance properly
    SynchronizeInheritanceParent(get_Parent());

}

void UIElement::OnInitialized(EventArgs* pEventArgs)
{
    if(!get_HasStyleEverBeenFetched())
    {
        InvalidateProperty(get_StyleProperty());
        set_HasStyleEverBeenFetched(true);
    }
}

Object* UIElement::FindName(const String& Name)
{
    DependencyObject* pScopeOwner = NULL;

    return FindName(Name, &pScopeOwner);
}


Object* UIElement::FindName(const String& Name, DependencyObject** ppScopeOwner)
{
    INameScope* pScope = UIElement::FindScope(this, ppScopeOwner);
    if (pScope != NULL)
    {
          return pScope->FindName(Name);
    }
    return NULL;
}


 
INameScope* UIElement::FindScope(UIElement* pUIElement, DependencyObject** ppScopeOwner)
{
    if(pUIElement == NULL)
    {
        return NULL;
    }

    INameScope* pScope = NameScope::GetNameScope(pUIElement);
    if(pScope != NULL)
    {
        *ppScopeOwner = pUIElement;

        return pScope;
    }

    return UIElement::FindScope(object_cast<UIElement>(pUIElement->get_VisualParent()), ppScopeOwner);
}

bool UIElement::GetLayoutClip(const Size& layoutSlotSize, Rect* pRect)
{
    if (!get_NeedsClipBounds() && !get_ClipToBoundsCache())
    {
        return false;
    }

    MinMax minMax(this);
    Size renderSize = get_RenderSize();
    float num = Float::IsPositiveInfinity(minMax.maxWidth) ? renderSize.get_Width() : minMax.maxWidth;
    float num2 = Float::IsPositiveInfinity(minMax.maxHeight) ? renderSize.get_Height() : minMax.maxHeight;
    bool flag = get_ClipToBounds() || (Float::LessThan(num, renderSize.get_Width()) || Float::LessThan(num2, renderSize.get_Height()));
    renderSize.set_Width(Float::Min(renderSize.get_Width(), minMax.maxWidth));
    renderSize.set_Height(Float::Min(renderSize.get_Height(), minMax.maxHeight));

    TRefCountedPtr<LayoutTransformData> pData = LayoutTransformDataField.GetValue(this);
    Rect rect;
    if (pData != NULL)
    {
        rect = Rect::Transform(Rect(0.0f, 0.0f, renderSize.get_Width(), renderSize.get_Height()), pData->get_Transform()->get_Value());
        renderSize.set_Width(rect.get_Width());
        renderSize.set_Height(rect.get_Height());
    }
    Thickness margin = get_Margin();
    float num3 = margin.get_Left() + margin.get_Right();
    float num4 = margin.get_Top() + margin.get_Bottom();
    Size clientSize(Float::Max(0.0f, (layoutSlotSize.get_Width() - num3)), Float::Max(0.0f, (layoutSlotSize.get_Height() - num4)));
    bool flag2 = (get_ClipToBounds() || Float::LessThan(clientSize.get_Width(), renderSize.get_Width())) || Float::LessThan(clientSize.get_Height(), renderSize.get_Height());

    if (flag && !flag2)
    {
        *pRect = Rect(0.0f, 0.0f, num, num2);
        return true;
    }
    if (!flag2)
    {
        return false;
    }
    Vector vector = ComputeAlignmentOffset(clientSize, renderSize);
    if (pData != NULL)
    {
        /*
        // Deviation - No good layouttransform clip to bounds. Revisit when we support geometry more thoroughly
        RectangleGeometry geometry2 = new RectangleGeometry(new Rect(-vector.X + rect.X, -vector.Y + rect.Y, clientSize.Width, clientSize.Height));
        Matrix matrix = data.Transform.Value;
        if (matrix.HasInverse)
        {
            matrix.Invert();
            geometry2.Transform = new MatrixTransform(matrix);
        }
        if (flag)
        {
            RectangleGeometry geometry3 = new RectangleGeometry(new Rect(0.0, 0.0, num, num2));
            PathGeometry geometry4 = Geometry.Combine(geometry3, geometry2, GeometryCombineMode.Intersect, null);
            if (flowDirectionTransform != null)
            {
                geometry4.Transform = flowDirectionTransform;
            }
            return geometry4;
        }
        if (flowDirectionTransform != null)
        {
            if (geometry2.Transform != null)
            {
                geometry2.Transform = new MatrixTransform(geometry2.Transform.Value * flowDirectionTransform.Value);
                return geometry2;
            }
            geometry2.Transform = flowDirectionTransform;
        }
        return geometry2;
        */
    }
    Rect rect2(-vector.get_X(), -vector.get_Y(), clientSize.get_Width(), clientSize.get_Height());
    if (flag)
    {
        Rect rect3(0.0f, 0.0f, num, num2);
        Rect::Intersect(rect2, rect3, &rect2);
    }

    *pRect = rect2;

    return true; 
}


void UIElement::OnVisibilityChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pUIElement = (UIElement*) pObject;
    Visibility::Enum newVisibility = (Visibility::Enum) UnboxValue<int>(e.pNewValue);
    pUIElement->set_VisibilityCache(newVisibility);
    pUIElement->switchVisibilityIfNeeded(newVisibility);
    pUIElement->UpdateIsVisibleCache();
}

 

void UIElement::switchVisibilityIfNeeded(Visibility::Enum visibility)
{
    switch (visibility)
    {
        case Visibility::Visible:
            ensureVisible();
            return;
    
        case Visibility::Hidden:
            ensureInvisible(false);
            return;
    
        case Visibility::Collapsed:
            ensureInvisible(true);
            return;
    default:
        return;
    }
}

void UIElement::ensureVisible()
{
    if(get_IsOpacitySuppressed())
    {
        set_VisualOpacity(get_Opacity());
        if(get_IsCollapsed())
        {
            set_IsCollapsed(false);
            signalDesiredSizeChange();
            InvalidateVisual();
        }
        set_IsOpacitySuppressed(false);
    }
}

void UIElement::ensureInvisible(bool isCollapsed)
{
      if (!get_IsOpacitySuppressed())
      {
            set_VisualOpacity(0.0f);
            set_IsOpacitySuppressed(true);
      }
      if (!get_IsCollapsed() && isCollapsed)
      {
          set_IsCollapsed(true);
          signalDesiredSizeChange();
      }
      else if (get_IsCollapsed() && !isCollapsed)
      {
            set_IsCollapsed(false);
            signalDesiredSizeChange();
      }
}

void UIElement::signalDesiredSizeChange()
{
    UIElement* pParent = get_UIParent();

    if (pParent != NULL)
    {
        pParent->OnChildDesiredSizeChanged(this);
    }
}

void UIElement::OnExecutedThunk(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    CommandManager::OnExecuted(pSender, pArgs);
}

void UIElement::OnPreviewExecutedThunk(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
//    CommandManager::OnPreviewExecuted(pSender, pArgs);
}

void UIElement::OnQueryCursorOverride(Object* pSender, QueryCursorEventArgs* pArgs)
{
    UIElement* pElement = (UIElement*) pSender;
    Cursor* pCursor = pElement->get_Cursor();

    if(pCursor != NULL && (!pArgs->get_IsHandled() || pElement->get_ForceCursor()))
    {
        pArgs->set_Cursor(pCursor);
        pArgs->set_IsHandled(true);
    }

    if(!pArgs->get_IsHandled())
    {
        pElement->OnQueryCursor(pArgs);
    }
}

 


ItemStructList<TRefCountedPtr<CommandBinding> >* UIElement::get_CommandBindings()
{
    return CommandBindingsField.GetValue(this);
}

void UIElement::BringIntoView()
{
     RequestBringIntoViewEventArgs args(this);
     args.set_RoutedEvent(&(get_RequestBringIntoViewEvent()));
     RaiseEvent(&args);
}

void UIElement::BringIntoView(const Rect& rect)
{
    RequestBringIntoViewEventArgs args(this, rect);
    args.set_RoutedEvent(&(get_RequestBringIntoViewEvent()));

    RaiseEvent(&args);
}

void UIElement::OnVisualParentChanged(DependencyObject* pOldParent)
{
    Visual* pNewParent = get_VisualParent();

    if(pNewParent != NULL)
    {
         UIElement* pUIElementParent = object_cast<UIElement>(pNewParent);

         if(pUIElementParent != NULL)
         {
             SynchronizeForceInheritProperties(this, pUIElementParent);
         }
    }
    else
    {
        if(get_HasTemplateGeneratedSubTree())
        {
            FrameworkTemplate::ClearTemplateChildList(this);

            if(m_pTemplateChild != NULL)
            {
                RemoveVisualChild(m_pTemplateChild);
                m_pTemplateChild = NULL;
            }

            set_HasTemplateGeneratedSubTree(false);
        }

        ClearRenderDataClientGeometry();
    }

    TryFireInitialized();

    Visual::OnVisualParentChanged(pOldParent);
}

void UIElement::SynchronizeForceInheritProperties(UIElement* pUIElement, DependencyObject* pParent)
{
    if(!UnboxValue<bool>(pParent->GetValue(get_IsEnabledProperty())))
    {
        pUIElement->CoerceValue(get_IsEnabledProperty());
    }

    if(!UnboxValue<bool>(pParent->GetValue(get_IsHitTestVisibleProperty())))
    {
        pUIElement->CoerceValue(get_IsHitTestVisibleProperty());
    }

    if (UnboxValue<bool>(pParent->GetValue(get_IsVisibleProperty())))
    {
        pUIElement->UpdateIsVisibleCache();
    }


    // Deviation - force inherit data context
    if(pParent->GetValue(get_DataContextProperty()))
    {
        pUIElement->CoerceValue(get_DataContextProperty());
    }

    // TODO -- Coercion on IsVisible?

    if(pParent->GetValue(get_DataContextProperty()) != NULL)
    {
        pUIElement->CoerceValue(get_DataContextProperty());
    }

}
 
TRefCountedPtr<HitTestResult> UIElement::HitTestCore(PointHitTestParameters* pHitTestParameters)
{
    if(ReadFlag(CoreFlags::IsHitTestInvisible))
    {
        return NULL;
    }

    return Visual::HitTestCore(pHitTestParameters);
}

void UIElement::PrivateSetFocused(bool isFocused)
{
    if(isFocused)
    {
        SetValue(get_IsFocusedProperty(), BoxValue(isFocused));
    }
    else
    {
        ClearValue(get_IsFocusedProperty());
    }
}

TriggerCollection* UIElement::get_Triggers()
{
    TriggerCollection* pCollection = EventTrigger::TriggerCollectionField.GetValue(this);
    if(pCollection == NULL)
    {
        TRefCountedPtr<TriggerCollection> pNewCollection = object_allocate<TriggerCollection>();

        EventTrigger::TriggerCollectionField.SetValue(this, pNewCollection);

        pCollection = pNewCollection;
    }

    return pCollection;
}

bool UIElement::HasNonDefaultValue(const DependencyProperty& dp)
{
    return !Helper::HasDefaultValue(dp, this, this);
}

void UIElement::RaiseIsMouseCaptureWithinChanged(DependencyPropertyChangedEventArgs* pArgs)
{
    OnIsMouseCaptureWithinChanged(pArgs);
    invoke_IsMouseCaptureWithinChanged(this, pArgs);
}

void UIElement::RaiseIsKeyboardFocusWithinChanged(DependencyPropertyChangedEventArgs* pArgs)
{
    OnIsKeyboardFocusWithinChanged(pArgs);
    invoke_IsKeyboardFocusWithinChanged(this, pArgs);
}

DependencyObject* UIElement::GetFrameworkParent(Object* pCurrent)
{
    UIElement* pElement = object_cast<UIElement>(pCurrent);
    if(pElement != NULL)
    {
        return pElement->get_Parent();
    }

    return pElement;
}

TRefCountedPtr<BindingExpressionBase> UIElement::SetBinding(const DependencyProperty& dp, BindingBase* pBinding)
{
    return BindingOperations::SetBinding(this, &dp, pBinding);
}


TRefCountedPtr<Expression> UIElement::GetExpressionCore(const DependencyProperty* pDP, DependencyPropertyMetadata* pMetadata)
{
    set_IsRequestingExpression(true);
    EffectiveValueEntry newEntry(*pDP);
    newEntry.set_Value(DependencyProperty::get_UnsetValue());

    EvaluateBaseValueCore(*pDP, pMetadata, &newEntry);

    set_IsRequestingExpression(false);

    return object_cast<Expression>(newEntry.get_Value());
}


void UIElement::OnPreviewMouseDownThunk(Object* pSender, MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        ((UIElement*) pSender)->OnPreviewMouseDown(pArgs);
    }
    ((UIElement*)pSender)->CrackMouseButtonEventAndReRaiseEvent(pArgs);
}


void UIElement::OnMouseDownThunk(Object* pSender, MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        CommandManager::TranslateInput((DependencyObject*)pSender, pArgs);
    }
    if (!pArgs->get_IsHandled())
    {
        ((UIElement*) pSender)->OnMouseDown(pArgs);
    }
    ((UIElement*)pSender)->CrackMouseButtonEventAndReRaiseEvent(pArgs);
}

void UIElement::OnPreviewMouseUpThunk(Object* pSender, MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        ((UIElement*) pSender)->OnPreviewMouseUp(pArgs);
    }
    ((UIElement*)pSender)->CrackMouseButtonEventAndReRaiseEvent(pArgs);
}

void UIElement::OnMouseUpThunk(Object* pSender, MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        ((UIElement*) pSender)->OnMouseUp(pArgs);
    }
    ((UIElement*)pSender)->CrackMouseButtonEventAndReRaiseEvent(pArgs);
}


void UIElement::CrackMouseButtonEventAndReRaiseEvent(MouseButtonEventArgs* pArgs)
{
    const RoutedEvent* pNewEvent = CrackMouseButtonEvent(pArgs);
    if (pNewEvent != NULL)
    {
        ReRaiseEventAs(pArgs, pNewEvent);
    }
} 

const RoutedEvent* UIElement::CrackMouseButtonEvent(MouseButtonEventArgs* pArgs)
{
    switch (pArgs->get_ChangedButton())
    {
    case MouseButton::Left:
        if (pArgs->get_RoutedEvent() != &Mouse::get_PreviewMouseDownEvent())
        {
            if (pArgs->get_RoutedEvent() == &Mouse::get_MouseDownEvent())
            {
                return &get_MouseLeftButtonDownEvent();
            }
            if (pArgs->get_RoutedEvent() == &Mouse::get_PreviewMouseUpEvent())
            {
                return &get_PreviewMouseLeftButtonUpEvent();
            }
            return &get_MouseLeftButtonUpEvent();
        }
        return &get_PreviewMouseLeftButtonDownEvent();

    case MouseButton::Middle:
        return NULL;

    case MouseButton::Right:
        if (pArgs->get_RoutedEvent() != &Mouse::get_PreviewMouseDownEvent())
        {
            if (pArgs->get_RoutedEvent() == &Mouse::get_MouseDownEvent())
            {
                return &get_MouseRightButtonDownEvent();
            }
            if (pArgs->get_RoutedEvent() == &Mouse::get_PreviewMouseUpEvent())
            {
                return &get_PreviewMouseRightButtonUpEvent();
            }
            return &get_MouseRightButtonUpEvent();
        }
        return &get_PreviewMouseRightButtonDownEvent();
    }
    return NULL;
}

void UIElement::ReRaiseEventAs(RoutedEventArgs* pArgs, const RoutedEvent* pNewEvent)
{
    const RoutedEvent* pRoutedEvent = pArgs->get_RoutedEvent();
    Object* pSource = pArgs->get_Source();

    pArgs->OverrideRoutedEvent(pNewEvent);

    RaiseEvent(pArgs);

    pArgs->OverrideRoutedEvent(pRoutedEvent);
    pArgs->OverrideSource(pSource);
}

void UIElement::OnKeyDownThunk(Object* pSender, KeyEventArgs* pArgs) 
{ 
    CommandManager::TranslateInput((DependencyObject*)pSender, pArgs);

    if (!pArgs->get_IsHandled())
    {
        ((UIElement*) pSender)->OnKeyDown(pArgs); 
    }
}

void UIElement::OnMouseWheelThunk(Object* pSender, MouseEventArgs* pArgs) 
{ 
    CommandManager::TranslateInput((DependencyObject*)pSender, pArgs);

    if (!pArgs->get_IsHandled())
    {
        ((UIElement*) pSender)->OnMouseWheel((MouseWheelEventArgs*) pArgs); 
    }
}


DependencyObject* UIElement::GetTemplateChild(const String& childName)
{
    FrameworkTemplate* pTemplateInternal = get_TemplateInternal();
    if (pTemplateInternal == NULL)
    {
        return NULL;
    }
    return object_cast<DependencyObject>(pTemplateInternal->FindName(childName, this));
}

ObjectPtr UIElement::GetActualWidth(DependencyObject* pDO, BaseValueSource::Enum* pValueSource)
{
    UIElement* pElement = (UIElement*) pDO;
//    if (pElement->get_HasWidthEverChanged())
    {
        *pValueSource = BaseValueSource::Local;
        return BoxValue(pElement->get_RenderSize().get_Width());
    }
//    source = BaseValueSourceInternal.Default;
//    return 0.0f;
}

ObjectPtr UIElement::GetActualHeight(DependencyObject* pDO, BaseValueSource::Enum* pValueSource)
{
    UIElement* pElement = (UIElement*) pDO;
//    if (pElement->get_HasHeightEverChanged())
    {
        *pValueSource = BaseValueSource::Local;
        return BoxValue(pElement->get_RenderSize().get_Height());
    }
//    source = BaseValueSourceInternal.Default;
//    return 0.0f;
}

ObjectPtr UIElement::GetIsVisible(DependencyObject* pDO, BaseValueSource::Enum* pValueSource)
{
    UIElement* pElement = (UIElement*) pDO;

    return BoxValue(pElement->m_isVisibleCache);
}

bool UIElement::CaptureMouse()
{
    return Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_Mouse()->CaptureMouse(this);
}

void UIElement::ReleaseMouseCapture()
{
    Mouse* pMouse = Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_Mouse();

    if(pMouse->get_MouseCapture() == this)
    {
        pMouse->CaptureMouse(NULL);
    }
}

const AVUI::DependencyProperty& UIElement::get_ToolTipProperty()
{
    const static DependencyProperty& property = ToolTipService::get_ToolTipProperty();
    return property;
}

const AVUI::DependencyProperty& UIElement::get_ContextMenuProperty()
{
    const static DependencyProperty& property = ContextMenuService::get_ContextMenuProperty();
    return property;
}


void LayoutTransformData::CreateTransformSnapshot(Transform* pSourceTransform)
{
    m_pTransform = object_allocate<MatrixTransform>(pSourceTransform->get_Value());
}

FocusWithinProperty* UIElement::get_FocusWithinProperty()
{
    static FocusWithinProperty focusWithinProperty;

    return &focusWithinProperty;
}

MouseCaptureWithinProperty* UIElement::get_MouseCaptureWithinProperty()
{
    static MouseCaptureWithinProperty mouseCaptureWithinProperty;

    return &mouseCaptureWithinProperty;
}

MouseOverProperty* UIElement::get_MouseOverProperty()
{
    static MouseOverProperty mouseOverProperty;

    return &mouseOverProperty;
}

void UIElement::UpdateIsVisibleCache()
{
    bool flag = (get_Visibility() == Visibility::Visible);
    if (flag)
    {
        if (get_VisualParent() != NULL && get_VisualParent()->IsType(GET_TYPE(UIElement)))
        {
            flag = ((UIElement*)get_VisualParent())->get_IsVisible();
        }
        else if (PresentationSource::CriticalFromVisual(this) != NULL)
        {
            flag = true;
        }
    }
    if (flag != get_IsVisible())
    {
        m_isVisibleCache = flag;

        DependencyPropertyChangedEventArgs args(get_IsVisibleProperty(), BoxValue(!flag), BoxValue(flag));
        NotifyPropertyChange(args);
    }    
}

void UIElement::OnIsVisibleChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    UIElement* pV = (UIElement*) pDO;
    pV->invoke_IsVisibleChanged(pV, (DependencyPropertyChangedEventArgs*) &e);
    InvalidateForceInheritPropertyOnChildren(pV, e.DependencyPropertyChanged);

    InputManager::get_Current()->NotifyHitTestInvalidated();
}

void UIElement::PropagateResumeLayout(Visual* pParent, Visual* pV)
{
    bool flag3 = pParent != NULL && pParent->CheckFlagsAnd(VisualFlags::IsLayoutSuspended);
    unsigned int num3 = (pParent == NULL) ? 0 : pParent->GetTreeLevel();

    if (!flag3)
    {
        pV->SetFlags(false, VisualFlags::IsLayoutSuspended);
        pV->SetTreeLevel(num3 + 1);

        /*
        if (pV->CheckFlagsAnd(VisualFlags::IsUIElement))
        {
            UIElement* pE = (UIElement*) pV;
            if(pE->get_MeasureInProgress() || pE->get_ArrangeInProgress())
            {
                LibraryCriticalError();
            }

            bool flag2 = (pE->get_MeasureDirty() && !pE->get_NeverMeasured()) && (pE->get_MeasureRequest() == NULL);
            bool flag = (pE->get_ArrangeDirty() && !pE->get_NeverArranged()) && (pE->get_ArrangeRequest == NULL);
            LayoutManager* pManager = (flag2 || flag) ? get_LayoutManager() : NULL;
            if (flag2)
            {
                pManager->AddToMeasureQueue(e);
            }
            if (flag)
            {
                pManager->AddToArrangeQueue(e);
            }
        }
        */

        int visualChildCount = pV->get_VisualChildCount();
        for (int i = 0; i < visualChildCount; i++)
        {
            Visual* pVisualChild = pV->get_VisualChild(i);
            if (pVisualChild != NULL)
            {
                PropagateResumeLayout(pV, pVisualChild);
            }
        }
    }
}

void UIElement::PropagateSuspendLayout(Visual* pV)
{
    if (!pV->CheckFlagsAnd(VisualFlags::IsLayoutSuspended))
    {
        pV->SetFlags(true, VisualFlags::IsLayoutSuspended);
        pV->SetTreeLevel(0);
        int visualChildCount = pV->get_VisualChildCount();
        for (int i = 0; i < visualChildCount; i++)
        {
            Visual* pVisualChild = pV->get_VisualChild(i);
            if (pVisualChild != NULL)
            {
                PropagateSuspendLayout(pVisualChild);
            }
        }
    }
}

void UIElement::AddLogicalChild(Object* pChild)
{
    if(pChild != NULL)
    {
        TryFireInitialized();
        if(pChild->IsType(GET_TYPE(UIElement)))
        {
            ((UIElement*)pChild)->ChangeLogicalParent(this);
        }
    }
}

void UIElement::RemoveLogicalChild(Object* pChild)
{
    if(pChild != NULL)
    {
        if(pChild->IsType(GET_TYPE(UIElement)) && ((UIElement*)pChild)->get_Parent() == this)
        {
            ((UIElement*)pChild)->ChangeLogicalParent(NULL);
        }
    }
}


void UIElement::ChangeLogicalParent(DependencyObject* pNewParent)
{
    if (m_pLogicalParent != NULL && pNewParent != NULL && m_pLogicalParent != pNewParent)
    {
        LibraryCriticalError();
    }
    if (pNewParent == this)
    {
        LibraryCriticalError();
    }

    OnNewParent(pNewParent);
    TryFireInitialized();
}

void UIElement::OnNewParent(DependencyObject* pNewParent)
{
    m_pLogicalParent = pNewParent;
}

class InputHitTestResult
{

public:
    InputHitTestResult() { }
    
    void InputHitTestResultCallback(Object* pResult, HitTestResultArgs* pArgs)
    {
        m_pResult = (HitTestResult*) pResult;
        pArgs->ResultBehavior = HitTestResultBehavior::Stop;
    } 

    DependencyObject* get_Result()
    {
        if (m_pResult == NULL)
        {
            return NULL;
        }
        return m_pResult->get_VisualHit();
    }

private:
    TRefCountedPtr<HitTestResult> m_pResult;
};


void UIElement::InputHitTest(const Point& pt, DependencyObject** ppEnabledHit, DependencyObject** ppRawHit)
{
    *ppRawHit = NULL;
    *ppEnabledHit = NULL;

    PointHitTestParameters hitTestParameters(pt);
    InputHitTestResult result;

    VisualTreeHelper::HitTest(this, HitTestFilterCallback::Create(this, &UIElement::InputHitTestFilterCallback), 
                              HitTestResultCallback::Create(&result, &InputHitTestResult::InputHitTestResultCallback), 
                              &hitTestParameters);

    DependencyObject* pDO = result.get_Result();
    *ppRawHit = pDO;

    while (pDO != NULL)
    {
        /*
        IContentHost host = o as IContentHost;
        if ((host != null) && ((bool) InputElement.GetContainingUIElement(o).GetValue(IsEnabledProperty)))
        {
            pt = InputElement.TranslatePoint(pt, this, o);
            enabledHit = rawHit = host.InputHitTest(pt);
            if (enabledHit != null)
            {
                return;
            }
        }
        */

        UIElement* pElement = object_cast<UIElement>(pDO);
        if (pElement != NULL)
        {
            if (*ppRawHit == NULL)
            {
                *ppRawHit = pElement;
            }
            if (pElement->get_IsEnabled())
            {
                *ppEnabledHit = pElement;
                return;
            }
        }
        /*
        UIElement3D elementd = o as UIElement3D;
        if (elementd != null)
        {
            if (rawHit == null)
            {
                rawHit = elementd;
            }
            if (elementd.IsEnabled)
            {
                enabledHit = elementd;
                return;
            }
        }
        */

        if (pDO == this)
        {
            return;
        }
        pDO = VisualTreeHelper::GetParent(pDO);
    }
}

void UIElement::InputHitTestFilterCallback(Object* pCurrentNode, HitTestFilterArgs* pArgs)
{
    pArgs->FilterBehavior = HitTestFilterBehavior::Continue;

    if (UIElementHelper::IsUIElementOrUIElement3D((DependencyObject*)pCurrentNode))
    {
        if (!UIElementHelper::IsVisible((DependencyObject*)pCurrentNode))
        {
            pArgs->FilterBehavior = HitTestFilterBehavior::ContinueSkipSelfAndChildren;
        }
        if (!UIElementHelper::IsHitTestVisible((DependencyObject*)pCurrentNode))
        {
            pArgs->FilterBehavior = HitTestFilterBehavior::ContinueSkipSelfAndChildren;
        }
    }
    else
    {
        pArgs->FilterBehavior = HitTestFilterBehavior::Continue;
    }
}


class PopupRoot;
Visual* PresentationSource::CriticalFromVisual(Visual* pVisual)
{
    while(pVisual != NULL)
    {
        if(pVisual->IsType(GET_TYPE(Window)))
        {
            return pVisual;
        }
        if(pVisual->IsType(GET_TYPE(PopupRoot)))
        {
            return pVisual;
        }
        pVisual = pVisual->get_VisualParent();
    }
    return pVisual;
}


}; // namespace AVUI
