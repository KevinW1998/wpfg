#include <AVUICommon.h>
#include <AVUIGradientBrush.h>

REGISTER_ELEMENT_BEGIN(AVUI::GradientStopCollection)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::GradientBrush)

    REGISTER_PROPERTY(MappingMode);
    REGISTER_PROPERTY(SpreadMethod);
    REGISTER_PROPERTY(GradientStops);     
REGISTER_ELEMENT_END()

namespace AVUI {

void GradientBrush::OnMappingModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((GradientBrush*) pDO)->m_mappingMode = (BrushMappingMode::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}

void GradientBrush::OnSpreadMethodChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((GradientBrush*) pDO)->m_spreadMethod = (GradientSpreadMethod::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}


}; // namespace AVUI
