#include <AVUITransform.h>
#include <AVUIMatrix.h>

namespace AVUI {

class MatrixTransform : public Transform
{
public:
    MatrixTransform() { };
    MatrixTransform(const Matrix& matrix);

    ~MatrixTransform() { }

    virtual Matrix get_Value();

    DECLARE_ELEMENT(MatrixTransform, Transform);

private:
    Matrix m_matrix;
};

}; // namespace AVUI
