#include <AVUICommon.h>
#include <AVUITileBrush.h>

REGISTER_ELEMENT_BEGIN(AVUI::TileBrush)

    REGISTER_PROPERTY(Viewbox);
    REGISTER_PROPERTY(Viewport);
    REGISTER_PROPERTY(ViewboxUnits);     
    REGISTER_PROPERTY(ViewportUnits);     
    REGISTER_PROPERTY(Stretch);     
    REGISTER_PROPERTY(TileMode);     
    REGISTER_PROPERTY(AlignmentX);
    REGISTER_PROPERTY(AlignmentY);
REGISTER_ELEMENT_END()


namespace AVUI {

void TileBrush::OnViewboxChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_viewBox = AVUI::UnboxValue<Rect>(e.pNewValue);
}

void TileBrush::OnViewportChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_viewPort = AVUI::UnboxValue<Rect>(e.pNewValue);
}

void TileBrush::OnViewboxUnitsChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_viewboxUnits = (BrushMappingMode::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}

void TileBrush::OnViewportUnitsChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_viewportUnits = (BrushMappingMode::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}

void TileBrush::OnStretchChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_stretch = (Stretch::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}

void TileBrush::OnTileModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_tileMode = (TileMode::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}

void TileBrush::OnAlignmentXChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_alignmentX = (AlignmentX::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}

void TileBrush::OnAlignmentYChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((TileBrush*) pDO)->m_alignmentY = (AlignmentY::Enum) (AVUI::UnboxValue<int>(e.pNewValue));
}


}; // namespace AVUI
