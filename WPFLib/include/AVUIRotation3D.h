#pragma once

#include <AVUIAnimatable.h>
#include <AVUIQuaternion.h>

namespace AVUI {

class Rotation3D : public Animatable
{
public:

    virtual Quaternion get_InternalQuaternion() = 0;
    DECLARE_ELEMENT(Rotation3D, Animatable);

    static Rotation3D* get_Identity();

    static void CleanupStaticAllocations();

protected:
    Rotation3D() { };
    virtual ~Rotation3D() { }

private:

    static TRefCountedPtr<Rotation3D> ms_pIdentity;
};


}; // namespace AVUI
