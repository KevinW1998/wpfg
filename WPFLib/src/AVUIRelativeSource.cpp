#include <AVUICommon.h>
#include <AVUIRelativeSource.h>

REGISTER_ELEMENT_BEGIN(AVUI::RelativeSource)

    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()

namespace AVUI {

void RelativeSource::set_AncestorLevel(int ancestorLevel)
{
    if (m_mode != RelativeSourceMode::FindAncestor)
    {
        if (ancestorLevel != 0)
        {
            LibraryCriticalError();
        }
    }
    else
    {
        if (ancestorLevel < 1)
        {
            LibraryCriticalError();
        }
        m_ancestorLevel = ancestorLevel;
    }
}

void RelativeSource::set_AncestorType(const Type* pType)
{
    if (IsUninitialized())
    {
        set_AncestorLevel(1);
    }
    if (m_mode != RelativeSourceMode::FindAncestor)
    {
        if (pType != NULL)
        {
            LibraryCriticalError();
        }
    }
    else
    {
        m_pAncestorType = pType;
    }
}

void RelativeSource::set_Mode(RelativeSourceMode::Enum mode)
{
    if (IsUninitialized())
    {
        InitializeMode(mode);
    }
    else if (mode != m_mode)
    {
        LibraryCriticalError();
    }
}

RelativeSource* RelativeSource::get_PreviousData()
{
    static RelativeSource previousData(RelativeSourceMode::PreviousData);

    return &previousData;
}

RelativeSource* RelativeSource::get_Self()
{
    static RelativeSource self(RelativeSourceMode::Self);

    return &self;
}

RelativeSource* RelativeSource::get_TemplatedParent()
{
    static RelativeSource templatedParent(RelativeSourceMode::TemplatedParent);

    return &templatedParent;
}

void RelativeSource::InitializeMode(RelativeSourceMode::Enum mode)
{
    m_mode = mode;
    m_ancestorLevel = 0;

    if (mode == RelativeSourceMode::FindAncestor)
    {
        m_ancestorLevel = 1;
    }
}

}; // namespace AVUI
