#include <AVUICommon.h>
#include <AVUIGridViewRowPresenter.h>
#include <AVUIUIElementCollection.h>
#include <AVUIGridViewColumnCollection.h>
#include <AVUIGridViewColumn.h>
#include <AVUITextBlock.h>
#include <AVUIContentPresenter.h>
#include <AVUIBinding.h>
#include <AVUILayoutManager.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridViewRowPresenter)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Content)
REGISTER_ELEMENT_END()

namespace AVUI
{

void GridViewRowPresenter::OnPreApplyTemplate()
{
    GridViewRowPresenterBase::OnPreApplyTemplate();
    if (get_NeedUpdateVisualTree())
    {
        get_InternalChildren()->Clear();
        GridViewColumnCollection* pColumns = get_Columns();
        if (pColumns != NULL)
        {
            for(int idx = 0; idx < pColumns->get_Count(); idx++)
            {
                get_InternalChildren()->Add(CreateCell((*pColumns)[idx]));
            }
        }
        set_NeedUpdateVisualTree(false);
    }
    m_viewPortValid = false;
}

TRefCountedPtr<UIElement> GridViewRowPresenter::CreateCell(GridViewColumn* pColumn)
{
    TRefCountedPtr<UIElement> pElement;
    Binding* pBinding = pColumn->get_DisplayMemberBinding();
    if (pBinding != NULL)
    {
        pElement = object_allocate<TextBlock>();
        pElement->set_DataContext(get_Content());
        pElement->SetBinding(TextBlock::get_TextProperty(), pBinding);

        ContentPresenter::BindToFontProperties(get_TemplatedParent(), pElement);
    }
    else
    {
        TRefCountedPtr<ContentPresenter> pPresenter = object_allocate<ContentPresenter>();
        pPresenter->set_Content(get_Content());

        DataTemplate* pCellTemplate = pColumn->get_CellTemplate();
        if (pCellTemplate != NULL)
        {
            pPresenter->set_ContentTemplate(pCellTemplate);
        }
        DataTemplateSelector* pCellTemplateSelector = pColumn->get_CellTemplateSelector();
        if (pCellTemplateSelector != NULL)
        {
            pPresenter->set_ContentTemplateSelector(pCellTemplateSelector);
        }

        pPresenter->BindTextPresenterToFontProperties(get_TemplatedParent());

        pElement = pPresenter;
    }
    ContentControl* pContentControl = object_cast<ContentControl>(get_TemplatedParent());
    if (pContentControl != NULL)
    {
        pElement->set_VerticalAlignment(pContentControl->get_VerticalContentAlignment());
        pElement->set_HorizontalAlignment(pContentControl->get_HorizontalContentAlignment());
    }
    pElement->set_Margin(Thickness(6, 0, 6, 0));

    return pElement;
}


Size GridViewRowPresenter::MeasureOverride(const Size& constraint)
{
    GridViewColumnCollection* pColumns = get_Columns();
    if (pColumns == NULL)
    {
        return Size();
    }
    UIElementCollection* pChildren = get_InternalChildren();
    float num = 0;
    float num2 = 0;
    float height = constraint.get_Height();

    bool flag = false;
    for(int columnIdx = 0; columnIdx < pColumns->get_Count(); columnIdx++)
    {
        GridViewColumn* pColumn = (*pColumns)[columnIdx];
        UIElement* pElement = (*pChildren)[pColumn->get_ActualIndex()]; 
        if (pElement != NULL)
        {
            float width = Float::Max(0, constraint.get_Width() - num2);
            if (pColumn->get_State() == ColumnMeasureState::Init || pColumn->get_State() == ColumnMeasureState::Headered)
            {
                if (!flag)
                {
                    EnsureDesiredWidthList();

                    get_LayoutManager()->add_LayoutUpdated(EventHandler::Create(this, &GridViewRowPresenter::OnLayoutUpdated));
                    flag = true;
                }

                pElement->Measure(Size(width, height));
                if (get_IsOnCurrentPage())
                {
                    pColumn->EnsureWidth(pElement->get_DesiredSize().get_Width());
                }
                m_desiredWidthList[pColumn->get_ActualIndex()] = pColumn->get_DesiredWidth();
                num2 += pColumn->get_DesiredWidth();
            }
            else if (pColumn->get_State() == ColumnMeasureState::Data)
            {
                width = Float::Min(width, pColumn->get_DesiredWidth());
                pElement->Measure(Size(width, height));
                num2 += pColumn->get_DesiredWidth();
            }
            else
            {
                width = Float::Min(width, pColumn->get_Width());
                pElement->Measure(Size(width, height));
                num2 += pColumn->get_Width();
            }
            num = Float::Max(num, pElement->get_DesiredSize().get_Height());
        }
    }
    m_isOnCurrentPageValid = false;
    return Size(num2 + 2, num);
}

Size GridViewRowPresenter::ArrangeOverride(const Size& arrangeSize)
{
    GridViewColumnCollection* pColumns = get_Columns();
    if (pColumns != NULL)
    {
        UIElementCollection* pChildren = get_InternalChildren();
        float x = 0;
        float width = arrangeSize.get_Width();
        for(int columnIdx = 0; columnIdx < pColumns->get_Count(); columnIdx++)
        {
            GridViewColumn* pColumn = (*pColumns)[columnIdx];
            UIElement* pElement = (*pChildren)[pColumn->get_ActualIndex()];

            if (pElement != NULL)
            {
                float num3 = Float::Min(width, (pColumn->get_State() == ColumnMeasureState::SpecificWidth) ? pColumn->get_Width() : pColumn->get_DesiredWidth());
                pElement->Arrange(Rect(x, 0, num3, arrangeSize.get_Height()));
                width -= num3;
                x += num3;
            }
        }
    }
    return arrangeSize;
}


void GridViewRowPresenter::OnLayoutUpdated(Object* pSender, EventArgs* pArgs)
{
    bool invalidateMeasure = false;

    GridViewColumnCollection* pColumns = get_Columns();

    for(int columnIdx = 0; columnIdx < pColumns->get_Count(); columnIdx++)
    {
        GridViewColumn* pColumn = (*pColumns)[columnIdx];

        if (pColumn->get_State() == ColumnMeasureState::SpecificWidth)
        {
            continue;
        }

        pColumn->set_State(ColumnMeasureState::Data);
        if (pColumn->get_ActualIndex() >= m_desiredWidthList.get_Count())
        {
            invalidateMeasure = true;
            break;
        }
        if (!Float::AreClose(pColumn->get_DesiredWidth(), m_desiredWidthList[pColumn->get_ActualIndex()]))
        {
            m_desiredWidthList[pColumn->get_ActualIndex()] = pColumn->get_DesiredWidth();
            invalidateMeasure = true;
        }
    }
    if (invalidateMeasure)
    {
        InvalidateMeasure();
    }

    get_LayoutManager()->remove_LayoutUpdated(EventHandler::Create(this, &GridViewRowPresenter::OnLayoutUpdated));
}

void GridViewRowPresenter::OnColumnPropertyChanged(GridViewColumn* pColumn, const Property* pProperty)
{
    int num;
    if (pProperty != &GridViewColumn::get_ActualWidthProperty() && ((num = pColumn->get_ActualIndex()) >= 0) && (num < get_InternalChildren()->get_Count()))
    {
        if (&GridViewColumn::get_WidthProperty() == pProperty)
        {
            InvalidateMeasure();
        }
        else if (&GridViewColumn::get_DisplayMemberBindingProperty() == pProperty)
        {
            UIElement* pElement = object_cast<UIElement>(get_InternalChildren()->GetItemAt(num));
            if (pElement != NULL)
            {
                BindingBase* pDisplayMemberBinding = pColumn->get_DisplayMemberBinding();
                if (pDisplayMemberBinding != NULL && pElement->IsType(GET_TYPE(TextBlock)))
                {
                    pElement->SetBinding(TextBlock::get_TextProperty(), pDisplayMemberBinding);
                }
                else
                {
                    RenewCell(num, pColumn);
                }
            }
        }
        else
        {
            ContentPresenter* pPresenter = object_cast<ContentPresenter>(get_InternalChildren()->GetItemAt(num));
            if (pPresenter != NULL)
            {
                if (&GridViewColumn::get_CellTemplateProperty() == pProperty)
                {
                    DataTemplate* pCellTemplate = pColumn->get_CellTemplate();
                    if (pCellTemplate == NULL)
                    {
                        pPresenter->ClearValue(ContentControl::get_ContentTemplateProperty());
                    }
                    else
                    {
                        pPresenter->set_ContentTemplate(pCellTemplate);
                    }
                }
                else if (&GridViewColumn::get_CellTemplateSelectorProperty() == pProperty)
                {
                    DataTemplateSelector* pCellTemplateSelector = pColumn->get_CellTemplateSelector();
                    if (pCellTemplateSelector == NULL)
                    {
                        pPresenter->ClearValue(ContentControl::get_ContentTemplateSelectorProperty());
                    }
                    else
                    {
                        pPresenter->set_ContentTemplateSelector(pCellTemplateSelector);
                    }
                }
            }
        }
    }
}
 

void GridViewRowPresenter::RenewCell(int index, GridViewColumn* pColumn)
{
    get_InternalChildren()->RemoveAt(index);
    get_InternalChildren()->Insert(index, CreateCell(pColumn));
}


};

