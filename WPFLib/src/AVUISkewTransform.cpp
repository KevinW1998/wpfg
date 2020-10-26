#include <AVUICommon.h>
#include <AVUISkewTransform.h>
#include <AVUIMatrix.h>

REGISTER_ELEMENT_BEGIN(AVUI::SkewTransform)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(AngleX);
    REGISTER_PROPERTY(AngleY);
    REGISTER_PROPERTY(CenterX);
    REGISTER_PROPERTY(CenterY);
REGISTER_ELEMENT_END()

namespace AVUI {

SkewTransform::SkewTransform(float angleX, float angleY)
{
    set_AngleX(angleX);
    set_AngleY(angleY);
}

SkewTransform::SkewTransform(float angleX, float angleY, float centerX, float centerY)
{
    set_AngleX(angleX);
    set_AngleY(angleY);
    set_CenterX(centerX);
    set_CenterY(centerY);
}


Matrix SkewTransform::get_Value()
{
    Matrix matrix;

    float angleX = get_AngleX();
    float angleY = get_AngleY();
    float centerX = get_CenterX();
    float centerY = get_CenterY();
    bool isOffset = (centerX != 0.0f) || (centerY != 0.0f);
    if (isOffset)
    {
        matrix.Translate(-centerX, -centerY);
    }

    matrix.Skew(angleX, angleY);

    if (isOffset)
    {
        matrix.Translate(centerX, centerY);
    }
    return matrix;
}


}; // namespace AVUI
