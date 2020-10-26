#include <AVUICommon.h>
#include <AVUIMatrixTransform3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::MatrixTransform3D)

REGISTER_ELEMENT_END()


namespace AVUI {

MatrixTransform3D::MatrixTransform3D(const Matrix3D& matrix) : m_matrix(matrix)
{

}

Matrix3D MatrixTransform3D::get_Value()
{
    return m_matrix;
}


}; // namespace AVUI
