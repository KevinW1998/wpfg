#pragma once

#include <AVUIListBoxItem.h>

namespace AVUI
{
    class Geometry3D;
    class DataTemplate;
    class ModelVisual3D;
}

class ListBox3DItem : public AVUI::ListBoxItem
{
public:
    friend class ListBox3D;
    DECLARE_ELEMENT(ListBox3DItem, AVUI::ListBoxItem);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(DataTemplate, AVUI::DataTemplate, NULL, AVUI::PropertyFlag::None);

    AVUI::TRefCountedPtr<AVUI::ModelVisual3D> GenerateModel();

};

