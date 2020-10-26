#include <AVUICommon.h>
#include <AVUIMatrixTransform.h>

REGISTER_ELEMENT_BEGIN(AVUI::MatrixTransform)

REGISTER_ELEMENT_END()

namespace AVUI {

MatrixTransform::MatrixTransform(const Matrix& matrix) : m_matrix(matrix)
{

}

Matrix MatrixTransform::get_Value()
{
    return m_matrix;
}


}; // namespace AVUI
