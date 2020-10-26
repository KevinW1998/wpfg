#include <AVUICommon.h>
#include <AVUIDefinitionBase.h>
#include <AVUIColumnDefinition.h>
#include <AVUIRowDefinition.h>

REGISTER_ELEMENT_BEGIN(AVUI::DefinitionBase)

REGISTER_ELEMENT_END()

namespace AVUI {

float DefinitionBase::get_UserMaxSize()
{
    if(m_isColumnDefinition)
    {
        return ((ColumnDefinition*)this)->get_MaxWidth();
    }
    else
    {
        return ((RowDefinition*)this)->get_MaxHeight();
    }
}
float DefinitionBase::get_UserMinSize()
{
    if(m_isColumnDefinition)
    {
        return ((ColumnDefinition*)this)->get_MinWidth();
    }
    else
    {
        return ((RowDefinition*)this)->get_MinHeight();
    }
}

const GridLength& DefinitionBase::get_UserSize()
{
    if(m_isColumnDefinition)
    {
        return ((ColumnDefinition*)this)->get_Width();
    }
    else
    {
        return ((RowDefinition*)this)->get_Height();
    }
}

void DefinitionBase::OnBeforeLayout(Grid* pGrid)
{
    m_pGrid = pGrid;
    m_minSize = 0;
    m_layoutWasUpdated = true;
}


}; // namespace AVUI
