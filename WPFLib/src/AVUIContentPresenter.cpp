#include <AVUICommon.h>
#include <AVUIContentPresenter.h>
#include <AVUIContentControl.h>
#include <AVUITextBlock.h>
#include <AVUITemplateBinding.h>
#include <AVUIDataTemplate.h>
#include <AVUIDataTemplateSelector.h>
#include <AVUIBindingOperations.h>
#include <AVUIHelper.h>
#include <AVUIStyleHelper.h>

REGISTER_ELEMENT_BEGIN(AVUI::ContentPresenter)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Content)
    REGISTER_PROPERTY(ContentTemplate)
    REGISTER_PROPERTY(ContentTemplateSelector)
    REGISTER_PROPERTY(ContentSource)
    // Template is internal only property
REGISTER_ELEMENT_END()

namespace AVUI {

ContentPresenter::ContentPresenter() : m_pCachedChild(NULL), m_contentIsItem(false), m_templateIsCurrent(false)
{
    m_pTextPresenter = object_allocate<TextBlock>();
}

ContentPresenter::~ContentPresenter()
{
    RemoveCachedChild();
}

void ContentPresenter::BindToBasicFontProperties(DependencyObject* pFrom, DependencyObject* pTo)
{
    BindProperty(pFrom, Control::get_FontSizeProperty(),
                 pTo, Control::get_FontSizeProperty());

    BindProperty(pFrom, Control::get_FontFamilyProperty(),
                 pTo, Control::get_FontFamilyProperty());
}


void ContentPresenter::BindToFontProperties(DependencyObject* pFrom, DependencyObject* pTo)
{
    if(pFrom != NULL && pFrom->IsType(GET_TYPE(Control)))
    {
        BindToBasicFontProperties(pFrom, pTo);
        BindProperty(pFrom, Control::get_ForegroundProperty(),
                     pTo, Control::get_ForegroundProperty());
    }
}

void ContentPresenter::BindTextPresenterToFontProperties(DependencyObject* pFrom)
{
    BindToFontProperties(pFrom, m_pTextPresenter);
}


void ContentPresenter::OnTemplatedParentSet()
{
    DependencyObject* pDOCreator = get_TemplatedParent();

    BindToFontProperties(pDOCreator, m_pTextPresenter);
}

void ContentPresenter::EnsureTemplate()
{
    DataTemplate* pOldTemplate = get_Template();
    TRefCountedPtr<DataTemplate> pNewTemplate = ChooseTemplate();
    if (pOldTemplate != pNewTemplate)
    {
        set_Template(NULL);
    }

    Object* pContent = get_Content();

    if(pNewTemplate != NULL && pContent != NULL && !pContent->IsType(GET_TYPE(UIElement)))
    {
        set_DataContext(pContent);
    }
    else
    {
        ClearValue(get_DataContextProperty());
    }

    if(pNewTemplate != NULL && m_pCachedChild != NULL)
    {
        RemoveVisualChild(m_pCachedChild);
        m_pCachedChild = NULL;
    }

    set_Template(pNewTemplate);

    /*
    if (oldFrameworkTemplate == template2)
    {
        StyleHelper.DoTemplateInvalidations(this, oldFrameworkTemplate);
    }
    */
}

TRefCountedPtr<DataTemplate> ContentPresenter::ChooseTemplate()
{
    TRefCountedPtr<DataTemplate> pContentTemplate;

    Object* pItem = get_Content();
    
    pContentTemplate = get_ContentTemplate();
    if (pContentTemplate == NULL && get_ContentTemplateSelector() != NULL)
    {
        pContentTemplate = get_ContentTemplateSelector()->SelectTemplate(pItem, this);
    }

    /*
    if (contentTemplate == null)
    {
        contentTemplate = DefaultTemplateSelector.SelectTemplate(item, this);
    }
    */

    return pContentTemplate;
}

 

 


void ContentPresenter::BindProperty(DependencyObject* pFrom, const DependencyProperty& dpFrom,
                                    DependencyObject* pTo, const DependencyProperty& dpTo)
{
    bool flag;
    BaseValueSource::Enum valueSource = pTo->GetValueSource(dpTo, NULL, &flag);
    if (valueSource != BaseValueSource::Default)
    {
        return;
    }


    TemplateBindingPtr pExpression = object_allocate<TemplateBinding>();

    pExpression->set_SourceObject(pFrom);
    pExpression->set_SourceProperty(&dpFrom);

    pTo->SetValue(dpTo, pExpression);
}



void ContentPresenter::OnContentChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ContentPresenter* pCP = (ContentPresenter*) pDO;
    if(pCP->m_pCachedChild != NULL)
    {
        pCP->RemoveVisualChild(pCP->m_pCachedChild);
        pCP->m_pCachedChild = NULL;
    }

