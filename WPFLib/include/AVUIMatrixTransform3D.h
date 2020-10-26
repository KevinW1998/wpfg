#include <AVUITransform3D.h>
#include <AVUIMatrix3D.h>

namespace AVUI {

class MatrixTransform3D : public Transform3D
{
public:
    MatrixTransform3D() { };
    MatrixTransform3D(const Matrix3D& matrix);

    ~MatrixTransform3D() { }

    virtual Matrix3D get_Value();

    DECLARE_ELEMENT(MatrixTransform3D, Transform3D);

private:
    Matrix3D m_matrix;
};

}; // namespace AVUI
