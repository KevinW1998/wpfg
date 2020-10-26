#include <AVUICommon.h>
#include <AVUICaretElement.h>
#include <AVUISolidColorBrush.h>
#include <AVUIVisualDrawingContext.h>
#include <AVUISingleAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::CaretElement)

REGISTER_ELEMENT_END()

namespace AVUI {

void CaretElement::UpdateRender()
{
    if(m_pBrush == NULL)
    {
        m_pBrush = BrushPtr(new SolidColorBrush(), false);
    }

    VisualDrawingContext ctx(this);

    ctx.DrawRoundedRectangle(m_pBrush, NULL, Rect(-1.0f, 0.0f, 2.0f, m_height), 0.0f, 0.0f);
}

void CaretElement::ShowCaret()
{
    SingleAnimationPtr pAnimation(new SingleAnimation(), false);

    pAnimation->set_From(0.0f);
    pAnimation->set_To(1.0f);
    pAnimation->set_Duration(TimeSpan::FromMilliseconds(650));
    pAnimation->set_AutoReverse(true);
    pAnimation->set_RepeatBehavior(RepeatBehavior::get_Forever());

    BeginAnimation(get_CaretOpacityProperty(), pAnimation, HandoffBehavior::SnapshotAndReplace);

}
void CaretElement::HideCaret()
{
    EndAnimation(get_CaretOpacityProperty());
    set_VisualOpacity(0.0f);
}




}; // namespace AVUI
