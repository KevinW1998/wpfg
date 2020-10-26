#include <AVUICommon.h>
#include <AVUIShape.h>
#include <AVUIPen.h>
#include <AVUIGeometry.h>

REGISTER_ELEMENT_BEGIN(AVUI::Shape)

    REGISTER_PROPERTY(Fill);
    REGISTER_PROPERTY(Stroke);
    REGISTER_PROPERTY(StrokeThickness);
    REGISTER_PROPERTY(Stretch);
REGISTER_ELEMENT_END()

namespace AVUI {

Shape::Shape()
{

}

Pen* Shape::GetPen()
{
    if(get_IsPenNoOp())
    {
        return NULL;
    }

    if(m_pPen == NULL)
    {
        m_pPen = PenPtr(new Pen(), false);
        m_pPen->set_Thickness(get_StrokeThickness());
        m_pPen->set_Brush(get_Stroke());
    }

    return m_pPen;
}

void Shape::OnPenPropertyChanged(Object* pShape, const DependencyPropertyChangedEventArgs& e)
{
    if(!e.IsSubPropertyChange)
    {
        object_cast<Shape>(pShape)->m_pPen = NULL;
    }
}

bool Shape::get_IsPenNoOp()
{
    float strokeThickness = get_StrokeThickness();
    if (get_Stroke() != NULL && !Float::IsNaN(strokeThickness))
    {
        return Float::IsZero(strokeThickness);
    }
    return true;
}

float Shape::GetStrokeThickness()
{
    if (get_IsPenNoOp())
    {
        return 0.0f;
    }
    return Float::Abs(get_StrokeThickness());
}

 

 



}; // namespace AVUI
