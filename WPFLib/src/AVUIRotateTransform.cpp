#include <AVUICommon.h>
#include <AVUIRotateTransform.h>
#include <AVUIMatrix.h>
#include <math.h>

REGISTER_ELEMENT_BEGIN(AVUI::RotateTransform)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Angle);
    REGISTER_PROPERTY(CenterX);
    REGISTER_PROPERTY(CenterY);
REGISTER_ELEMENT_END()

namespace AVUI {

Matrix RotateTransform::get_Value()
{
    float centerX = get_CenterX();
    float centerY = get_CenterY();
    float angle = fmodf(get_Angle(),360.0f) *  0.017453292519943295f;

    float sinValue = sinf(angle);
    float cosValue = cosf(angle);
    float offsetX = (centerX * (1.0f - cosValue)) + (centerY * sinValue);
    float offsetY = (centerY * (1 - cosValue)) - (centerX * sinValue);

    return Matrix(cosValue, sinValue, -sinValue, cosValue, offsetX, offsetY);
}


}; // namespace AVUI
