#include <AVUICommon.h>
#include <AVUITranslateTransform.h>
#include <AVUIDoubleAnimation.h>
#include <AnimatingScrollContentPresenter.h>

REGISTER_ELEMENT_BEGIN(AnimatingScrollContentPresenter)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()


using namespace AVUI;


Size AnimatingScrollContentPresenter::ArrangeOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() == 0)
    {
        return ScrollContentPresenter::ArrangeOverride(constraintSize);
    }

    Visual* pChild = get_VisualChild(0);

    Vector currentOffset;

    TRefCountedPtr<TranslateTransform> pTransform = object_cast<TranslateTransform>(pChild->get_VisualTransform());

    if(pTransform != NULL)
    {
        currentOffset = Vector(pTransform->get_X(), pTransform->get_Y());
    }

    pChild->set_VisualTransform(NULL);

    Size baseSize = ScrollContentPresenter::ArrangeOverride(constraintSize);

    pTransform = object_allocate<TranslateTransform>();
    pChild->set_VisualTransform(pTransform);

    Vector layoutOffset = pChild->get_VisualOffset();

    if(layoutOffset != m_lastContentOffset)
    {        
        TRefCountedPtr<DoubleAnimation> pAnim;

        if(m_lastContentOffset.get_X() != layoutOffset.get_X())
        {
            pAnim =  object_allocate<DoubleAnimation>();

            pTransform->set_X(currentOffset.get_X() + (m_lastContentOffset.get_X() - layoutOffset.get_X()));

            pAnim->set_To(0.0f);
            pAnim->set_AccelerationRatio(0.1f);
            pAnim->set_DecelerationRatio(0.6f);
            pAnim->set_Duration(TimeSpan::FromMilliseconds(300));
            pAnim->set_FillBehavior(FillBehavior::HoldEnd);

            pTransform->BeginAnimation(TranslateTransform::get_XProperty(), pAnim, HandoffBehavior::SnapshotAndReplace);
        }

        if(m_lastContentOffset.get_Y() != layoutOffset.get_Y())
        {
            pAnim = object_allocate<DoubleAnimation>();

            pTransform->set_Y(currentOffset.get_Y() + (m_lastContentOffset.get_Y() - layoutOffset.get_Y()));

            pAnim->set_To(0.0f);
            pAnim->set_AccelerationRatio(0.1f);
            pAnim->set_DecelerationRatio(0.6f);
            pAnim->set_Duration(TimeSpan::FromMilliseconds(300));
            pAnim->set_FillBehavior(FillBehavior::HoldEnd);

            pTransform->BeginAnimation(TranslateTransform::get_YProperty(), pAnim, HandoffBehavior::SnapshotAndReplace);

        }
        m_lastContentOffset = layoutOffset;
    }

    return baseSize;
}

bool AnimatingScrollContentPresenter::GetLayoutClip(const Size& layoutSlotSize, Rect* pRect)
{
    if(get_ClipToBounds())
    {
        *pRect = Rect(get_RenderSize());
        return true;
    }

    return false;
}

