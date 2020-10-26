#include <AVUICommon.h>
#include <AVUIRadioButton.h>

REGISTER_ELEMENT_BEGIN(AVUI::RadioButton)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(GroupName)
REGISTER_ELEMENT_END()

namespace AVUI
{

ItemStructList<RadioButton::GroupItem> RadioButton::m_registeredGroups;

RadioButton::~RadioButton()
{
    RemoveFromGroup(this, get_GroupName());
}


void RadioButton::OnToggle()
{
    set_IsChecked(true);
}

void RadioButton::OnChecked(RoutedEventArgs* pArgs)
{
    UpdateRadioButtonGroup();
    ToggleButton::OnChecked(pArgs);
}

void RadioButton::UpdateRadioButtonGroup()
{
    GroupItem* pGroupItem = GetItemForGroupName(get_GroupName());
    if(pGroupItem == NULL)
    {
        return;
    }

    for(int idx = 0; idx < pGroupItem->GroupMembers->get_Count(); idx++)
    {
        RadioButton* pButton = (*(pGroupItem->GroupMembers))[idx];
        if(pButton != this)
        {
            pButton->set_IsChecked(false);
        }
    }

}

void RadioButton::OnGroupNameChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    RemoveFromGroup((RadioButton*) pDO, (String*) e.pOldValue);
    AddToGroup((RadioButton*) pDO, (String*) e.pNewValue);
}

void RadioButton::AddToGroup(RadioButton* pButton, String* pGroup)
{
    if(pGroup == NULL)
    {
        return;
    }

    GroupItem* pGroupItem = GetItemForGroupName(pGroup);
    if(pGroupItem == NULL)
    {
        m_registeredGroups.AddNew();
        pGroupItem = &(m_registeredGroups[m_registeredGroups.get_Count() - 1]);
        pGroupItem->GroupName = pGroup;
        pGroupItem->GroupMembers = object_allocate< ItemStructList<RadioButton*> >();
    }

    pGroupItem->GroupMembers->AddItem(pButton);

}

void RadioButton::RemoveFromGroup(RadioButton* pButton, String* pGroup)
{
    if(pGroup == NULL)
    {
        return;
    }

    GroupItem* pGroupItem = GetItemForGroupName(pGroup);
    if(pGroupItem == NULL)
    {
        return;
    }

    pGroupItem->GroupMembers->Remove(pButton);
}

RadioButton::GroupItem* RadioButton::GetItemForGroupName(String* pGroup)
{
    if(pGroup == NULL)
    {
        return NULL;
    }

    for(int idx = 0; idx < m_registeredGroups.get_Count(); idx++)
    {
        if(m_registeredGroups[idx].GroupName->Equals(pGroup))
        {
            return &(m_registeredGroups[idx]);
        }
    }
    return NULL;
}


};


