#pragma once

#include <AVUIDefinitionBase.h>

namespace AVUI {

class Grid;

class RowDefinition : public DefinitionBase
{
    friend class RowDefinitionCollection;

public:
    RowDefinition() : DefinitionBase(false), m_pOwner(NULL) { }

    DECLARE_ELEMENT(RowDefinition, DefinitionBase);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Height,   GridLength, GridLength(), DependencyPropertyMetadata(PropertyFlag::None, OnPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MinHeight,   float, 0.0f, DependencyPropertyMetadata(PropertyFlag::None, OnPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MaxHeight,   float, Float::get_PositiveInfinity(), DependencyPropertyMetadata(PropertyFlag::None, OnPropertyChanged));

    float get_ActualHeight();

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
