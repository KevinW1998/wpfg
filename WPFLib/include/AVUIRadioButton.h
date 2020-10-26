#pragma once

#include <AVUIToggleButton.h>

namespace AVUI {

class RadioButton;

class ControlTemplate;
class RadioButton : public ToggleButton
{
public:
    RadioButton() { };
    ~RadioButton();

    DECLARE_ELEMENT(RadioButton, ToggleButton);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(GroupName, String, NULL, DependencyPropertyMetadata(PropertyFlag::None, OnGroupNameChanged));

protected:
    virtual void OnToggle();
    virtual void OnChecked(RoutedEventArgs* pArgs);

private:

    struct GroupItem
    {
        TRefCountedPtr<String> GroupName;
        TRefCountedPtr<ItemStructList<RadioButton* > > GroupMembers;
    };

    static ItemStructList<GroupItem> m_registeredGroups;

    static void OnGroupNameChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    void UpdateRadioButtonGroup();
    static void AddToGroup(RadioButton* pButton, String* pGroup);
    static void RemoveFromGroup(RadioButton* pButton, String* pGroup);
    static GroupItem* GetItemForGroupName(String* pGroup);
};

}; // namespace AVUI
