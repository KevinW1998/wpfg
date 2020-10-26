#include <AVUICommon.h>
#include <AVUIRotation3D.h>
#include <AVUIQuaternionRotation3D.h>
#include <AVUIMatrixTransform3D.h>
#include <AVUIMatrix3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::Rotation3D)

REGISTER_ELEMENT_END()

namespace AVUI {

TRefCountedPtr<Rotation3D> Rotation3D::ms_pIdentity;
    
Rotation3D* Rotation3D::get_Identity()
{
    if(ms_pIdentity == NULL)
    {
        ms_pIdentity = object_allocate<QuaternionRotation3D>(Quaternion::get_Identity());
    }

    return ms_pIdentity;
}

void Rotation3D::CleanupStaticAllocations()
{
    ms_pIdentity = NULL;
}

}; // namespace AVUI
