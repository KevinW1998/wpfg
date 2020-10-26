#pragma once

#include <AVUITextBoxBase.h>

namespace AVUI {

class TextBoxView;

class TextBox : public TextBoxBase
{
public:

    DECLARE_ELEMENT(TextBox, TextBoxBase);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Text, String, NULL, DependencyPropertyMetadata(PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure, TextBox::OnTextChanged));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(TextAlignment, TextAlignment, TextAlignment::Left, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(TextWrapping, TextWrapping, TextWrapping::NoWrap, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsReadOnly, bool, true, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ROUTED_EVENT(TextChanged, RoutedEventArgs, RoutingStrategy::Bubble);

    TextBox() : m_caretIndex(0) { }
    virtual ~TextBox() { };


    void set_CaretIndex(int caretIndex);

protected:
    virtual void OnApplyTemplate();
    virtual void OnMouseDown(MouseButtonEventArgs* pArgs);
    virtual void OnKeyDown(KeyEventArgs* pArgs);
    virtual void OnKeyUp(KeyEventArgs* pArgs);
    virtual void OnKeyChar(KeyCharEventArgs* pArgs);

    virtual void OnGotFocus(RoutedEventArgs* pArgs);
    virtual void OnLostFocus(RoutedEventArgs* pArgs);

private:

    void BindProperty(DependencyObject* pDO, const DependencyProperty& dpFrom, const DependencyProperty& dpTo);
    static void OnTextChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    TRefCountedPtr<TextBoxView> m_pTextBoxView;
    int m_caretIndex;
};


}; // namespace AVUI
