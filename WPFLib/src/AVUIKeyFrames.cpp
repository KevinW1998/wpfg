#include <AVUICommon.h>
#include <AVUIKeyFrames.h>

REGISTER_ELEMENT_BEGIN(AVUI::SingleKeyFrame)

    REGISTER_PROPERTY(Value);
    REGISTER_PROPERTY(KeyTime);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::DoubleKeyFrame)

    REGISTER_PROPERTY(Value);
    REGISTER_PROPERTY(KeyTime);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::PointKeyFrame)

    REGISTER_PROPERTY(Value);
    REGISTER_PROPERTY(KeyTime);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::ColorKeyFrame)

    REGISTER_PROPERTY(Value);
    REGISTER_PROPERTY(KeyTime);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Vector3DKeyFrame)

    REGISTER_PROPERTY(Value);
    REGISTER_PROPERTY(KeyTime);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Point3DKeyFrame)

    REGISTER_PROPERTY(Value);
    REGISTER_PROPERTY(KeyTime);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Rotation3DKeyFrame)

    REGISTER_PROPERTY(Value);
    REGISTER_PROPERTY(KeyTime);
REGISTER_ELEMENT_END()


namespace AVUI {


}; // namespace AVUI