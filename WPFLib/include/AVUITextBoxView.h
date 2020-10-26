#pragma once
#include <AVUIFrameworkElement.h>

namespace AVUI {

class FormattedText;
class CaretElement;
class TextBox;

class TextBoxView : public FrameworkElement
{
    friend class TextBox;
public:

    DECLARE_ELEMENT(TextBoxView, FrameworkElement);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Text, String, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Foreground, Brush, NULL, PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(FontFamily, String, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(FontSize, float, 50.0f, PropertyFlag::AffectsMeasure);

    TextBoxView();
    virtual ~TextBoxView();


protected:

    virtual unsigned int get_VisualChildCount();
    virtual Visual* get_VisualChild(unsigned int idx);

private:

    void set_TextBox(TextBox* pTextBox) { m_pTextBox = pTextBox; }
    void ShowCaret();
    void HideCaret();
    void set_CaretIndex(int index);
    int get_CaretIndexFromPoint(const Point& point);

    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);
    void OnRender(DrawingContext& drawingContext);

    TRefCountedPtr<FormattedText> m_pFormattedText;
    bool m_isCaretVisible;
    TRefCountedPtr<CaretElement> m_pCaretElement;
    int m_caretIndex;
    TextBox* m_pTextBox;
};


}; // namespace AVUI
