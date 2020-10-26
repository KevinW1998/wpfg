#include <AVUICommon.h>
#include <AVUIPerspectiveCamera.h>

REGISTER_ELEMENT_BEGIN(AVUI::PerspectiveCamera)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(FieldOfView)
REGISTER_ELEMENT_END()


namespace AVUI {

#include <math.h>

Matrix3D PerspectiveCamera::GetProjectionMatrix(float aspectRatio)
{
    return GetProjectionMatrix(aspectRatio, get_NearPlaneDistance(), get_FarPlaneDistance());
}

Matrix3D PerspectiveCamera::GetProjectionMatrix(float aspectRatio, float zn, float zf)
{
    float num2 = tanf(M3DUtil::DegreesToRadians(get_FieldOfView()) / 2.0f);
    float num5 = aspectRatio / num2;
    float num4 = 1.0f / num2;
    float num = (!Float::IsPositiveInfinity(zf)) ? (zf / (zn - zf)) : -1;
    return Matrix3D(num4, 0, 0, 0, 
                    0, num5, 0, 0,
                    0, 0, num, -1, 
                    0, 0, zn * num, 0);
}


}; // namespace AVUI
