#include <AVUICommon.h>
#include <AVUIVisual.h>
#include <AVUIVisualTreeHelper.h>

namespace AVUI {

DependencyObject* VisualTreeHelper::GetParent(DependencyObject* pReference)
{
    if(is_type<Visual>(pReference))
    {
        return ((Visual*)pReference)->get_VisualParent();
    }

    /*
    if(is_type<Visual3D>(pReference))
    {
        return ((Visual3D*)pReference)->get_VisualParent();
    }
    */
    return NULL;
}


}; // namespace AVUI
 

