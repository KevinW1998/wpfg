#pragma once

#include <AVUIDependencyObject.h>
#include <AVUIINotifyPropertyChanged.h>

namespace AVUI {

class Style;
class DataTemplate;
class DataTemplateSelector;
class Binding;

class GridViewColumn : public DependencyObject, public INotifyPropertyChanged
{
    friend class GridViewColumnCollection;
    friend class GridViewRowPresenter;
    friend class GridViewHeaderRowPresenter;
public:
    GridViewColumn() : m_pInheritanceContext(NULL) 
    { 
        ResetPrivateData();
        m_state = Float::IsNaN(get_Width()) ? ColumnMeasureState::Init : ColumnMeasureState::SpecificWidth;
    };

    ~GridViewColumn() { };

    DECLARE_ELEMENT(GridViewColumn, DependencyObject);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Header, Object, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(HeaderContainerStyle, Style, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(HeaderTemplate, DataTemplate, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(HeaderTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(CellTemplate, DataTemplate, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(CellTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Width, float, Float::get_NaN(), DependencyPropertyMetadata(PropertyFlag::None, OnWidthChanged));


    IMPLEMENT_REFERENCE_PROPERTY(DisplayMemberBinding, Binding, false);
    IMPLEMENT_READONLY_VALUE_PROPERTY(ActualWidth, float, false);
    IMPLEMENT_READONLY_VALUE_PROPERTY(DesiredWidth, float, false);
    IMPLEMENT_EVENT(PropertyChanged, PropertyChangedEventArgs);

    virtual DependencyObject* get_InheritanceContext() { return m_pInheritanceContext; }
    virtual void AddInheritanceContext(DependencyObject* pContext, const DependencyProperty* pDP)
    {
        if(m_pInheritanceContext != pContext)
        {
            m_pInheritanceContext = pContext;
            // OnInheritanceContextChanged();
        }
    }
    virtual void RemoveInheritanceContext(DependencyObject* pContext, const DependencyProperty* pDP)
    {
        if (m_pInheritanceContext == pContext)
        {
            m_pInheritanceContext = NULL;
            // OnInheritanceContextChanged(EventArgs.Empty);
        }
    }

    float get_ActualWidth() { return m_actualWidth; }
    float get_DesiredWidth() { return m_desiredWidth; }

    Binding* get_DisplayMemberBinding() { return m_pDisplayMemberBinding; }
    void set_DisplayMemberBinding(Binding* pBinding) { m_pDisplayMemberBinding = pBinding; /* OnDisplayMemberBindingChanged */ }

protected:

    virtual void OnPropertyChanged(PropertyChangedEventArgs* pArgs);

 


private:

    void OnPropertyChanged(const Property* pProperty);

    static void OnWidthChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    void UpdateActualWidth();

    ColumnMeasureState::Enum get_State() { return m_state; }
    void set_State(ColumnMeasureState::Enum newState);

    int get_ActualIndex() { return m_actualIndex; }
    void set_ActualIndex(int actualIndex) { m_actualIndex = actualIndex; }

    void set_ActualWidth(float actualWidth);
    void set_DesiredWidth(float desiredWidth) { m_desiredWidth = desiredWidth; }
    void ResetPrivateData();
    float EnsureWidth(float width);

    ColumnMeasureState::Enum m_state;
    int m_actualIndex;
    float m_actualWidth;
    float m_desiredWidth;
    TRefCountedPtr<Binding> m_pDisplayMemberBinding;

    DependencyObject* m_pInheritanceContext;

};


}; // namespace AVUI

