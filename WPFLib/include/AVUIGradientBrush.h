#pragma once

#include <AVUIBrush.h>
#include <AVUIGradientStop.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class GradientStopCollection : public TObjectCollection<GradientStop>
{
public:
    DECLARE_ELEMENT(GradientStopCollection, ObjectCollectionBase);
};

class GradientBrush : public Brush
{
public:

    DECLARE_ELEMENT(GradientBrush, Brush);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(MappingMode, BrushMappingMode, BrushMappingMode::RelativeToBoundingBox, DependencyPropertyMetadata(PropertyFlag::None, &GradientBrush::OnMappingModeChanged));
    const BrushMappingMode::Enum get_MappingMode() const { return m_mappingMode; }
    void set_MappingMode(const BrushMappingMode::Enum& value) { SetValue(get_MappingModeProperty(), BoxValue((int)value)); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY_DEFINITION(SpreadMethod, GradientSpreadMethod, GradientSpreadMethod::Pad, DependencyPropertyMetadata(PropertyFlag::None, &GradientBrush::OnSpreadMethodChanged));
    const GradientSpreadMethod::Enum get_SpreadMethod() const { return m_spreadMethod; }
    void set_SpreadMethod(const GradientSpreadMethod::Enum& value) { SetValue(get_SpreadMethodProperty(), BoxValue((int)value)); }

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(GradientStops, GradientStopCollection, true);

    GradientStopCollection* get_GradientStops() { return &m_gradients; }
    const GradientStopCollection* get_GradientStops() const { return &m_gradients; }

protected:

    GradientBrush() : m_mappingMode(BrushMappingMode::RelativeToBoundingBox), m_spreadMethod(GradientSpreadMethod::Pad), m_gradients(this) { };
    ~GradientBrush() { m_gradients.InvalidateWeakReferences(); };

    virtual void OnGradientStopsChanged() = 0;

private:

    static void OnMappingModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnSpreadMethodChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    void OnGradientStopAdded(GradientStop* pAdd)
    {
        pAdd->add_Changed(EventHandler::Create(this, &GradientBrush::OnGradientStopChangedEvent));

        OnGradientStopsChanged();
    }

    void OnGradientStopRemoved(GradientStop* pRemove)
    {
        pRemove->remove_Changed(EventHandler::Create(this, &GradientBrush::OnGradientStopChangedEvent));

        OnGradientStopsChanged();
    }

    void OnGradientStopChangedEvent(Object* pObject, EventArgs* pArgs)
    {
        OnGradientStopsChanged();
    }


    class GradientBrushGradientStopCollection : public GradientStopCollection
    {
    public:
        GradientBrushGradientStopCollection(GradientBrush* pOwner) : m_pOwner(pOwner) { }

    private:
        virtual void OnAdd(GradientStop* pAdd) { if(m_pOwner != NULL) m_pOwner->OnGradientStopAdded(pAdd); }
        virtual void OnRemove(GradientStop* pRemove) { if(m_pOwner != NULL) m_pOwner->OnGradientStopRemoved(pRemove); }

        GradientBrush* m_pOwner;
    };

    BrushMappingMode::Enum m_mappingMode;
    GradientSpreadMethod::Enum m_spreadMethod;

    GradientBrushGradientStopCollection m_gradients;
};

typedef TRefCountedPtr<GradientBrush> GradientBrushPtr;

}; // namespace AVUI
