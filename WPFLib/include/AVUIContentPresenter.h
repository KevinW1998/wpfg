#pragma once

#include <AVUIFrameworkElement.h>

namespace AVUI {

class TextBlock;
class DataTemplate;
class DataTemplateSelector;

class ContentPresenter : public FrameworkElement
{
    friend class ItemsControl;
public:
    ContentPresenter();
    ~ContentPresenter();

    DECLARE_ELEMENT(ContentPresenter, FrameworkElement);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Content, Object, NULL, DependencyPropertyMetadata(PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure, ContentPresenter::OnContentChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ContentTemplate, DataTemplate, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, ContentPresenter::OnContentTemplateChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ContentTemplateSelector, DataTemplateSelector, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, ContentPresenter::OnContentTemplateSelectorChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ContentSource, String, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, ContentPresenter::OnContentSourceChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Template, DataTemplate, NULL, PropertyFlag::AffectsMeasure);

    virtual unsigned int get_VisualChildCount();
    virtual Visual* get_VisualChild(unsigned int);

    static void BindToFontProperties(DependencyObject* pFrom, DependencyObject* pTo);
    static void BindToBasicFontProperties(DependencyObject* pFrom, DependencyObject* pTo);
    void BindTextPresenterToFontProperties(DependencyObject* pFrom);

protected:

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    static void BindProperty(DependencyObject* pFrom, const DependencyProperty& dpFrom,
                             DependencyObject* pTo, const DependencyProperty& dpTo);

    virtual FrameworkTemplate* get_TemplateInternal() { return (FrameworkTemplate*) get_Template(); }
    virtual void OnPreApplyTemplate();
    virtual void OnTemplatedParentSet();

    virtual TRefCountedPtr<DataTemplate> ChooseTemplate();

private:
    void EnsureTemplate();
    void RemoveCachedChild();
    static void OnContentChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnContentSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnContentTemplateChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnContentTemplateSelectorChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    void PrepareContentPresenter(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector);

    TWeakPtr<UIElement> m_pCachedChild;
    TRefCountedPtr<TextBlock> m_pTextPresenter;
    bool m_contentIsItem : 1;
    bool m_templateIsCurrent : 1;

};


}; // namespace AVUI

