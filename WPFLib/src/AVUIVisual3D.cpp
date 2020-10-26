#include <AVUICommon.h>
#include <AVUIVisual3D.h>
#include <AVUIVisual3DCollection.h>
#include <AVUIAnimationStorage.h>
#include <AVUIDeferredReference.h>


REGISTER_ELEMENT_BEGIN(AVUI::Visual3DCollection)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::Visual3D)

REGISTER_ELEMENT_END()



namespace AVUI {


void Visual3D::BeginAnimation(const DependencyProperty& dp, AnimationTimeline* pTimeline, HandoffBehavior::Enum handoffBehavior)
{
    AnimationStorage::BeginAnimation(this, dp, pTimeline, handoffBehavior);
}


void Visual3D::EvaluateAnimatedValueCore(const DependencyProperty& dp, DependencyPropertyMetadata* pMetadata, EffectiveValueEntry* pEntry)
{
    if (get_HasAnimatedProperties())
    {
        AnimationStorage* pStorage = AnimationStorage::GetStorage(this, dp);

        if (pStorage != NULL)
        {
            ObjectPtr pAnimationBaseValue = pEntry->GetFlattenedEntry(RequestFlags::FullyResolved).get_Value();
            if (pEntry->get_IsDeferredReference())
            {
                pAnimationBaseValue = ((DeferredReference*) (Object*) pAnimationBaseValue)->GetValue(pEntry->get_BaseValueSource());
                pEntry->SetAnimationBaseValue(pAnimationBaseValue);
                pEntry->set_IsDeferredReference(false);
            }
            ObjectPtr pAnimatedValue = AnimationStorage::GetCurrentPropertyValue(pStorage, this, dp, pMetadata, pAnimationBaseValue);
            pEntry->SetAnimatedValue(pAnimatedValue, pAnimationBaseValue);
        }
    }
}

DependencyObject* Visual3D::get_InheritanceContext()
{
    return m_pInheritanceContext;
}
void Visual3D::AddInheritanceContext(DependencyObject* pDO, const DependencyProperty* pDP)
{
    if(m_pInheritanceContext != pDO)
    {
        m_pInheritanceContext = pDO;
    }
}

void Visual3D::RemoveInheritanceContext(DependencyObject* pDO, const DependencyProperty* pDP)
{
    if(m_pInheritanceContext == pDO)
    {
        m_pInheritanceContext = NULL;
    }
}



}; // namespace AVUI
