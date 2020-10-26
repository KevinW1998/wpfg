
#pragma once

#include <AVUIDefinitionBase.h>

namespace AVUI {

class Grid;

class ColumnDefinition : public DefinitionBase
{
    friend class ColumnDefinitionCollection;

public:
    ColumnDefinition() : DefinitionBase(true), m_pOwner(NULL) { }

    DECLARE_ELEMENT(ColumnDefinition, DefinitionBase);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Width,   GridLength, GridLength(), DependencyPropertyMetadata(PropertyFlag::None, OnPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MinWidth,   float, 0.0f, DependencyPropertyMetadata(PropertyFlag::None, OnPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MaxWidth,   float, Float::get_PositiveInfinity(), DependencyPropertyMetadata(PropertyFlag::None, OnPropertyChanged));

    float get_ActualWidth();

private:
    static void OnPropertyChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    void set_Owner(Grid* pOwner)
    {
        if(m_pOwner != NULL)
        {
            LibraryCriticalError();
        }

        m_pOwner = pOwner;
    }

    Grid* m_pOwner;

};



}; // namespace AVUI
