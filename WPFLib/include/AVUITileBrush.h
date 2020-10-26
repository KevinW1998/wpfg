#pragma once

#include <AVUIGradientBrush.h>

namespace AVUI {

class TileBrush : public Brush
{
public:
    TileBrush() : m_viewBox(0.0f, 0.0f, 1.0f, 1.0f), m_viewPort(0.0f, 0.0f, 1.0f, 1.0f),
                  m_viewboxUnits(BrushMappingMode::RelativeToBoundingBox),
                  m_viewportUnits(BrushMappingMode::RelativeToBoundingBox),
                  m_stretch(Stretch::Fill),
                  m_tileMode(TileMode::None),
                  m_alignmentX(AlignmentX::Center),
                  m_alignmentY(AlignmentY::Center)
    { };

    ~TileBrush() { };

    DECLARE_ELEMENT(TileBrush, Brush);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(Viewbox, Rect, Rect(0.0f, 0.0f, 1.0f, 1.0f), DependencyPropertyMetadata(PropertyFlag::None, OnViewboxChanged));
    const Rect &get_Viewbox() const { return m_viewBox; }
    void set_Viewbox(const Rect& value) { SetValue(get_ViewboxProperty(), AVUI::BoxValue(value)); }

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(Viewport, Rect, Rect(0.0f, 0.0f, 1.0f, 1.0f), DependencyPropertyMetadata(PropertyFlag::None, OnViewportChanged));
    const Rect &get_Viewport() const { return m_viewPort; }
    void set_Viewport(const Rect& value) { SetValue(get_ViewportProperty(), AVUI::BoxValue(value)); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(ViewboxUnits, BrushMappingMode, BrushMappingMode::RelativeToBoundingBox, DependencyPropertyMetadata(PropertyFlag::None, OnViewboxUnitsChanged));
    const BrushMappingMode::Enum get_ViewboxUnits() const { return m_viewboxUnits; }
    void set_ViewboxUnits(const BrushMappingMode::Enum& value) { SetValue(get_ViewboxUnitsProperty(), BoxValue((int)value)); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(ViewportUnits, BrushMappingMode, BrushMappingMode::RelativeToBoundingBox, DependencyPropertyMetadata(PropertyFlag::None, OnViewportUnitsChanged));
    const BrushMappingMode::Enum get_ViewportUnits() const { return m_viewportUnits; }
    void set_ViewportUnits(const BrushMappingMode::Enum& value) { SetValue(get_ViewportUnitsProperty(), BoxValue((int)value)); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(Stretch, Stretch, Stretch::Fill, DependencyPropertyMetadata(PropertyFlag::None, OnStretchChanged));
    const Stretch::Enum get_Stretch() const { return m_stretch; }
    void set_Stretch(const Stretch::Enum& value) { SetValue(get_StretchProperty(), BoxValue((int)value)); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(TileMode, TileMode, TileMode::None, DependencyPropertyMetadata(PropertyFlag::None, OnTileModeChanged));
    const TileMode::Enum get_TileMode() const { return m_tileMode; }
    void set_TileMode(const TileMode::Enum& value) { SetValue(get_TileModeProperty(), BoxValue((int)value)); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(AlignmentX, AlignmentX, AlignmentX::Center, DependencyPropertyMetadata(PropertyFlag::None, OnAlignmentXChanged));
    const AlignmentX::Enum get_AlignmentX() const { return m_alignmentX; }
    void set_AlignmentX(const AlignmentX::Enum& value) { SetValue(get_AlignmentXProperty(), BoxValue((int)value)); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(AlignmentY, AlignmentY, AlignmentY::Center, DependencyPropertyMetadata(PropertyFlag::None, OnAlignmentYChanged));
    const AlignmentY::Enum get_AlignmentY() const { return m_alignmentY; }
    void set_AlignmentY(const AlignmentY::Enum& value) { SetValue(get_AlignmentYProperty(), BoxValue((int)value)); }

private:
    static void OnViewboxChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnViewportChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnViewboxUnitsChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnViewportUnitsChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnStretchChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnTileModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnAlignmentXChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnAlignmentYChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    Rect m_viewBox;
    Rect m_viewPort;
    BrushMappingMode::Enum m_viewboxUnits;
    BrushMappingMode::Enum m_viewportUnits;
    Stretch::Enum m_stretch;
    TileMode::Enum m_tileMode;
    AlignmentX::Enum m_alignmentX;
    AlignmentY::Enum m_alignmentY;
};

typedef TRefCountedPtr<TileBrush> TileBrushPtr;

}; // namespace AVUI
