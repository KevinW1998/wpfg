#pragma once

#include <AVUIVisual.h>

namespace AVUI {
             
class VisualTreeHelper
{
public:

    static DependencyObject* GetParent(DependencyObject* pReference);

    static TRefCountedPtr<HitTestResult> HitTest(Visual* pReference, const Point& point)
    {
        return HitTest(pReference, point, true);
    }

    static void HitTest(Visual* pReference, HitTestFilterCallback* pFilterCallback, HitTestResultCallback* pResultCallback, HitTestParameters* pHitTestParameters)
    {
        pReference->HitTest(pFilterCallback, pResultCallback, pHitTestParameters);
    }
   
private:

    static TRefCountedPtr<HitTestResult> HitTest(Visual* pReference, const Point& point, bool include2DOn3D)
    {
        return pReference->HitTest(point, include2DOn3D);
    }

};


}; // namespace AVUI

