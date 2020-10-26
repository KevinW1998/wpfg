#include <AVUICommon.h>
#include <AVUIProgressBar.h>
#include <AVUIControlTemplate.h>

REGISTER_ELEMENT_BEGIN(AVUI::ProgressBar)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Orientation)
REGISTER_ELEMENT_END()


namespace AVUI
{

void ProgressBar::OnApplyTemplate()
{
    RangeBase::OnApplyTemplate();

    ControlTemplate* pTemplate = get_Template();
    if(pTemplate != NULL)
    {
        m_pTrack = object_cast<UIElement>(pTemplate->FindName(String(L"PART_Track"),  this));
        m_pIndicator = object_cast<UIElement>(pTemplate->FindName(String(L"PART_Indicator"),  this));
    }

    if(m_pTrack != NULL)
    {
        m_pTrack->add_SizeChanged(RoutedEventHandler::Create(this, &ProgressBar::OnTrackSizeChanged));
    }

}

void ProgressBar::SetProgressBarIndicatorLength()
{
    if(m_pTrack == NULL || m_pTrack == NULL)
    {
        return;
    }

    float progress = (get_Value() - get_Minimum()) / (get_Maximum() - get_Minimum());

    progress = Float::Max(Float::Min(progress, 1.0f), 0.0f);

    if(get_Orientation() == Orientation::Horizontal)
    {
        m_pIndicator->set_Width(progress * m_pTrack->get_RenderSize().get_Width());
    }
    else
    {
        m_pIndicator->set_Height(progress * m_pTrack->get_RenderSize().get_Height());
    }
}



}; // end namespace AVUI
