#pragma once

namespace AVUI
{
    class DependencyObject;
    class UIElement;

    class Helper
    {
    public:
        static bool HasDefaultValue(const DependencyProperty& dp, DependencyObject* pDO, UIElement* pUIElement);
        static TRefCountedPtr<Object> FindNameInTemplate(String* pName, DependencyObject* pTemplatedParent);
        static DependencyObject* GetTemplatedParent(DependencyObject* pObject);
        static DependencyObject* FindMentor(DependencyObject* pDO);

 

    };
}
