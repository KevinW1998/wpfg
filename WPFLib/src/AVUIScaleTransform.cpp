#include <AVUICommon.h>
#include <AVUIScaleTransform.h>
#include <AVUIMatrix.h>

REGISTER_ELEMENT_BEGIN(AVUI::ScaleTransform)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(ScaleX);
    REGISTER_PROPERTY(ScaleY);
    REGISTER_PROPERTY(CenterX);
    REGISTER_PROPERTY(CenterY);
REGISTER_ELEMENT_END()

namespace AVUI {

Matrix ScaleTransform::get_Value()
{
    float scaleX = get_ScaleX();
    float scaleY = get_ScaleY();
    float centerX = get_CenterX();
    float centerY = get_CenterY();

    return Matrix(scaleX, 0.0f, 0.0f, scaleY, centerX - (scaleX * centerX), centerY - (scaleY * centerY));
}


}; // namespace AVUI
