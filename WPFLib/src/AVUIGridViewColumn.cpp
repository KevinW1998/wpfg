#include <AVUICommon.h>
#include <AVUIGridViewColumn.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridViewColumn)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Header)
    REGISTER_PROPERTY(HeaderContainerStyle)
    REGISTER_PROPERTY(HeaderTemplate)
    REGISTER_PROPERTY(HeaderTemplateSelector)
    REGISTER_PROPERTY(CellTemplate)
    REGISTER_PROPERTY(CellTemplateSelector)
    REGISTER_PROPERTY(DisplayMemberBinding)
    REGISTER_PROPERTY(Width)
REGISTER_ELEMENT_END()

namespace AVUI {

void GridViewColumn::OnWidthChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    GridViewColumn* pColumn = (GridViewColumn*) pDO;
    float newValue = UnboxValue<float>(e.pNewValue);
    pColumn->set_State(Float::IsNaN(newValue) ? ColumnMeasureState::Init : ColumnMeasureState::SpecificWidth);

    pColumn->OnPropertyChanged(&get_WidthProperty());
}

void GridViewColumn::set_State(ColumnMeasureState::Enum newState)
{
    if (m_state != newState)
    {
        m_state = newState;
        if (newState != ColumnMeasureState::Init)
        {
            UpdateActualWidth();
        }
        else
        {
            set_DesiredWidth(0);
        }
    }
    else if (newState == ColumnMeasureState::SpecificWidth)
    {
        UpdateActualWidth();
    }
}

void GridViewColumn::set_ActualWidth(float actualWidth)
{
    if (!Float::IsNaN(actualWidth) && !Float::IsInfinity(actualWidth) && actualWidth >= 0 && m_actualWidth != actualWidth)
    {
        m_actualWidth = actualWidth;
        OnPropertyChanged(&get_ActualWidthProperty());
    }
}

void GridViewColumn::OnPropertyChanged(const Property* pProperty)
{
    PropertyChangedEventArgs args(*pProperty);

    OnPropertyChanged(&args);
} 

void GridViewColumn::OnPropertyChanged(PropertyChangedEventArgs* pArgs)
{
    invoke_PropertyChanged(this, pArgs);
}


void GridViewColumn::ResetPrivateData()
{
    m_actualIndex = -1;
    m_desiredWidth = 0;
}

void GridViewColumn::UpdateActualWidth()
{
    set_ActualWidth(get_State() == ColumnMeasureState::SpecificWidth ? get_Width() : get_DesiredWidth());
}

float GridViewColumn::EnsureWidth(float width)
{
    if (width > get_DesiredWidth())
    {
        set_DesiredWidth(width);
    }
    return get_DesiredWidth();
}

 

 



}; // namespace AVUI


