#pragma once

#include <AVUIListBox.h>

namespace AVUI
{
    class ModelVisual3D;
}
                        
class ListBox3D : public AVUI::ListBox
{
    friend class ListBox3DItem;    
public:
    ListBox3D() : m_pItemsGroup(NULL) { };
    ~ListBox3D() { };

    DECLARE_ELEMENT(ListBox3D, AVUI::ListBox);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsZoomedIn, bool, false, AVUI::PropertyFlag::None);

protected:

    virtual void OnApplyTemplate();

    virtual bool IsItemItsOwnContainerOverride(AVUI::Object* pItem);
    virtual AVUI::TRefCountedPtr<AVUI::DependencyObject> GetContainerForItemOverride();

    virtual void OnSelectionChanged(AVUI::SelectionChangedEventArgs* pArgs);
    virtual void OnKeyDown(AVUI::KeyEventArgs* pArgs);

private:

    void RotateModel(float start, float end, int duration, bool useStart);

    AVUI::ModelVisual3D* m_pItemsGroup;
};



