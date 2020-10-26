#include <AVUICommon.h>
#include <AVUIToolTip.h>
#include <AVUIToolTipService.h>
#include <AVUIPopup.h>
#include <AVUIPopupControlService.h>

REGISTER_ELEMENT_BEGIN(AVUI::ToolTip)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(IsOpen)
    REGISTER_PROPERTY(HasDropShadow)
    REGISTER_PROPERTY(StaysOpen)
    REGISTER_PROPERTY(HorizontalOffset)
    REGISTER_PROPERTY(VerticalOffset)
    REGISTER_PROPERTY(PlacementTarget)
    REGISTER_PROPERTY(PlacementRectangle)
    REGISTER_PROPERTY(Placement)
    REGISTER_ROUTED_EVENT(Opened)
    REGISTER_ROUTED_EVENT(Closed)
REGISTER_ELEMENT_END()


namespace AVUI {

//
//    UIElement.FocusableProperty.OverrideMetadata(typeof(ToolTip), new FrameworkPropertyMetadata(false));


void ToolTip::ForceClose()
{
    if (m_pParentPopup != NULL)
    {
        m_pParentPopup->ForceClose();
    }
}

 
void ToolTip::OnIsOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ToolTip* pElement = (ToolTip*) pDO;
    if (UnboxValue<bool>(e.pNewValue))
    {
        if (pElement->m_pParentPopup == NULL)
        {
            pElement->HookupParentPopup();
        }
    }
}


void ToolTip::HookupParentPopup()
{
    m_pParentPopup = object_allocate<Popup>();
    m_pParentPopup->set_AllowsTransparency(true);
    m_pParentPopup->set_HitTestable(!get_StaysOpen());
    CoerceValue(get_HasDropShadowProperty());
    m_pParentPopup->add_Opened(EventHandler::Create(this, &ToolTip::OnPopupOpened));
    m_pParentPopup->add_Closed(EventHandler::Create(this, &ToolTip::OnPopupClosed));
    m_pParentPopup->add_PopupCouldClose(EventHandler::Create(this, &ToolTip::OnPopupCouldClose));

//    this._parentPopup.SetResourceReference(Popup.PopupAnimationProperty, SystemParameters.ToolTipPopupAnimationKey);
    m_pParentPopup->set_PopupAnimation(PopupAnimation::Fade);
    Popup::CreateRootPopup(m_pParentPopup, this);
}

void ToolTip::OnPopupOpened(Object* pSource, EventArgs* e)
{
    RoutedEventArgs args(&get_OpenedEvent(), this);
    OnOpened(&args);
}

void ToolTip::OnPopupClosed(Object* pSource, EventArgs* e)
{
    RoutedEventArgs args(&get_ClosedEvent(), this);
    OnClosed(&args);
}

void ToolTip::OnPopupCouldClose(Object* pSource, EventArgs* e)
{
    set_IsOpen(false);
}

void ToolTip::OnOpened(RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void ToolTip::OnClosed(RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}


const AVUI::DependencyProperty& ToolTip::get_StaysOpenProperty()
{
    const static DependencyProperty& property = Popup::get_StaysOpenProperty();
    return property;
}

const AVUI::DependencyProperty& ToolTip::get_HasDropShadowProperty()
{
    const static DependencyProperty& property = Popup::get_HasDropShadowProperty();
    return property;
}

const AVUI::DependencyProperty& ToolTip::get_HorizontalOffsetProperty()
{ 
    static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoerceHorizontalOffset));
    const static AVUI::DependencyProperty& property = ToolTipService::get_HorizontalOffsetProperty().AddOwner(GET_TYPE(ToolTip), BoxValue(0.0f), &metadata);

    return property;
}

const AVUI::DependencyProperty& ToolTip::get_VerticalOffsetProperty()
{ 
    static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoerceVerticalOffset));
    const static AVUI::DependencyProperty& property = ToolTipService::get_HorizontalOffsetProperty().AddOwner(GET_TYPE(ToolTip), BoxValue(0.0f), &metadata);

    return property;
}

const AVUI::DependencyProperty& ToolTip::get_PlacementTargetProperty()
{ 
    static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoercePlacementTarget));
    const static AVUI::DependencyProperty& property = ToolTipService::get_PlacementTargetProperty().AddOwner(GET_TYPE(ToolTip), NULL, &metadata);

    return property;
}

const AVUI::DependencyProperty& ToolTip::get_PlacementRectangleProperty()
{ 
    static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoercePlacementRectangle));
    const static AVUI::DependencyProperty& property = ToolTipService::get_PlacementTargetProperty().AddOwner(GET_TYPE(ToolTip), BoxValue(Rect::get_Empty()), &metadata);

    return property;
}

const AVUI::DependencyProperty& ToolTip::get_PlacementProperty()
{ 
    static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, NULL, CoercePlacement));
    const static AVUI::DependencyProperty& property = ToolTipService::get_PlacementProperty().AddOwner(GET_TYPE(ToolTip), BoxValue((int)PlacementMode::Mouse), &metadata);

    return property;
}

ObjectPtr ToolTip::CoerceHorizontalOffset(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ToolTipService::get_HorizontalOffsetProperty());
}

ObjectPtr ToolTip::CoerceVerticalOffset(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ToolTipService::get_VerticalOffsetProperty());
}

ObjectPtr ToolTip::CoercePlacementTarget(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ToolTipService::get_PlacementTargetProperty());
}

ObjectPtr ToolTip::CoercePlacementRectangle(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ToolTipService::get_PlacementRectangleProperty());
}

ObjectPtr ToolTip::CoercePlacement(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ToolTipService::get_PlacementProperty());
}


}; // namespace AVUI


