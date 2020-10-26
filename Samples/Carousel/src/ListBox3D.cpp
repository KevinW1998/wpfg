#include <AVUICommon.h>
#include <AVUIControlTemplate.h>
#include <AVUIModelVisual3D.h>
#include <AVUIItemCollection.h>
#include <AVUIRotateTransform3D.h>
#include <AVUIAxisAngleRotation3D.h>
#include <AVUIQuaternionRotation3D.h>
#include <AVUITranslateTransform3D.h>
#include <AVUITransform3DGroup.h>
#include <AVUIQuaternionAnimation.h>
#include <ListBox3D.h>
#include <ListBox3DItem.h>

REGISTER_ELEMENT_BEGIN(ListBox3D)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(IsZoomedIn)

REGISTER_ELEMENT_END()


using namespace AVUI;


bool ListBox3D::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem->IsType(GET_TYPE(ListBox3DItem));
}

TRefCountedPtr<DependencyObject> ListBox3D::GetContainerForItemOverride()
{
    return (DependencyObject*) object_allocate<ListBox3DItem>();
}

void ListBox3D::OnApplyTemplate()
{
    ListBox::OnApplyTemplate();

    ControlTemplate* pTemplate = get_Template();

    m_pItemsGroup = object_cast<ModelVisual3D>(pTemplate->FindName(String(L"PART_ItemsHost"),  this));

    if(m_pItemsGroup == NULL)
    {
        return;
    }

    m_pItemsGroup->set_Transform(object_allocate<RotateTransform3D>(object_allocate<QuaternionRotation3D>(Quaternion(Vector3D(0.0f, 1.0f, 0.0f), 0.0f))));

    ItemCollection* pItems = get_Items();

    for(int idx = 0; idx < pItems->get_Count(); idx++)
    {
        ListBox3DItem* pItem = object_cast<ListBox3DItem>(pItems->GetItemAt(idx));

        TRefCountedPtr<Transform3DGroup> pTransforms = object_allocate<Transform3DGroup>();

        pTransforms->get_Children()->Add(object_allocate<TranslateTransform3D>(0.0f, 0.0f, -4.0f));
        pTransforms->get_Children()->Add(object_allocate<RotateTransform3D>(object_allocate<AxisAngleRotation3D>(Vector3D(0.0f, 1.0f, 0.0f), idx * (360.0f / pItems->get_Count()))));

        TRefCountedPtr<ModelVisual3D> pVisual = pItem->GenerateModel();

        pVisual->set_Transform(pTransforms);

        m_pItemsGroup->get_Children()->Add(pVisual);
    }

    set_SelectedIndex(0);
}


void ListBox3D::OnSelectionChanged(AVUI::SelectionChangedEventArgs* pArgs)
{
    float newAngle = -(get_SelectedIndex() * (360.0f / get_Items()->get_Count()));

    RotateModel(0.0f, newAngle, 500, false);
}

void ListBox3D::OnKeyDown(AVUI::KeyEventArgs* pArgs)
{
    int selItem = get_SelectedIndex();
    if(pArgs->get_Key() == Key::Left)
    {
        set_SelectedIndex(selItem > 0 ? selItem - 1 : get_Items()->get_Count() - 1);
    }

    if(pArgs->get_Key() == Key::Right)
    {
        set_SelectedIndex(selItem < get_Items()->get_Count() - 1 ? selItem + 1 : 0);
    }

    if(pArgs->get_Key() == Key::Enter)
    {
        set_IsZoomedIn(!get_IsZoomedIn());
    }

}

void ListBox3D::RotateModel(float start, float end, int duration, bool useStart)
{
   RotateTransform3D* pTransform = object_cast<RotateTransform3D>(m_pItemsGroup->get_Transform());
   Rotation3D* pRotation = pTransform->get_Rotation();

   TRefCountedPtr<QuaternionAnimation> pAnim = object_allocate<QuaternionAnimation>();

   if(useStart)
   {
       pAnim->set_From(Quaternion(Vector3D(0.0f, 1.0f, 0.0f), start));
   }

   pAnim->set_To(Quaternion(Vector3D(0.0f, 1.0f, 0.0f), end));
   pAnim->set_AccelerationRatio(0.1f);
   pAnim->set_DecelerationRatio(0.6f);
   pAnim->set_Duration(TimeSpan::FromMilliseconds(duration));

   pRotation->BeginAnimation(QuaternionRotation3D::get_QuaternionProperty(), pAnim, HandoffBehavior::SnapshotAndReplace);
}

/*
public void OnRotateEnded(object sender, EventArgs args)
{
   if (sender == null)
      return;
   Clock clock = sender as Clock;
   if (clock == null)
      return;
   if (clock.CurrentState == ClockState.Filling)
   {
      if (this.IsAutoRotating)
      {
         if (this.AutoRotationState == (int)ExpeditionCarousel3DRotationStates.ShowcaseRotate)
         {
            this.RotateToNextItem();
         }
         else
         {
            this.ShowcaseItem();
         }
      }
      clock.CurrentStateInvalidated -= new EventHandler(this.OnRotateEnded);
      clock = null;
   }
}
*/
