#include <AVUICommon.h>
#include <AVUITransformGroup.h>
#include <AVUITransformCollection.h>
#include <AVUIMatrix.h>

REGISTER_ELEMENT_BEGIN(AVUI::TransformGroup)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Children);
REGISTER_ELEMENT_END()

namespace AVUI {

TransformGroup::TransformGroup()
{
    m_pChildren = object_allocate<TransformCollection>();
}

void TransformGroup::CloneCore(Animatable* pSource)
{
    TransformGroup* pTransformSource = (TransformGroup*) pSource;

    TransformCollection* pTransforms = pTransformSource->get_Children();

    for(int idx = 0; idx < pTransforms->get_Count(); idx++)
    {
        get_Children()->Add((*pTransforms)[idx]->Clone());
    }

    Animatable::CloneCore(pSource);
}


Matrix TransformGroup::get_Value()
{
    TransformCollection* pTransforms = get_Children();
    if (pTransforms == NULL || pTransforms->get_Count() == 0)
    {
          return Matrix();
    }

    TransformCollection& childTransforms = *(pTransforms);
    Matrix matrix = childTransforms[0]->get_Value();

    for(int index = 1; index < pTransforms->get_Count(); index++)
    {
        matrix = matrix * childTransforms[index]->get_Value();
    }


    return matrix;
}

}; // namespace AVUI
