#include <AVUICommon.h>
#include <AVUIImage.h>
#include <AVUIImageBrush.h>
#include <AVUIViewbox.h>

REGISTER_ELEMENT_BEGIN(AVUI::Image)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Source);
    REGISTER_PROPERTY(Stretch);
    REGISTER_PROPERTY(StretchDirection);
REGISTER_ELEMENT_END()

namespace AVUI {

Image::~Image() { }

Size Image::MeasureOverride(const Size& constraintSize)
{
    if(m_pBrush == NULL)
    {
        m_pBrush = object_allocate<ImageBrush>();
    }

    m_pBrush->set_ImageSource(get_Source());

    return MeasureArrangeHelper(constraintSize);
}

Size Image::ArrangeOverride(const Size& finalSize)
{    
    return MeasureArrangeHelper(finalSize);
}

Size Image::MeasureArrangeHelper(const Size& inputSize)
{
    Size contentSize(m_pBrush->get_NaturalWidth(), m_pBrush->get_NaturalHeight());

    Size size2 = Viewbox::ComputeScaleFactor(inputSize, contentSize, get_Stretch(), get_StretchDirection());
    return Size(contentSize.get_Width() * size2.get_Width(), contentSize.get_Height() * size2.get_Height());
}

 
void Image::OnRender(DrawingContext& drawingContext)
{
    drawingContext.DrawRoundedRectangle(m_pBrush, NULL, Rect(get_RenderSize()), 0.0f, 0.0f);
}


}; // namespace AVUI
