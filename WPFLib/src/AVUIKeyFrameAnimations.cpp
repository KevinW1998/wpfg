#include <AVUICommon.h>
#include <AVUIKeyFrameAnimations.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::KeyTime)
    REGISTER_TYPE_CONVERTER(KeyTimeTypeConverter);
REGISTER_VALUE_TYPE_END()

REGISTER_ELEMENT_BEGIN(AVUI::SingleAnimationUsingKeyFrames)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
    REGISTER_PROPERTY(KeyFrames);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::DoubleAnimationUsingKeyFrames)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
    REGISTER_PROPERTY(KeyFrames);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::PointAnimationUsingKeyFrames)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
    REGISTER_PROPERTY(KeyFrames);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::ColorAnimationUsingKeyFrames)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
    REGISTER_PROPERTY(KeyFrames);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Vector3DAnimationUsingKeyFrames)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
    REGISTER_PROPERTY(KeyFrames);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Point3DAnimationUsingKeyFrames)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
    REGISTER_PROPERTY(KeyFrames);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Rotation3DAnimationUsingKeyFrames)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
    REGISTER_PROPERTY(KeyFrames);
REGISTER_ELEMENT_END()


namespace AVUI
{


ObjectPtr KeyTimeTypeConverter::ConvertFromString(const String* pString) const
{
    KeyTime keyTime;

    keyTime.m_type = KeyTimeType::TimeSpan;
    keyTime.m_timeSpanValue = UnboxValue<TimeSpan>(GET_TYPE(TimeSpan).get_TypeConverter()->ConvertFrom(pString));

    return BoxValue(keyTime);
}

ObjectPtr KeyTimeTypeConverter::ConvertToString(const KeyTime& value) const
{
    LibraryCriticalError();
    return NULL;
}



}; // end namespace AVUI

