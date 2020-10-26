
#pragma once

#include <AVUIListBox.h>

namespace AVUI {

class ViewBase;

class ListView : public ListBox
{

public:
    ListView();
    ~ListView() { };

    DECLARE_ELEMENT(ListView, ListBox);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(View, ViewBase, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, &ListView::OnViewChanged));

protected:

    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();
    virtual void PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem);

private:

    void ApplyNewView();
    static void OnViewChanged(Object* pDO, const DependencyPropertyChangedEventArgs& args);

    ViewBase* m_pPreviousView;

};

 


}; // namespace AVUI



