#include <AVUICommon.h>
#include <AVUITranslateTransform.h>
#include <AVUIMatrix.h>

REGISTER_ELEMENT_BEGIN(AVUI::TranslateTransform)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(X);
    REGISTER_PROPERTY(Y);
REGISTER_ELEMENT_END()

namespace AVUI {

TranslateTransform::TranslateTransform(float offsetX, float offsetY)
{
    set_X(offsetX);
    set_Y(offsetY);
}

Matrix TranslateTransform::get_Value()
{
    return Matrix(1.0f, 0.0f, 0.0f, 1.0f, get_X(), get_Y());
}


}; // namespace AVUI
