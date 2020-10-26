#include <AVUICommon.h>
#include <AVUITransform3D.h>
#include <AVUIMatrixTransform3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::Transform3D)

REGISTER_ELEMENT_END()

namespace AVUI {

const Transform3D* Transform3D::get_Identity()
{
    static MatrixTransform3D matrixTransform(Matrix3D::CreateIdentity());

    return &matrixTransform;
}



bool Transform3D::get_IsIdentity()
{
    if(get_Value() == Matrix3D::CreateIdentity())
    {
        return true;
    }

    return false;
}


}; // namespace AVUI