    if(pCP->get_Template() == NULL)
    {
        if(e.pNewValue != NULL)
        {
            if(e.pNewValue->IsType(GET_TYPE(String)))
            {
                pCP->m_pCachedChild = pCP->m_pTextPresenter;
                pCP->m_pTextPresenter->set_Text((String*) e.pNewValue);
            }
            else if(e.pNewValue->IsType(GET_TYPE(UIElement)))
            {
                pCP->m_pCachedChild = object_cast<UIElement>(e.pNewValue);
            }
            else
            {
                pCP->set_DataContext(e.pNewValue);
            }

            if(pCP->m_pCachedChild != NULL)
            {
                Visual* pVisualParent = pCP->m_pCachedChild->get_VisualParent();
                if(pVisualParent != NULL && pVisualParent->IsType(GET_TYPE(ContentPresenter)))
                {
                    ((ContentPresenter*)pVisualParent)->RemoveCachedChild();
                }
                pCP->AddVisualChild(pCP->m_pCachedChild);
            }

        }
    }
}

void ContentPresenter::RemoveCachedChild()
{
    if(m_pCachedChild != NULL)
    {
        RemoveVisualChild(m_pCachedChild);
        m_pCachedChild = NULL;
    }
}

Size ContentPresenter::MeasureOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Measure(constraintSize);
            return pVisualChild->get_DesiredSize();
        }
    }
    return Size(0, 0);
}

Size ContentPresenter::ArrangeOverride(const Size& constraintSize)
{
    if(get_VisualChildCount() > 0)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Arrange(Rect(constraintSize));
        }
    }
    return constraintSize;
}


unsigned int ContentPresenter::get_VisualChildCount()
{
    if(m_pCachedChild != NULL)
    {
        return 1;
    }

    return UIElement::get_VisualChildCount();
}

Visual* ContentPresenter::get_VisualChild(unsigned int idx)
{
    if(m_pCachedChild != NULL)
    {
        return m_pCachedChild;
    }
    return UIElement::get_VisualChild(idx);
}


void ContentPresenter::PrepareContentPresenter(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector)
{
    if (pItem != this)
    {
        if (m_contentIsItem || !HasNonDefaultValue(get_ContentProperty()))
        {
            set_Content(pItem);
            m_contentIsItem = true;
        }
        if (pItemTemplate != NULL)
        {
            SetValue(get_ContentTemplateProperty(), pItemTemplate);
        }
        if (pItemTemplateSelector != NULL)
        {
            SetValue(get_ContentTemplateSelectorProperty(), pItemTemplateSelector);
        }
    }
}

 
void ContentPresenter::OnContentTemplateChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ContentPresenter* pContentPresenter = (ContentPresenter*) pDO;
    pContentPresenter->set_Template(NULL);
    pContentPresenter->m_templateIsCurrent = false;
}

void ContentPresenter::OnContentTemplateSelectorChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ContentPresenter* pContentPresenter = (ContentPresenter*) pDO;
    pContentPresenter->set_Template(NULL);
    pContentPresenter->m_templateIsCurrent = false;
}

void ContentPresenter::OnContentSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ContentPresenter* pCP = (ContentPresenter*) pDO;
    String* pContentSource = (String*) e.pNewValue;

    if(pContentSource != NULL)
    {
        const Property* pProperty = pCP->get_TemplatedParent()->get_Type().get_Property(*pContentSource);

        pCP->ClearValue(ContentPresenter::get_ContentProperty());

        pCP->BindProperty(pCP->get_TemplatedParent(), *(DependencyProperty*)pProperty,
                     pCP, ContentPresenter::get_ContentProperty());
    }
    else
    {
        pCP->BindProperty(pCP->get_TemplatedParent(), ContentControl::get_ContentProperty(),
                     pCP, ContentPresenter::get_ContentProperty());
    }
}


void ContentPresenter::OnPreApplyTemplate()
{
    UIElement::OnPreApplyTemplate();

    if (get_TemplatedParent() == NULL)
    {
        InvalidateProperty(get_ContentProperty());
    }
    if (!m_templateIsCurrent)
    {
        if(get_TemplatedParent() != NULL && get_TemplatedParent()->IsType(GET_TYPE(ContentControl)))
        {
            Expression* pExpression = StyleHelper::GetExpression(this, &get_ContentProperty());

            if(pExpression == NULL)
            {
                BindProperty(get_TemplatedParent(), ContentControl::get_ContentProperty(), this, get_ContentProperty());
            }
        }

        EnsureTemplate();
        m_templateIsCurrent = true;
    }
}

 

 
 



}; // namespace AVUI
