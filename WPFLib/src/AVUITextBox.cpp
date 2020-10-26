#include <AVUICommon.h>
#include <AVUIControlTemplate.h>
#include <AVUIScrollViewer.h>
#include <AVUITextBoxView.h>
#include <AVUITextBox.h>
#include <AVUITemplateBinding.h>
#include <AVUIInputManager.h>
#include <AVUIDecorator.h>

REGISTER_ELEMENT_BEGIN(AVUI::TextBox)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Text);
    REGISTER_PROPERTY(TextAlignment);
    REGISTER_PROPERTY(TextWrapping);
    REGISTER_PROPERTY(IsReadOnly);
REGISTER_ELEMENT_END()


namespace AVUI {

void TextBox::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    ControlTemplate* pTemplate = get_Template();
    if(pTemplate != NULL)
    {

        m_pTextBoxView = object_allocate<TextBoxView>();

        BindProperty(m_pTextBoxView, get_FontSizeProperty(), TextBoxView::get_FontSizeProperty());
        BindProperty(m_pTextBoxView, get_FontFamilyProperty(), TextBoxView::get_FontFamilyProperty());
        BindProperty(m_pTextBoxView, get_ForegroundProperty(), TextBoxView::get_ForegroundProperty());
        BindProperty(m_pTextBoxView, get_TextProperty(), TextBoxView::get_TextProperty());
        m_pTextBoxView->set_HorizontalAlignment(HorizontalAlignment::Left);
        m_pTextBoxView->set_VerticalAlignment(VerticalAlignment::Center);

        UIElement* pContentHost = object_cast<UIElement>(pTemplate->FindName(String(L"PART_ContentHost"),  this));

        if(pContentHost == NULL)
        {
            LibraryCriticalError();
        }

        if(pContentHost->IsType(GET_TYPE(ScrollViewer)))
        {
            ((ScrollViewer*)pContentHost)->set_Content(m_pTextBoxView);
        }
        else if(pContentHost->IsType(GET_TYPE(Decorator)))
        {
            ((Decorator*)pContentHost)->set_Child(m_pTextBoxView);
        }
    }

    if(m_pTextBoxView != NULL)
    {
        m_pTextBoxView->set_TextBox(this);
    }

    if(get_Text() == NULL)
    {
        set_Text(object_allocate<String>(L""));
    }

}

void TextBox::BindProperty(DependencyObject* pDO, const DependencyProperty& dpFrom, const DependencyProperty& dpTo)
{
    TRefCountedPtr<TemplateBinding> pBinding = object_allocate<TemplateBinding>();

    pBinding->set_SourceObject(this);
    pBinding->set_SourceProperty(&dpFrom);

    pDO->SetValue(dpTo, pBinding);
}

void TextBox::OnMouseDown(MouseButtonEventArgs* pArgs)
{
    Focus();

    if(m_pTextBoxView != NULL)
    {
        Point point = get_InputManager()->get_MousePosition(this);
        set_CaretIndex(m_pTextBoxView->get_CaretIndexFromPoint(point));
    }


    Control::OnMouseDown(pArgs);
}

void TextBox::set_CaretIndex(int caretIndex)
{
    m_caretIndex = caretIndex;
    if(m_pTextBoxView != NULL)
    {
        m_pTextBoxView->set_CaretIndex(caretIndex);
    }
}

void TextBox::OnGotFocus(RoutedEventArgs* pArgs)
{
    if(m_pTextBoxView != NULL)
    {
        m_pTextBoxView->ShowCaret();
    }
    UIElement::OnGotFocus(pArgs);
}

void TextBox::OnLostFocus(RoutedEventArgs* pArgs)
{
    if(m_pTextBoxView != NULL)
    {
        m_pTextBoxView->HideCaret();
    }
    UIElement::OnLostFocus(pArgs);
}

void TextBox::OnKeyDown(KeyEventArgs* pArgs)
{
    if(pArgs->get_IsHandled())
    {
        return;
    }

    bool isHandled = false;

    if(pArgs->get_Key() == Key::Right)
    {
        set_CaretIndex(Int::Min(m_caretIndex + 1, get_Text()->get_Cch()));
        isHandled = true;
    }
    else if(pArgs->get_Key() == Key::Left)
    {
        set_CaretIndex(Int::Max(m_caretIndex - 1, 0));
        isHandled = true;
    }
    else if(pArgs->get_Key() == Key::End)
    {
        set_CaretIndex(get_Text()->get_Cch());
        isHandled = true;
    }
    else if(pArgs->get_Key() == Key::Home)
    {
        set_CaretIndex(0);
        isHandled = true;
    }
    else if(pArgs->get_Key() == Key::Delete)
    {
        String* pString = get_Text();
        if(m_caretIndex != pString->get_Cch())
        {
            get_Text()->DeleteAt(m_caretIndex);

            InvalidateSubProperty(get_TextProperty());

            if(m_pTextBoxView != NULL)
            {
                m_pTextBoxView->InvalidateSubProperty(TextBoxView::get_TextProperty());
            }

            RoutedEventArgs args(&(get_TextChangedEvent()));
            RaiseEvent(&args);
        }
        isHandled = true;
    }

    if(isHandled)
    {
        pArgs->set_IsHandled(true);
    }
}

void TextBox::OnKeyUp(KeyEventArgs* pArgs)
{

}
void TextBox::OnKeyChar(KeyCharEventArgs* pArgs)
{
    if(pArgs->get_IsHandled())
    {
        return;
    }

    bool isChanged = false;

    if(pArgs->get_Key() == Key::Back)
    {
        if(m_caretIndex != 0)
        {
            get_Text()->DeleteAt(m_caretIndex - 1);
            set_CaretIndex(m_caretIndex - 1);

            isChanged = true;
        }
    }
    else if(pArgs->get_Key() != Key::Enter)
    {
        get_Text()->InsertAt(m_caretIndex, pArgs->get_Char());
        set_CaretIndex(m_caretIndex+1);

        isChanged = true;
    }

    if(isChanged)
    {
        InvalidateSubProperty(get_TextProperty());

        RoutedEventArgs args(&(get_TextChangedEvent()));
        RaiseEvent(&args);

        pArgs->set_IsHandled(true);
    }
}


void TextBox::OnTextChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    TextBox* pTextBox = ((TextBox*)pDO);
    int maxCch = 0;

    if(e.pNewValue != NULL)
    {
        maxCch = ((String*) e.pNewValue)->get_Cch();
    }
    else
    {
        pTextBox->set_Text(object_allocate<String>(""));
    }

    pTextBox->set_CaretIndex(Int::Min(pTextBox->m_caretIndex, maxCch));
}



}; // namespace AVUI
