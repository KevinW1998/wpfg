#include <AVUICommon.h>
#include <AVUIAnimatedTypeHelpers.h>
#include <AVUIRotation3D.h>
#include <AVUIQuaternionRotation3D.h>

namespace AVUI {


template<>
TRefCountedPtr<Rotation3D> AnimatedReferenceTypeHelpers::Add(Rotation3D* pOne, Rotation3D* pTwo)
{
    if (pOne == NULL)
    {
        pOne = Rotation3D::get_Identity();
    }
    if (pTwo == NULL)
    {
        pTwo = Rotation3D::get_Identity();
    }
    return (Rotation3D*)object_allocate<QuaternionRotation3D>(AnimatedTypeHelpers::Add(pOne->get_InternalQuaternion(), 
                                                                          pTwo->get_InternalQuaternion()));

}

template<>
TRefCountedPtr<Rotation3D> AnimatedReferenceTypeHelpers::Subtract(Rotation3D* pOne, Rotation3D* pTwo)
{
    return (Rotation3D*)object_allocate<QuaternionRotation3D>(AnimatedTypeHelpers::Subtract(pOne->get_InternalQuaternion(), 
                                                                               pTwo->get_InternalQuaternion()));
}

template<>
TRefCountedPtr<Rotation3D> AnimatedReferenceTypeHelpers::Scale(Rotation3D* pOne, float scale)
{
    return (Rotation3D*)object_allocate<QuaternionRotation3D>(AnimatedTypeHelpers::Scale(pOne->get_InternalQuaternion(), 
                                                                               scale));
}

template<>
TRefCountedPtr<Rotation3D> AnimatedReferenceTypeHelpers::Interpolate(Rotation3D* pFrom, Rotation3D* pTo, float progress)
{
    return (Rotation3D*)object_allocate<QuaternionRotation3D>(AnimatedTypeHelpers::Interpolate(pFrom->get_InternalQuaternion(), 
                                                                               pTo->get_InternalQuaternion(),
                                                                               progress));

}

template <> 
TRefCountedPtr<Rotation3D> AnimatedReferenceTypeHelpers::GetZeroValue(Rotation3D*) 
{ 
    return Rotation3D::get_Identity(); 
}



}; // namespace AVUI


