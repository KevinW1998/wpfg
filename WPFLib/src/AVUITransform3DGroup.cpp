#include <AVUICommon.h>
#include <AVUITransform3DGroup.h>
#include <AVUIMatrix3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::Transform3DGroup)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Children);
REGISTER_ELEMENT_END()

namespace AVUI {

Transform3DGroup::Transform3DGroup()
{
}

void Transform3DGroup::CloneCore(Animatable* pSource)
{
    Transform3DGroup* pTransformSource = (Transform3DGroup*) pSource;

    Transform3DCollection* pTransforms = pTransformSource->get_Children();

    for(int idx = 0; idx < pTransforms->get_Count(); idx++)
    {
        get_Children()->Add((*pTransforms)[idx]->Clone());
    }

    Animatable::CloneCore(pSource);
}


Matrix3D Transform3DGroup::get_Value()
{
    Transform3DCollection* pTransforms = get_Children();
    if (pTransforms == NULL || pTransforms->get_Count() == 0)
    {
          return Matrix3D();
    }

    Transform3DCollection& childTransforms = *(pTransforms);
    Matrix3D matrix = childTransforms[0]->get_Value();

    for(int index = 1; index < pTransforms->get_Count(); index++)
    {
        matrix = matrix * childTransforms[index]->get_Value();
    }


    return matrix;
}

}; // namespace AVUI
