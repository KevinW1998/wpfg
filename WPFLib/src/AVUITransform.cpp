#include <AVUICommon.h>
#include <AVUITransform.h>
#include <AVUIMatrixTransform.h>

REGISTER_ELEMENT_BEGIN(AVUI::Transform)

REGISTER_ELEMENT_END()

namespace AVUI {

const Transform* Transform::get_Identity()
{
    static MatrixTransform matrixTransform(Matrix::get_Identity());

    return &matrixTransform;
}



bool Transform::get_IsIdentity()
{
    if(get_Value() == Matrix::get_Identity())
    {
        return true;
    }

    return false;
}


}; // namespace AVUI
