#include <AVUICommon.h>
#include <AVUIRoutedEventArgs.h>


REGISTER_ELEMENT(AVUI::RoutedEventArgs);


namespace AVUI
{

void RoutedEventArgs::set_Source(Object* pSource)
{
    if (m_pSource == NULL && m_pOriginalSource == NULL)
    {
        m_pSource = m_pOriginalSource = pSource;
        OnSetSource(pSource);
    }
    else if (m_pSource != pSource)
    {
        m_pSource = pSource;
        OnSetSource(pSource);
    }
}



}
