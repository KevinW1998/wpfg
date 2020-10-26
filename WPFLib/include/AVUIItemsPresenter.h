#pragma once

#include <AVUIFrameworkElement.h>
    
namespace AVUI {

class ItemsControl;
class ItemContainerGenerator;
class ItemsPanelTemplate;
class Panel;

class ItemsPresenter : public FrameworkElement
{
public:
    ItemsPresenter() { };

    DECLARE_ELEMENT(ItemsPresenter, FrameworkElement);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Template, ItemsPanelTemplate, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, OnTemplateChangedStatic));

    static ItemsPresenter* FromPanel(Panel* pPanel);
    ItemsControl* get_Owner() { return m_pOwner; }
    ItemContainerGenerator* get_Generator() { return m_pGenerator; }
protected:

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    virtual void OnPreApplyTemplate();
    virtual void OnApplyTemplate();

    virtual FrameworkTemplate* get_TemplateInternal() { return (FrameworkTemplate*) get_Template(); }

private:
    void AttachToOwner();
    void UseGenerator(ItemContainerGenerator* pGenerator);

    // Update template cache?
    static void OnTemplateChangedStatic(Object* pDO, const DependencyPropertyChangedEventArgs& e) { }

    TRefCountedPtr<ItemContainerGenerator> m_pGenerator;
    TWeakPtr<ItemsControl> m_pOwner;
};



}; // namespace AVUI


