#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

class Brush;
class Pen : public Animatable
{
public:
    Pen() : m_width(1.0f), m_pBrush(NULL) { }
    ~Pen() { };

    DECLARE_ELEMENT(Pen, Animatable);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY_DEFINITION(Brush, Brush, NULL, DependencyPropertyMetadata(PropertyFlag::None, Pen::OnBrushChanged));
    Brush* get_Brush() const { return m_pBrush; }
    void set_Brush(Brush* pBrush) { SetValue(get_BrushProperty(), (Object*) pBrush); }
    static void OnBrushChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
    {
        ((Pen*)pDO)->m_pBrush = (Brush*) e.pNewValue;
    }

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Thickness, float, 1.0f, PropertyFlag::None);

private:
    float m_width;
    Brush* m_pBrush;
};


typedef TRefCountedPtr<Pen> PenPtr;

}; // namespace AVUI
